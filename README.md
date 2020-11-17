# SCOUTING
40MHz scouting CMS

## Control:

- main.py : python interface to get/set (r/rw) properties

            Arguments:
            
            -- board (-b):    choose ["board0", "board1", ...]
            -- property (-p): choose ["property0", "property1", ...]
            -- action (-a):   choose ["set", "get"]
            -- value (-v):    optional new state for property
            
- functions.c : C functions to read/write in the board using AXI-lite from Xilinx (https://github.com/Xilinx/dma_ip_drivers/tree/master/XDMA/linux-kernel)
            
            before running build the shared library by doing:
            
            cc -fPIC -shared -o functions.so functions.c
