# SCOUTING
40MHz scouting CMS

## Control:

- main.py : python interface to get/set (r/rw) properties

            Arguments:
            
            -- board (-b):    choose ["board0", "board1", ...]
            -- property (-p): choose ["property0", "property1", ...]
            -- action (-a):   choose ["set", "get"]
            -- value (-v):    optional new state for property when action == "set"
            
- functions.c : C functions to read/write in the board using AXI-lite from Xilinx (https://github.com/Xilinx/dma_ip_drivers/tree/master/XDMA/linux-kernel)
            
### set up:
 - download https://github.com/Xilinx/dma_ip_drivers/
 - add functions.c:
            
            cd $HOME/dma_ip_drivers-master/XDMA/linux-kernel/
            git clone https://github.com/GaiaGrosso/SCOUTING/ 
            
 - if the driver is not installed follow the instruction to install the Xlinx driver
            
 - build the shared library:
           
            cd $HOME/dma_ip_drivers-master/XDMA/linux-kernel/SCOUTING
            cc -fPIC -shared -o functions.so functions.c

- run main.py

            main.py -b [board_name] -a [set/get] -p [property_name] (-v [new_value])

## To Do:
- Configuration file (json): convert the dictionary in main.py into a json file
