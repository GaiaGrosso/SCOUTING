import sys
import argparse
import math
import ctypes
import os
import Utils as u

if __name__ == '__main__':
    parser = argparse.ArgumentParser()    #Python tool that makes it easy to create an user-friendly command-line interface                                                        
    parser.add_argument('-f','--filelog',    type=str, help="logfile",   required=True)
    args = parser.parse_args()

    device          = "/dev/wz-xdma0_user"
    location_list   = ["0x"+"%08i"%(i) for i in range(4096)]
    logfile         = args.filelog
    os.system("chmod a+rw %s\n"%(logfile))
    f = open(logfile, "w")
    for location in location_list:

        iswritable = u.WriteProperty(device, location, 0, 32, ctypes.c_uint(100), verbose=False)
        read_value = u.ReadProperty(device, location, 0, 32, verbose=False)
        print("%s,%i,%s\n"%(location[2:], iswritable, bin(read_value)[2:]))
        f.write("%s,%i,%s\n"%(location[2:], iswritable, bin(read_value)[2:]))
    f.close()
