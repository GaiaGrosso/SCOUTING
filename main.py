import sys
import argparse
import ctypes
so_file = "./functions.so"
reg     = ctypes.CDLL(so_file)

config_dict = {
    "board0" : {
        "driver"   : "/dev/wz-xdma0_user",
        "property" : {
            "property0" : {
                "location": "0x00000000",
                "bitmask" : "00000000000000000000000000001111"
            },

            "property1" : {
                "location": "0x00000000",
                "bitmask" : "00000000111000000000000000000000"
            }
        }
    },
    "board1" : {
        "driver"   : "/dev/wz-xdma1_user",
        "property" : {
            "property0" : {
                "location": "0x00000000",
                "bitmask" : "00000000000000000000000000001111"
            },

            "property1" : {
                "location": "0x00000001",
                "bitmask" : "00000000000000000000000000001110"
            }
        }
    }
    #"board2" : "/dev/wz-xdma2_user",                                                                                                                        
    #...                                                                                                                                                     
}

actions = ["get", "set"]

def ReadProperty(device, location, mask_32bits):
    read_32bits   = ctypes.c_uint(reg.read_bits(device, location, width="w"))
    string_32bits = "{:032b}".format(read_32bits.value)
    print("Read:  %s"%(string_32bits))
    list_32bits   = [int(bit) for bit in string_32bits]
    list_32mask   = [int(bit) for bit in mask_32bits]
    selected_bits = []
    for i in range(32):
        if list_32mask[i]:
            selected_bits = [list_32bits[i]] + selected_bits
    output        = 0
    for bit in selected_bits:
        output = (output<<1)|bit
    return output

def WriteProperty(device, location, string_mask, write_value):
    read_value    = ctypes.c_uint(reg.read_bits(device, location, width="w"))
    string_read   = "{:032b}".format(read_value.value)
    string_write  = "{:032b}".format(write_value.value)
    print("Read:  %s"%(string_read))
    print("Mask:  %s"%(string_mask))
    print("Write: %s"%(string_write))
    list_32read   = [int(bit) for bit in string_read]
    list_32mask   = [int(bit) for bit in string_mask]
    list_32write  = [int(bit) for bit in string_write]
    #selected_bits = np.where(list_32mask>0, )                                                                                                               
    list_32out    = []
    j=0
    for i in range(32):
        if not list_32mask[31-i]:
            list_32out = [list_32read[31-i]]+list_32out
        else:
            list_32out = [list_32write[31-j]]+list_32out
            j+=1
    output = 0
    for bit in list_32out:
        output = (output<<1)|bit
    output = ctypes.c_uint(output)
    reg.write_bits(device, location, "w", output);
    return


def NBitsAllowed(mask): #string of bits                                                                                                                      
    output=0
    for i in range(32):
        if int(mask[i])==1:
            output +=1
    print("Number of allowed bits: %i"%(output))
    return output

def NBitsNeeded(value): #string of bits                                                                                                                      
    output=32
    i=0
    while(int(value[i])<1):
        output -= 1
        i+=1
    print("Number of required bits: %i"%(output))
    return output

if __name__ == '__main__':

    parser = argparse.ArgumentParser()    #Python tool that makes it easy to create an user-friendly command-line interface                                 \
                                                                                                                                                             
    parser.add_argument('-b','--board',    type=str, help="scouting board",   required=True)
    parser.add_argument('-a','--action',   type=str, help="control action",   required=True, choices=actions)
    parser.add_argument('-p','--property', type=str, help="control property", required=True)
    parser.add_argument('-v','--value',    type=str, help="new value",        required=False)
    args = parser.parse_args()

    board      = args.board
    property   = args.property
    device     = config_dict[board]["driver"]
    location   = config_dict[board]["property"][property]["location"]
    bitmask    = config_dict[board]["property"][property]["bitmask"]
    action     = args.action
    value      = args.value
    if action == "set":
        if value == None:
            print("action 'set' requires the optional argument --value (-v).")
            exit()
        else:
            writevalue    = ctypes.c_uint(int(args.value)) #unsigned int (32 bits)                                                                           
            nbits_allowed = NBitsAllowed(bitmask)
            nbits_write   = NBitsNeeded("{:032b}".format(writevalue.value))
            if nbits_write > nbits_allowed:
                print("Overflow error! value %s doesn't fit in the allowed memory location."%(args.value))
                exit()
            else:
                WriteProperty(device, location, bitmask, writevalue)
                print('Set property %s from location %s on %s: %i'%(property, location, board, writevalue.value))

    elif action == "get":
        readvalue = ReadProperty(device, location, bitmask)
        print('Get property %s from location %s on %s: %i'%(property, location, board, readvalue))
