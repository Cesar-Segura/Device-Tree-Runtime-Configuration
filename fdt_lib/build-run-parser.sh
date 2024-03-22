#!/bin/bash

echo "Making fdt_lib_test" 
echo "" 
make fdt_lib_test
echo ""
echo ""

echo ""
echo "Running parser test"
echo ""

./fdt_lib_test ../dtb_files/virt_aarch64.dtb

echo ""
echo "Done running parser test" 
echo ""

make clean 