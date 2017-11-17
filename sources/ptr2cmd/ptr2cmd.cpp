
typedef int (*cmd_function_t)(int argc, char *args[]);

struct cmd_t {
  const char *name;
  const char *usage;
  const char *desc;
  const char *aliases[4];
  int naliases;
  cmd_function_t fn;

  bool matches(const char *s) {
    if(streq(s, name)) return true;
    for(int i = 0; i < naliases; i += 1) {
      if(streq(s, aliases[i])) return true;
    }
    return false;
  };
  void printhelp(const char *prefix) {
    printf("%s %s %s\n", prefix, this->name, this->usage);
    printf("   %s\n", this->desc);
    printf("   ALIASES: ");
    for(int i = 0 ; i < naliases; i += 1) {
      printf("%s ", aliases[i]);
    }
  }
  inline int exec(int argc, char *args[]) {
    return this->fn(argc, args);
  }
};

const cmd_t CMD_EXTRACT = {
  "extract", "[tm0-folder] [tex0] [output-png]",
  "Extract tex0 from tm0 folder and output as RGBA PNG",
  {"e", "x", "get", "g"}, 4
};

const cmd_t CMD_INJECT = {
  "inject", "[tm0-folder] [tex0] [input-png]",
  "Inject tex0 into tm0 environment using 32-bit RGBA PNG",
  {"i", "put", "p"}, 3
};

const cmd_t CMD_EXTRACT_LIST = {
  "extract-list", "[tm0-folder] [list] <png-folder>",
  "Batch extract tex0s in list from tm0 folder and output as RGBA PNG",
  {"el", "xl", "get-list", "gl"}, 4
};

const cmd_t CMD_INJECT_LIST = {
  "inject-list", "[tm0-folder] [list] <png-folder>",
  "Batch inject tex0s in list into tm0 environment using 32-bit RGBA PNG",
  {"il", "put-list", "pl"}, 3
};


