# PTR2TOOLS
NOTE: This is a different version of PTR2Tools which fixes a whole lot of things.
The old version by posesix is unusable.
## Introduction

PTR2TOOLS is a collection of open source software for interoperability with PaRappa the Rapper 2. You may freely use, modify, and distribute the source code. Pull requests are welcome.

At the time of writing, the following functionality is available:
- INT Packer/Unpacker
- TM0 Encoder/Decoder
- SPM TEX0 Extraction

## Dependencies

The following libraries allow for a complete build of ptr2tools, whose usage may restrict licensing.
- libpng (PTR2TEX, [ http://www.libpng.org/pub/png/libpng.html ])

## Compiling

The bash script `make.sh` will set up the options for compiling and execute a make program. Configure ptr2tools by modifying `make.sh`.

Simply type `sh make.sh` to build every tool, and `sh make.sh install` to install all binaries into `INSTALL_DIR` (`/usr/bin` by default). You can build individual tools by specifying their name.
```
sh make.sh 
sh make.sh install
```

You can also use `sh make.sh strip` to `strip` the resulting binaries if you would like. (Do this before installing).

#Tutorial and Documentation

##Information

Today (2/25/18) is the day where I have fixed PTR2Tools. (Took me about 2 whole months)
Disregard he other tutorial I made before this one, it is very outdated and will provide false information.
This tutorial will just be an edited version of the tutorial that I had make before, although it is a bit more complex.
It will also update every now and then, due to me making changes.
------------------------------------
 
 
#Compiling and setup
1. Install MSYS2 and open the MSYS console.
2. Use the following commands (paste with shift+insert):

```pacman -S git
pacman -S gcc (RESTART AFTER THIS ONE, STUFF MAY BREAK (also ignore errors))
pacman -S make
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-libpng
```

Afterwards, use these:

```git clone https://github.com/MGRich/ptr2tools.git
cd ptr2tools
sh make.sh
exit
```

3. OPEN MINGW64. DO NOT OPEN MSYS.

TO FREE MINGW64:
1. Run "cd /".
2. From here you can CD to the C (or any) drive by doing "cd c"
Note: You'll have to use it every single time you startup.
##PTR2INT
###Commands
list [intfile]
   Print list of contents of an INT file.
   ALIASES: l ls
extract [intfile] [folder]
   Extract contents of an INT file to a folder.
   ALIASES: e ex x
create [intfile] [folder]
   Create an INT file using the contents of a folder.
   ALIASES: c make pack
optimize [folder] [out-neworderfile]
   Attempts to optimize the _order.txt file of a folder to achieve lzss compression.
   ALIASES: optimise
###Command arguments:
intfile: The INT file. It's best to have this in your cd directory.
folder: The folder the INT contents will be placed in.
folder (optimize): Th folder of which an _order.txt is located.
Example: ptr2int e TITLE.INT title
##PTR2SPM
###Commands
gtex0 [spmfile] [outfile]
   Extracts tex0 from an SPM model.
###Command arguments:
spmfile: The spm file.
outfile: The output file in .txt.
Example: ptr2spm gtex0 ant_ant.spm meme.txt
##PTR2TEX
---Commands---
extract [tm0-folder] [in-listfile] <png-folder>
   Batch extract textures from tm0-folder using listfile and output as RGBA PNG.
     If png-folder is not specified, the current directory is used for output.
   ALIASES: el xl get-list gl
---
inject [tm0-folder] [in-listfile] <png-folder>
   Batch inject textures into tm0-folder using listfile with 32-bit RGBA PNGs.
     If png-folder is not specified, the current directory is used as input.
   ALIASES: il put-list pl
**NOTE: This one can be a little confusing, so I'll try to explain it the best I can.**
###Commands defined:
extract: Extracts a list of TM0 textures.
inject: Puts in a list of TM0 textures.
###Command arguments:
tm0-folder: The folder that all the TM0s are in. It is named "TEXTURES" when the INT is extracted.
in-listfile: The .txt file with the tex0 IDs. See PTR2SPM: outfile.
png-folder: The folder with the PNGs to be injected in.
Example: ptr2tex extract TEXTURES meme.txt maymay