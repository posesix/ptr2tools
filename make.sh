# General Options
  # Which programs to compile
  export PROGS="ptr2int ptr2spm ptr2tex"

  # Source code directory
  export SOURCES="sources"

# Install options
  export INSTALL_DIR="/usr/bin"

# Compiler options
  export CC="gcc"
  export CXX="g++"
  export INCLUDE_DIRS=""
  export LIB_DIRS=""
  export MAKE="make"

  # You will need a compatible library for ptr2tex
  export IMPORT_PNG="-lpng"

  # Uncomment for WINDOWS builds, if necessary
# export OSFLAG="-D_WIN32"

export CFLAGS="-Os $INCLUDE_DIRS $LIB_DIRS $OSFLAG"

$MAKE $@

