# PTR2TOOLS
NOTE: This is a different version of PTR2Tools which fixes a whole lot of things.
The old version by posesix is unusable.
Old README at the bottom.

# Tutorial and Documentation

## Information

Today (2/25/18) is the day where I have fixed PTR2Tools. (Took me about 2 whole months)
Disregard he other tutorial I made before this one, it is very outdated and will provide false information.
This tutorial will just be an edited version of the tutorial that I had make before, although it is a bit more complex.
It will also update every now and then, due to me making changes.
 
# Compiling and setup
TO FREE MINGW:
1. Run "cd /".
2. From here you can CD to the C (or any) drive by doing "cd c"
3. You have to do this every time you start up MINGW.
---
1. [Install MSYS2](https://mysys2.org/) and open the MINGW (this tutorial uses 64) console.
2. Using the "TO FREE MINGW" above, direct to the directory where you want it.
3. Using normal file explorer now, download the `make.sh` located at the releases and place it into that directory.

3b. If you put in the file while the directory was open, do `cd .`.

4. Run it using `sh make.sh`.
5. Say yes to everything, and once its finally done you should have ability to use PTR2tools (and ISOMod).

5b. **The make acts as an updater. Run it periodically to get the latest updates. Anything other than PTR2Tools will be deleted if it is placed in that folder.**

**You must continue to use MINGW. The commands must be stored there. If they are not, they have the very possibility to break.**
## PTR2INT
### Commands
### list [intfile]
   Print list of contents of an INT file.
   **ALIASES: l, ls**
### extract [intfile] [folder]
   Extract contents of an INT file to a folder.
   **ALIASES: e, ex, x**
### create [intfile] [folder]
   Create an INT file using the contents of a folder.
   **ALIASES: c, i, make, pack**
### optimize [folder] [out-neworderfile]
   **I do not recommend using this.**

   Attempts to optimize the \_order.txt file of a folder to achieve lzss compression.
   **ALIAS: optimise**
### Command arguments:
intfile: The INT file. It's best to have this in your cd directory.

folder: The folder the INT contents will be placed in.

folder (optimize): The folder of which an \_order.txt is located.

out-neworderfile: The new order file to be made.

Example: ptr2int e TITLE.INT title
## PTR2SPM
### Commands
### gtex0 [spmfile] [outfile]
   Extracts tex0 from an SPM model.
### Command arguments:
spmfile: The spm file.

outfile: The output file in .txt.

Example: ptr2spm gtex0 ant_ant.spm meme.txt
## PTR2TEX
### Commands
### extract [tm0-folder] [in-listfile] <png-folder>
   Batch extract textures from tm0-folder using listfile and output as RGBA PNG.
     If png-folder is not specified, the current directory is used for output.
   **ALIASES: e, x, get, g**
### inject [tm0-folder] [in-listfile] <png-folder>
   Batch inject textures into tm0-folder using listfile with 32-bit RGBA PNGs.
     If png-folder is not specified, the current directory is used as input.
   **ALIASES: i, put, p**
### Commands defined:

extract: Extracts a list of TM0 textures.

inject: Puts in a list of TM0 textures.
### Command arguments:
tm0-folder: The folder that all the TM0s are in. It is named "TEXTURES" when the INT is extracted.

in-listfile: The .txt file with the tex0 IDs. See PTR2SPM: outfile.

png-folder: The folder with the PNGs to be injected in.

Example: ptr2tex extract TEXTURES meme.txt maymay
##OLD PTR2TEX (PTR2TEX2)
##Information
This was included in here just in case the common "Segmentation fault" error appeared with the fixed PTR2Tex.

If you know which tex0 give that error, you can use the `extract` and `inject` command separately.

Here it is defined by PTR2Tools.
```
---Commands---
extract [tm0-folder] [tex0] [output-filename]
inject [tm0-folder] [tex0] [input-filename]
extract-list [tm0-folder] [list-filename] <png-folder>
inject-list [tm0-folder] [list-filename] <png-folder>
```

# Old README
## Introduction

PTR2TOOLS is a collection of open source software for interoperability with PaRappa the Rapper 2. You may freely use, modify, and distribute the source code. Pull requests are welcome.

At the time of writing, the following functionality is available:
- INT Packer/Unpacker
- TM0 Encoder/Decoder
- SPM TEX0 Extraction

## Dependencies

The following libraries allow for a complete build of ptr2tools, whose usage may restrict licensing.
- libpng (PTR2TEX, [ http://www.libpng.org/pub/png/libpng.html ])
