# Device-Tree-Runtime-Configuration

This repository contains code for a device tree runtime parser. The primary API that is used to parse the tree can be found in /fdt_lib/fdt_lib_tools.h

Structure of the code:
- dtb_files: Contains device tree binaries
- dts_files: Contains device tree source files
- fdt_lib: Contains source code relating to the parsing of the device tree \n

# TODO:
- Redo function that returns the memory reservation block info (right now the information is just printed, not stored)
- Error check for malloc failures
- Reconsider placement of offset alignment functions
- Figure out a better way of error propogation through the embedded functions
- Backwards compatibility with older versions of device tree
- Better printing of property values
