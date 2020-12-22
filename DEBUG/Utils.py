import sys
import argparse
import math
import ctypes
so_file = "./functions.so"
reg     = ctypes.CDLL(so_file)


def ReadProperty(device, location, offset, width, verbose=False):
    read_32bits   = ctypes.c_uint(0);
    isRead        = reg.read_bits(device, location, ctypes.byref(read_32bits), 0)
    if isRead == -1:
        print("An error occured while executing C function read_bits")
        return -1

    string_32bits = "{:032b}".format(read_32bits.value)
    if verbose:
        print("Read:  %s"%(string_32bits))
    string_32bits = "{:032b}".format(read_32bits.value>>offset)
    string_out = string_32bits[-1*(width):] # remove masked bits                                                                                                                   
    print(string_out)
    output     = int(string_out, 2)                          # convert selected string to integer                                                                                  
    if verbose:
        print("Unmasked result: %i"%(output))
    return output
    
 def WriteProperty(device, location, offset, width, write_value, verbose=False):
    read_value    = ctypes.c_uint(0);
    isRead        = reg.read_bits(device, location, ctypes.byref(read_value), 0)
    if isRead == -1:
        print("An error occured while executing C function read_bits")
        return -1
    string_read   = "{:032b}".format(read_value.value)
    string_write  = "{:032b}".format(write_value.value<<offset)
    if verbose:
        print("Read:  %s"%(string_read))
        print("Write: %s"%(string_write))

    string_write = string_read[:-1*(offset+width)] + string_write[-1*(offset+width):-1*offset] + string_read[-1*offset:]
    output = int(string_write, 2)
    isWritten = reg.write_bits(device, location, output, 0)
    if isWritten == -1:
        print("An error occured while executing C function write_bits.")
        return -1
    # Readback as checkout                                                                                                                                                         
    readcheck = ctypes.c_uint(reg.read_bits(device, location, ctypes.byref(read_value), 0)).value
    if readcheck == -1:
        print("An error occured while executing C function read_bits")
        return -1
    if read_value == output:
        return 0
    else:
        print("Readback failed. Register not writable.")
        print("Check the reading permission for location %s on board %s"%(location, device))
        return -1
