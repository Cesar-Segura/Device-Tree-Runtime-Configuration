# Device-Tree-Runtime-Configuration

This repository contains the code for the device tree runtime parser.

Primary APIs:
- /fdt_lib/fdt_lib_header.h:
  - Interface for parsing the header of the device tree
- /fdt_lib/fdt_lib_mem_rev_block.h:
  - Interface for parsing the memory reservation block of the device tree
- /fdt_lib/fdt_lib_tools.h:
  - Interface for parsing the structure block of the device tree
- /fdt_lib/fdt_lib.h:
  - Low-level bit manipulation, pointer offset management, and general device tree info

Structure of the code (by directory name):
- dtb_files: Contains device tree binaries
- dts_files: Contains device tree source files
- fdt_lib: Contains source code relating to the parsing of the device tree

Command to run the parser test:
- Change directories to fdt_lib
- run ./build-run-parser.sh from the terminal

# TODO:
- The code needs to be refactored to include an iterator object
- This iterator object will hold an offset (and other attributes) in it
- iterator passed into each function
- better than static offset because we can have more than one thread of iteration
- general parsing API: fdt_iterator_next(), fdt_iterator_init()
- implement offset wrapped as own type and passed into function
- Other items:
	- Reconsider placement of offset alignment functions
	- Figure out a better way of error propogation through the embedded functions
	- Backwards compatibility with older versions of device tree
