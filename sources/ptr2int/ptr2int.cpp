#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <algorithm>
#include <time.h>
#include "types.h"
#include "int.h"
extern "C" {
#include "lzss.h"
}
#include <ptr2common.h>
#include <ptr2cmd.h>

#ifdef _WIN32
  #include <direct.h>
  #define mkdir(x,y) _mkdir(x)
#endif

#define INFO(s,...) printf(s, __VA_ARGS__);
#define ERROR(s,...) fprintf(stderr, s, __VA_ARGS__);
#define FATAL(ec, s,...) fprintf(stderr, s, __VA_ARGS__); exit(ec);

static int cmd_list(int argc, char *args[]);
static int cmd_extract(int argc, char *args[]);
static int cmd_create(int argc, char *args[]);
static int cmd_optimize(int argc, char *args[]);

cmd_t commands[] = {
  { "list", "[intfile]",
    "Print list of contents of an INT file.",
    {"l", "ls"}, 2, cmd_list
  },
  { "extract", "[intfile] [folder]",
    "Extract contents of an INT file to a folder.",
    {"e", "ex", "x"}, 3, cmd_extract
  },
  { "create", "[intfile] [folder]",
    "Create an INT file using the contents of a folder.",
    {"c", "make", "pack"}, 3, cmd_create
  },
  { "optimize", "[folder] [out-neworderfile]",
    "Attempts to optimize the _order.txt file of a folder to achieve lzss compression.
	Rich's note: I RECOMMEND NOT USING THIS.",
    {"optimise"}, 1, cmd_optimize
  }
};

#define assertheader(i, s, ...) if(false == ptr2int::checkheader((const void*)(i))) { \
  fprintf(stderr, s, __VA_ARGS__); \
  return 1; \
}

bool makedir(const char *newdir) {
  struct stat st;
  int err = stat(newdir, &st);
  if(err == -1) {
    if(mkdir(newdir, S_IRWXU) != 0) {
      ERROR("Error making dir %s\n", newdir);
      return false;
    }
  } else if(0 == err) {
    if(S_ISDIR(st.st_mode)) {
      return true;
    } else {
      return false;
    }
  }
  return true;
}

struct intfile_t {
  intfile_t(std::string fn, int filesize, int fdp);
  std::string filename;
  int filesize;
  int offset;
};

intfile_t::intfile_t(std::string fn, int filesize, int fdp) :
  filename(fn), filesize(filesize), offset(fdp) {}

struct intsection_offset_table_t {
  u32 hdr;
  u32 offsets;
  u32 filename_table;
  u32 characters;
  u32 lzss;
  u32 end;
};

#define ALIGN(x, y) (((x) + (y-1)) & (~((y)-1)))

intsection_offset_table_t get_int_section_offsets(std::vector<intfile_t> &intfiles, int lzss_size) {
  intsection_offset_table_t r;
  r.hdr = 0;
  r.offsets = r.hdr + sizeof(ptr2int::header_t);
  r.filename_table = r.offsets + (sizeof(u32) * (intfiles.size() + 1));
  r.filename_table = ALIGN(r.filename_table, 0x10);
  r.characters = r.filename_table + (sizeof(ptr2int::filename_entry_t) * intfiles.size());
  size_t fnsize = 0;
  for(intfile_t &intfile : intfiles) {
    fnsize += intfile.filename.length() + 1;
  }
  r.lzss = ALIGN(r.characters + fnsize, 0x800);
  r.end = ALIGN((r.lzss + lzss_size), 0x800);
  return r;
}

size_t calc_int_section_size(std::vector<intfile_t> &intfiles, int lzss_size) {
  return get_int_section_offsets(intfiles, lzss_size).end;
}

void build_int_section(int restype, std::vector<intfile_t> &intfiles, const byte *lzss, int lzss_size, byte *out) {
  intsection_offset_table_t r = get_int_section_offsets(intfiles, lzss_size);
  ptr2int::header_t *hdr = (ptr2int::header_t*)(out + r.hdr);
  u32 *offsets = (u32*)(out + r.offsets);
  ptr2int::filename_entry_t *fentries = (ptr2int::filename_entry_t*)(out + r.filename_table);
  char *characters = (char*)(out + r.characters);
  ptr2int::lzss_header_t *lzss_sec = (ptr2int::lzss_header_t*)(out + r.lzss);

  memset(out, 0, r.end);

  hdr->unk[0] = 0;
  hdr->unk[1] = 0;
  hdr->lzss_section_size = r.end - r.lzss;
  hdr->resourcetype = restype;
  hdr->fntablesizeinbytes = r.lzss - r.filename_table;
  hdr->magic = INTHDR_MAGIC;
  hdr->fntableoffset = r.filename_table;
  hdr->filecount = intfiles.size();

  u32 offset = 0;
  u32 fnoffs = 0;
  for(u32 i = 0; i < intfiles.size(); i += 1) {
    intfile_t &intfile = intfiles[i];
    offsets[i] = offset;
    fentries[i].offset = fnoffs;
    fentries[i].sizeof_file = intfile.filesize;
    size_t fnlen = intfile.filename.length() + 1;
    const char *cstr = intfile.filename.c_str();
    memcpy(characters+fnoffs, cstr, fnlen);
    
    offset += ALIGN(intfile.filesize, 0x10);
    fnoffs += fnlen;
  }
  offsets[intfiles.size()] = offset;

  lzss_sec->uncompressed_size = offset;
  lzss_sec->compressed_size = lzss_size;
  memcpy(lzss_sec->data, lzss, lzss_size);

  return;
}

bool direxists(const char *dirname) {
  struct stat st;
  if(stat(dirname, &st) != 0) {
    return false;
  }
  if(S_ISDIR(st.st_mode)) {
    return true;
  } else {
    return false;
  }
}

bool isfile(const char *fn) {
  struct stat st;
  if(stat(fn, &st) != 0) {
    return false;
  }
  if(S_ISREG(st.st_mode)) {
    return true;
  } else {
    return false;
  }
}

struct resfile_iterator_t {
  virtual ~resfile_iterator_t() {};
  virtual const char *next() = 0;
};

struct resfile_dir_iterator_t : resfile_iterator_t { 
  DIR *dir;
  struct dirent *de;
  resfile_dir_iterator_t(DIR *_dir) : dir(_dir) {};
  ~resfile_dir_iterator_t() { closedir(dir); }
  const char *next();
};

const char *resfile_dir_iterator_t::next() {
  de = readdir(dir);
  if(de == NULL) return NULL;
  return de->d_name;
}

struct resfile_order_iterator_t : resfile_iterator_t {
  FILE *orderfile;
  char buf[256];
  resfile_order_iterator_t(FILE *f) : orderfile(f) {}
  ~resfile_order_iterator_t() { this->close(); }
  const char *next();
  void close() { if(NULL != orderfile) { fclose(orderfile); orderfile = NULL; } return; }
};

const char *resfile_order_iterator_t::next() {
  buf[0] = char(0);
  while(!feof(orderfile)) {
    fscanf(orderfile, "%s\n", buf);
    if(buf[0] == char(0)) continue;
    return buf;
  }
  return NULL;
}

resfile_iterator_t *openiterator(const char *dirname) {
  char lbuf[256];
  snprintf(lbuf, sizeof(lbuf), "%s/_order.txt", dirname);
  if(isfile(lbuf)) {
    FILE *orderfile = fopen(lbuf, "r");
    if(NULL != orderfile) {
      return new resfile_order_iterator_t(orderfile);
    }
  }
  DIR *dir = opendir(dirname);
  if(NULL == dir) {
    return NULL;
  }
  return new resfile_dir_iterator_t(dir);
}

int pad_folderdata(byte *folderdata, int start, int end) {
  int remain = end - start;
  while(remain > 0) {
    //TODO: Repeating last few bytes of file might help compression
    //      instead of zero-filling. The official INT packer 
    //      zero-fills.
    folderdata[end-remain] = 0; remain--;
  }
  return (end-start);
}

#define REQUIRE(x,c) if(argc < x) { commands[c].printhelp(""); return 1; }

static int cmd_list(int argc, char *args[]) {
  REQUIRE(1,0);

  const char *intfilename = args[0];

  FILE *f = fopen(intfilename, "rb");
  if(NULL == f) {
    ERROR("Could not open INT file %s\n", intfilename);
    return 1;
  }
  int len = getfilesize(f);
  void *mint = malloc(len);
  fread(mint, 1, len, f);
  fclose(f);

  assertheader(mint, "Not an INT file %s\n", intfilename);

  ptr2int::header_t *hdr = ptr2int::getheader(mint);

  while(hdr->resourcetype != INT_RESOURCE_END) {
    ptr2int::filename_entry_t *entries = ptr2int::getfilenameentries(hdr);
    char *characters = ptr2int::getfilenames(hdr);
    for(u32 i = 0; i < hdr->filecount; i += 1){
      printf("%s - %d bytes\n", characters + entries[i].offset, entries[i].sizeof_file);
    }
    hdr = ptr2int::getnextheader(hdr);
  }
  free(mint);
  printf("Done.\n");
  return 0;
}

static int cmd_extract(int argc, char *args[]) {
  REQUIRE(2,1);
  const char *intfilename = args[0];
  const char *outputfoldername = args[1];
  FILE *f = fopen(intfilename, "rb");
  int len = getfilesize(f);
  void *mint = malloc(len);
  fread(mint, 1, len, f);

  assertheader(mint, "Not an INT file %s\n", intfilename);

  INFO("INFLATE %s\n", intfilename);
  ptr2int::header_t *hdr = ptr2int::getheader(mint);
  byte *history = (byte*)(malloc(4096));

  while(hdr->resourcetype != INT_RESOURCE_END) {
    const char *restypename = ptr2int::typenames[hdr->resourcetype];
    printf("INFLATE: Section %s\n", restypename);
    ptr2int::lzss_header_t *lzss = ptr2int::getlzssheader(hdr);
    byte *uncompressed = (byte*)(malloc(lzss->uncompressed_size));
    byte *compressed = lzss->data;
    memset(history,0,4096);
    lzss_decompress(12,4,2,2,history,compressed,lzss->compressed_size,uncompressed,lzss->uncompressed_size);
    ptr2int::filename_entry_t *entries = ptr2int::getfilenameentries(hdr);
    u32 *fileoffsets = ptr2int::getfiledataoffsets(hdr);
    char *fnchars = ptr2int::getfilenames(hdr);
    char lbuf[256];

    const char *dirname = outputfoldername;
    makedir(dirname);

    printf("Extracting %d files (TYPE: %s)\n", hdr->filecount, restypename);

    snprintf(lbuf, sizeof(lbuf), "%s/%s", dirname, restypename);
    makedir(lbuf);

    snprintf(lbuf, sizeof(lbuf), "%s/%s/_order.txt", dirname, restypename);
    FILE *orderfile = fopen(lbuf, "w");
    for(u32 i = 0; i < hdr->filecount; i += 1) {
      ptr2int::filename_entry_t &entry = entries[i];
      const char *filename = fnchars + entry.offset;
      snprintf(lbuf, sizeof(lbuf), "%s/%s/%s", dirname, restypename, filename);

      printf("WRITE: %s (%d bytes)\n", filename, entry.sizeof_file);
      FILE *outfile = fopen(lbuf, "wb");
      if(NULL == outfile) {
        INFO("Couldn't open %s...\n", lbuf);
        continue;
      }
      fwrite(uncompressed + fileoffsets[i], 1, entry.sizeof_file, outfile);
      fclose(outfile);
      fprintf(orderfile, "%s\n", filename);
    }
    fclose(orderfile);
    hdr = ptr2int::getnextheader(hdr);
    free(uncompressed);
  }
  free(history);
  printf("Done.\n");
  return 0;
}

static int cmd_create(int argc, char *args[]) {
  REQUIRE(2,2);
  char lbuf[256];
  FILE *outfile;
  const char *intname = args[0];
  const char *dirname = args[1];
  std::vector<intfile_t> intfiles;
  outfile = fopen(intname, "wb");
  if(NULL == outfile) {
    ERROR("CREATE: Could not open target %s\n", intname);
    return 1;
  }
  if(false == direxists(dirname)) {
    ERROR("CREATE: Could not find directory %s\n", dirname);
    return 1;
  }

  byte *history = (byte*)(malloc(4096*2)); //For lzss compression
  for(int i = 1; i < INT_RESOURCE_TYPE_COUNT; i += 1) {
    const char *restypename = ptr2int::typenames[i];
    snprintf(lbuf, sizeof(lbuf), "%s/%s", dirname, restypename);
    printf("Checking for %s... ", restypename);
    if(false == direxists(lbuf)) {
      printf("no\n"); continue;
    } else {
      printf("yes\n");
      intfiles.clear();

      resfile_iterator_t *iter = openiterator(lbuf);
      int folderlen = 0;
      if(iter == NULL) {
        fprintf(stderr, "But I couldn't open it :(\n");
        continue;
      }

      byte *folderdata = (byte*)(malloc(4));
      const char *filename;
      while((filename = iter->next()) != NULL) {
        snprintf(lbuf, sizeof(lbuf), "%s/%s/%s", dirname, restypename, filename);
        printf("CHECK: %s\n", lbuf);
        if(false == isfile(lbuf)) {
          continue;
        }
        printf("ADD: %s\n", filename);
        FILE *f = fopen(lbuf, "rb");
        if(NULL == f) {
          fprintf(stderr, "   Could not open %s\n", lbuf);
          continue;
        }
        int len = getfilesize(f);
        int fdp = folderlen;
        folderlen = ALIGN(folderlen + len, 0x10);
        folderdata = (byte*)(realloc(folderdata, folderlen));
        fread(folderdata + fdp, 1, len, f);
        pad_folderdata(folderdata, fdp+len, folderlen);
        fclose(f);
        intfiles.push_back(intfile_t(std::string(filename), len, fdp));
      }
      delete iter;

      printf("COMPRESS: Input %d bytes\n", folderlen);
      memset(history,0,4096*2);
      int complen = lzss_compress(12,4,2,2,history,folderdata,folderlen,NULL);

      printf("COMPRESS: Output %d bytes\n", complen);
      memset(history,0,4096*2);
      byte *lzss_data = (byte*)(malloc(complen));
      lzss_compress(12,4,2,2,history,folderdata,folderlen,lzss_data);
      free(folderdata);

      printf("Building %s section\n", restypename);
      int seclen = calc_int_section_size(intfiles, complen);

      printf("  %d bytes\n", seclen);
      byte *secdata = (byte*)(malloc(seclen));
      build_int_section(i, intfiles, lzss_data, complen, secdata);

      printf("Writing %d bytes\n", seclen);
      fwrite(secdata, 1, seclen, outfile);

      free(secdata);
    }
  }


  printf("Finalizing\n");
  fwrite(&ptr2int::nullhdr, sizeof(ptr2int::nullhdr), 1, outfile);

  fclose(outfile);
  printf("Done.\n");
  free(history);
  return 0;
}

static int cmd_optimize(int argc, char *args[]) {
  REQUIRE(2,3);
  const char *foldername = args[0];
  const char *outfilename = args[1];
  char lbuf[256];
  byte *history = (byte*)(malloc(4096*2));
  std::vector<intfile_t> intfiles;
  std::vector<int> indices;

  FILE *outfile = fopen(outfilename, "a");
  if(outfile == NULL) {
    ERROR("Couldn't open target file %s\n", outfilename);
    return 1;
  }
  fclose(outfile);

  snprintf(lbuf, sizeof(lbuf), "%s/_order.txt", foldername);
  FILE *orderfile = fopen(lbuf, "r");
  if(NULL == orderfile) {
    ERROR("Couldn't find order in %s\n", lbuf);
    return 1;
  }
  resfile_order_iterator_t iter(orderfile);

  const char *filename;
  int i = 0;
  byte *orig_folderdata = (byte*)(malloc(4));
  int orig_folderlen = 0;
  while((filename = iter.next()) != NULL) {
    snprintf(lbuf, sizeof(lbuf), "%s/%s", foldername, filename);
    FILE *f = fopen(lbuf, "rb");
    if(NULL == f) {
      fprintf(stderr, "Couldn't find file %s\n", lbuf);
      continue;
    }
    printf("LOAD: %s\n", filename);
    int len = getfilesize(f);
    int fdp = orig_folderlen;
    orig_folderlen += ALIGN(len, 0x10);
    orig_folderdata = (byte*)(realloc(orig_folderdata, orig_folderlen));
    fread(orig_folderdata + fdp, 1, len, f);
    pad_folderdata(orig_folderdata, fdp + len, orig_folderlen);
    fclose(f);
    indices.push_back(i);
    intfiles.push_back(intfile_t(std::string(filename), len, fdp));
    i++;
  }
  iter.close();
      
  printf("%d files using %d bytes\n", u32(intfiles.size()), orig_folderlen);
  printf("Compressing...\n");
  memset(history, 0, 4096*2);
  int orig_complen = lzss_compress(12,4,2,2,history,orig_folderdata,orig_folderlen,NULL);
  int ccomplen = orig_complen;
  printf("Original compressed size is %d bytes\n", orig_complen);

  int attempts = 1;

  for(;;) {
    byte *folderdata = (byte*)(malloc(4));
    int folderlen = 0;
    int fdp = 0;
    int len;
    printf("\nAttempt %d...\n", attempts);
    srand(time(NULL));
    std::random_shuffle(indices.begin(), indices.end());
    for(i = 0; i < indices.size(); i += 1) {
      intfile_t &intfile = intfiles[indices[i]];
      len = intfile.filesize;
      fdp = folderlen;
      folderlen += ALIGN(len, 0x10);
      folderdata = (byte*)(realloc(folderdata, folderlen));
      memcpy(folderdata + fdp, orig_folderdata + intfile.offset, intfile.filesize);
      pad_folderdata(folderdata, fdp + len, folderlen);
    }
    if(folderlen != orig_folderlen) {
      printf("   Error when making new folderdata!!!\n");
      printf("   %d != %d\n", orig_folderlen, folderlen);
      getchar();
      free(folderdata);
      continue;
    }
    memset(history, 0, 4096*2);
    int newcomplen = lzss_compress(12,4,2,2,history,folderdata,folderlen,NULL);
    if(newcomplen < ccomplen) {
      printf("   Saved %d bytes (%d from original)\n", (ccomplen - newcomplen), (newcomplen - orig_complen));
      ccomplen = newcomplen;
      outfile = fopen(outfilename, "w");
      if(NULL == outfile) {
        ERROR("Couldn't open target file %s\n", outfilename);
        return 1;
      }
      for(i = 0; i < indices.size(); i += 1) {
        fprintf(outfile, "%s\n", intfiles[indices[i]].filename.c_str());
      }
      fclose(outfile);
      printf("   Wrote %s\n", outfilename);
    } else {
      printf("   Failed with %d bytes\n", newcomplen);
    }
    free(folderdata);
    attempts += 1;
  }

  free(history);
  free(orig_folderdata);
  return 0;
}

int main(int argc, char *args[]) {
  if(ptr2int::checkinstall() == false) {
    fprintf(stderr, "Bad compile\n");
    return 2;
  }
  
  if(argc <= 1) {
    for(cmd_t &cmd : commands) {
      cmd.printhelp("");
    }
    return 1;
  } else {
    for(cmd_t &cmd : commands) {
      if(cmd.matches(args[1])) {
	return cmd.exec(argc-2, args+2);
      }
    }
    printf("ptr2int:   Unknown command %s\n", args[1]);
    return 1;
  }
}
