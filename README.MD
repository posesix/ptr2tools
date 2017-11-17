# PTR2TOOLS

## Introduction

PTR2TOOLS is a collection of open source software for interoperability with PaRappa the Rapper 2. You may freely use, modify, and distribute the source code. Pull requests are welcome.

At the time of writing, the following functionality is available:
- INT Packer/Unpacker
- TM0 Encoder/Decoder
- SPM TEX0 Extraction

## Dependencies

The following libraries allow for a complete build of ptr2tools, whose usage may restrict licensing.
- libpng (PTR2TEX, [http://www.libpng.org/pub/png/libpng.html])

## Compiling

The bash script `make.sh` will set up the options for compiling and execute a make program. Configure ptr2tools by modifying `make.sh`.

Simply type `sh make.sh` to build every tool, and `sh make.sh install` to install all binaries into `INSTALL_DIR` (`/usr/bin` by default). You can build individual tools by specifying their name.
```
sh make.sh 
sh make.sh install
```

You can also use `sh make.sh strip` to `strip` the resulting binaries if you would like. (Do this before installing).

