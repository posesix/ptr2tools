#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <vector>
#include <algorithm>
#include "spm.h"

#ifdef _WIN32
  #define PRIx64 "llx"
#endif

#define ERROR(s,...) fprintf(stderr, s, __VA_ARGS__)

int getfilesize(FILE *f) {
  int savepos = ftell(f);
  int ret;
  fseek(f, 0, SEEK_END);
  ret = ftell(f);
  fseek(f, savepos, SEEK_SET);
  return ret;
}

void loadfile(FILE *f, void *out) {
  int len = getfilesize(f);
  fread(out, 1, len, f);
  return;
}

void printhelp() {
  printf("ptr2spm [cmd] [args...]\n");
  printf("---Commands---\n");
  printf("gtx0 <spmfile> <outfile>\n");
}

bool streq(const char *s1, const char *s2) {
  if(0 == strcasecmp(s1, s2)) {
    return true;
  }
  return false;
}

std::vector<u64> tex0listfromspm(const void *spm, int spmlen) {
  int npoly = spm::getpolygoncount(spm, spmlen);
  int i;
  spm::polygonheader_t *poly;
  std::vector<u64> tex0list;

  for(i = 0; i < npoly; i += 1) {
    if(spm::getpolygonbyindex(spm, spmlen, i, &poly)) {
      u8 fmt = poly->format >> 24;
      if(fmt == 0x50) {
	u64 tex0v = spm::tex0frompolygon(poly);
	if(tex0list.end() == std::find(tex0list.begin(), tex0list.end(), tex0v)) {
	  tex0list.push_back(tex0v);
	}
      }
    }
  }
  return tex0list;
}

void gtx0(FILE *spmfile, FILE *outfile) {
  int spmlen = getfilesize(spmfile);
  void *spm = malloc(spmlen);
  fread(spm, 1, spmlen, spmfile);

  std::vector<u64> tex0list = tex0listfromspm(spm, spmlen);

  for(size_t i = 0; i < tex0list.size(); i += 1) {
    fprintf(outfile, "%016" PRIx64 "\n", tex0list[i]);
  }

  printf("Found %d tex0\n", int(tex0list.size()));

  return;
}

#define CMD(a) if(true == streq(#a,args[1]))

int main(int argc, char *args[]) {
  if(argc <= 1) {
    printhelp();
  } else {
    CMD(gtex0) {
      if(argc <= 3) {
	printf("gtex0 <spmfile> <outfile>\n");
	return 1;
      } else {
	FILE *spmfile = fopen(args[2], "rb");
	if(NULL == spmfile) {
	  ERROR("Couldn't open SPM file %s\n", args[2]);
	  return 1;
	}
	FILE *outfile = fopen(args[3], "w");
	if(NULL == outfile) {
	  ERROR("Couldn't open output file %s\n", args[3]);
	  return 1;
	}
	printf("spmfile = %s\n", args[2]);
	get_tex0_list(spmfile, outfile);
	fclose(spmfile);
	fclose(outfile);
      }
    } else {
      ERROR("ptr2spm:  Unknown command %s\n", args[1]);
      return 1;
    }
  }
  return 0;
}
