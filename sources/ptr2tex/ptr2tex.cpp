#include "gs.h"
#include "tim2.h"
#include "tim2upload.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <dirent.h>
#include "pnghelper.h"
#include "gsutil.inc"
#include <vector>
#include <string>

#ifdef _WIN32
  #define PRIx64 "llx"
#endif

#define ERROR(s,...) fprintf(stderr, s, __VA_ARGS__);
#define MAX_PATH 256

struct tm0info_t {
  std::string name;
  gs::tex0_t tex0;
  void setfilename(const char *newname);
};

void tm0info_t::setfilename(const char *newname) {
  this->name = std::string(newname);
}

struct tm0env_t {
  const char *path;
  std::vector<tm0info_t> tm0s;
  bool load(const char *path);
  bool save(const char *path);
};

bool streq(const char *s1, const char *s2) {
  if(0 == strcasecmp(s1, s2)) return true;
  return false;
}
void printhelp() {
  printf("ptr2tex [command] [args...]\n");
  printf("---Commands---\n");
  printf("extract [tm0-folder] [tex0] [output-filename]\n");
  printf("inject [tm0-folder] [tex0] [input-filename]\n");
  printf("extract-list [tm0-folder] [list-filename] <png-folder>\n");
  printf("inject-list [tm0-folder] [list-filename] <png-folder>\n");
}

const char *extof(const char *filename) {
  return (strrchr(filename, '.'));
}

int getfilesize(FILE *f) {
  int spos = ftell(f);
  fseek(f, 0, SEEK_END);
  int len = ftell(f);
  fseek(f, spos, SEEK_SET);
  return len;
}

bool tm0env_t::save(const char *path) {
  void *tm0;
  char lbuf[MAX_PATH];

  for(tm0info_t &tm0info : this->tm0s) {
    const char *tm0filename = tm0info.name.c_str();
    snprintf(lbuf, sizeof(lbuf), "%s/%s", path, tm0filename);
    printf("TIM2_SAVE: %s\n", lbuf);
    FILE *f = fopen(lbuf, "rb");
    if(NULL == f) {
      ERROR("   Could not open %s for reading\n", lbuf);
      continue;
    }
    int len = getfilesize(f);
    tm0 = malloc(len);
    fread(tm0, 1, len, f);
    fclose(f);

    if(false == tim2download(tm0)) {
      ERROR("   Failed to download TIM2 %s\n", lbuf);
      free(tm0);
      continue;
    }

    f = fopen(lbuf, "wb");
    if (NULL == f) {
      ERROR("   Couldn't open %s for writing\n", lbuf);
      free(tm0);
      continue;
    }
    fwrite(tm0, 1, len, f);
    fclose(f);
    free(tm0);
  }

  return true;
}

bool tm0env_t::load(const char *path) {
  void *tm0;
  struct dirent *de;
  DIR *dir = opendir(path);
  char lbuf[MAX_PATH];

  if(NULL == dir) {
    fprintf(stderr, "Failed to open tm0 folder %s\n", path);
    return false;
  }

  while((de = readdir(dir)) != NULL) {
    const char *filename = de->d_name;
    snprintf(lbuf, sizeof(lbuf), "%s/%s", path, filename);
    /*
    struct stat st;
    if(stat(lbuf, &st) == -1) {
      fprintf(stderr, "Couldn't stat file: %s\n", filename);
      continue;
    }*/

    if(streq(extof(filename), ".tm0")) {
      tm0info_t tm0info;
      FILE *tm0file = fopen(lbuf, "rb");
      if(NULL == tm0file) {
	ERROR("Could not open TIM2 %s\n", lbuf);
	continue;
      }
      printf("TIM2_LOAD: %s\n", lbuf);
      int len = getfilesize(tm0file);
      tm0 = malloc(len);
      fread(tm0, 1, len, tm0file);
      if(false == tim2upload(tm0)) {
	ERROR("Failed to upload TIM2 %s\n", lbuf);
      }
      free(tm0);
      fclose(tm0file);
      tm0info.setfilename(filename);
      this->tm0s.push_back(tm0info);
    }
  }
  return true;
}


void extract_from_tex0(gs::tex0_t tex0, u32 *outpixels) {
  int tw,th; wh_from_tex0(tex0, tw, th);
  int npixels = tw * th;
  int pixstride = calc_pixel_stride(tex0.psm);
  void *pixels = (void*)(malloc(npixels * pixstride));
  int clutsize;
  void *clut = NULL;

  gs::ReadTexture(tex0.psm, tex0.tb_addr, tex0.tb_width, 0, 0, tw, th, pixels);

  if(ispalettetype(tex0.psm)) {
    clutsize = calc_clut_size(tex0.psm, tex0.clut_pixmode);
    clut = (void*)(malloc(clutsize));
    gs::ReadCLUT(tex0.psm, tex0.clut_pixmode, tex0.cb_addr, 1, 0, 0, calc_ncolors(tex0.psm), 1, clut);
  }

  convert_pixels_to_rgba(tex0.psm, pixels, tw, th, tex0.clut_pixmode, clut, outpixels);

  free(clut);
  free(pixels);
  return;
}


void inject_tex(gs::tex0_t tex0, u32 *inpixels) {
  int tw,th; wh_from_tex0(tex0, tw, th);
  int npixels = tw * th;
  u32 clut32[256];
  void *clut = (void*)(clut32);
  void *pixels = malloc(npixels * calc_pixel_stride(tex0.psm));

  convert_pixels_to_tex0(tex0.psm, inpixels, tw, th, tex0.clut_pixmode, clut, pixels);

  if(ispalettetype(tex0.psm)) {
    gs::WriteCLUT(tex0.psm, tex0.clut_pixmode, tex0.cb_addr, 1, 0, 0, calc_ncolors(tex0.psm), 1, clut);
  }

  gs::WriteTexture(tex0.psm, tex0.tb_addr, tex0.tb_width, 0, 0, tw, th, pixels);

  free(pixels);

  return;
}

void ptr2tex_alloc_out_image_from_tex0(gs::tex0_t tex0, u32 **outpixels) {
  int tw,th; wh_from_tex0(tex0, tw, th);
  int npixels = tw * th;
  *outpixels = (u32*)(malloc(npixels * sizeof(**outpixels)));
}

#define CMD(x) if(streq(#x, args[1]))
#define REQUIRE(x) if((argc <= (x+1)))
int main(int argc, char *args[]) {
  if(!tim2::check_install()) {
    printf("Bad compile\n");
    return 1;
  }

  if(argc <= 1) {
    printhelp();
  } else {
    CMD(extract) {
      REQUIRE(3) {
	printf("ptr2tex extract [tm0folder] [tex0] [outfile]\n");
	return 0;
      }
      gs::tex0_t tex0;
      tm0env_t tm0env;
      if(tm0env.load(args[2]) == false) {
	return 1;
      }
      sscanf(args[3], "%016" PRIx64, &tex0.value);
      printf("Tex0=(%d,%d,%d)\n", tex0.psm, tex0.clut_pixmode, tex0.tex_width);

      int tw,th; wh_from_tex0(tex0, tw, th);
      int npixels = tw * th;
      u32 *outpixels = (u32*)(malloc(npixels * sizeof(*outpixels)));

      extract_from_tex0(tex0, outpixels);

      FILE *outfile = fopen(args[4], "wb");
      if(NULL == outfile) {
	ERROR("Could not open output PNG %s\n", args[4]);
	return 1;
      }
      pngwrite(outfile, tw, th, outpixels);
      fclose(outfile);
      
      free(outpixels);

      return 0; 
    } else CMD(extract-list) {
      REQUIRE(2) {
	printf("ptr2tex extract-list [tm0folder] [list] <png-folder>\n");
	printf("   If png-folder is not specified, current directory is used\n");
	return 0;
      }
      const char *tm0path = args[2];
      const char *listfilename = args[3];
      const char *pngfolder = (argc > 4) ? args[4] : NULL;
      gs::tex0_t tex0;
      tm0env_t tm0env;
      if(false == tm0env.load(tm0path)) {
	return 1;
      }
      char lbuf[64];
      FILE *listfile = fopen(listfilename, "r");
      if(NULL == listfile) {
	ERROR("Could not open list file %s\n", listfilename);
	return 1;
      }
      int ntex = 0;
      while(!feof(listfile)) {
	int err = fscanf(listfile, "%016" PRIx64 "\n", &tex0.value);
	if(err < 1) continue; //Did not return tex0 value
	int tw,th; wh_from_tex0(tex0, tw, th);
	int npixels = tw * th;
	u32 *outpixels = (u32*)(malloc(npixels * sizeof(*outpixels)));

	if(pngfolder != NULL) {
	  snprintf(lbuf, sizeof(lbuf), "%s/%d.png", pngfolder, ntex);
	} else {
	  snprintf(lbuf, sizeof(lbuf), "%d.png", ntex);
	}

	printf("EXTRACT: %016" PRIx64 " -> %s\n", tex0.value, lbuf);
	extract_from_tex0(tex0, outpixels);
	
	ntex += 1;
	FILE *outfile = fopen(lbuf, "wb");
	if(outfile == NULL) {
	  ERROR("Could not open PNG file %s\n", lbuf);
	  free(outpixels);
	  continue;
	}
	pngwrite(outfile, tw, th, outpixels);
	fclose(outfile);

	free(outpixels);
      }
      printf(" %d textures\n", ntex);
      return 0;
    } else CMD(inject) {
      REQUIRE(3) {
	printf("ptr2tex inject [tm0-folder] [tex0] [pngfile]\n");
	return 0;
      }
      gs::tex0_t tex0;
      tm0env_t tm0env;
      if(false == tm0env.load(args[2])) {
	return 1;
      }
      sscanf(args[3], "%016" PRIx64, &tex0.value);
      
      int tw,th; wh_from_tex0(tex0, tw, th);
      int npixels = tw * th;
      u32 *inpixels = (u32*)(malloc(npixels * sizeof(*inpixels)));

      FILE *infile = fopen(args[4], "rb");
      if(NULL == infile) {
	ERROR("Could not open input list file %s\n", args[4]);
	return 1;
      }
      pngread(infile, inpixels);
      fclose(infile);

      inject_tex(tex0, inpixels);

      tm0env.save(args[2]);
      printf("Injected %016" PRIx64, tex0.value);
      free(inpixels);
      return 0;
    } else CMD(inject-list) {
      REQUIRE(2) {
	printf("ptr2tex inject-list [tm0-folder] [list] <png-folder>\n");
	printf("   If png-folder is not specified, current directory is used.\n");
	return 0;
      }
      const char *pngfolder = (argc > 4) ? args[4] : NULL;
      gs::tex0_t tex0;
      tm0env_t tm0env;
      if(false == tm0env.load(args[2])) {
	return 1;
      }
      char lbuf[64];
      FILE *listfile = fopen(args[3], "r");
      if(NULL == listfile) {
	ERROR("Could not open list file %s\n", args[3]);
	return 1;
      }
      int ntex = 0;
      while(!feof(listfile)) {
	int err = fscanf(listfile, "%016" PRIx64 "\n", &tex0.value);
	if(err < 1) continue;
	int tw,th; wh_from_tex0(tex0,tw,th);
	int npixels = tw * th;
	u32 *inpixels = (u32*)(malloc(npixels * sizeof(*inpixels)));

	if(pngfolder == NULL) {
  	  snprintf(lbuf, sizeof(lbuf), "%d.png", ntex);
	} else {
	  snprintf(lbuf, sizeof(lbuf), "%s/%d.png", pngfolder, ntex);
	}
	FILE *infile = fopen(lbuf, "rb");
	if(NULL == infile) {
	  fprintf(stderr, "Couldn't open file %s\n", lbuf);
	  free(inpixels);
	  ntex++;
	  continue;
	}
	pngread(infile, inpixels);
	fclose(infile);

	printf("INJECT: %016" PRIx64 " [%s]\n", tex0.value, lbuf);

	inject_tex(tex0, inpixels);

	free(inpixels);
	ntex++;
      }
      tm0env.save(args[2]);
      printf("Uploaded %d textures\n", ntex);
      return 0;
    } else {
      ERROR("ptr2tex:  Unknown command: %s\n", args[1]);
      return 1;
    }
  }
  return 0; 
}
