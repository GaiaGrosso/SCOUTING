#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <byteswap.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/mman.h>

/* ltoh: little to host */
/* htol: little to host */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ltohl(x)       (x)
#define ltohs(x)       (x)
#define htoll(x)       (x)
#define htols(x)       (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define ltohl(x)     __bswap_32(x)
#define ltohs(x)     __bswap_16(x)
#define htoll(x)     __bswap_32(x)
#define htols(x)     __bswap_16(x)
#endif

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

#define MAP_SIZE (32*1024UL)
#define MAP_MASK (MAP_SIZE - 1)

// New functions declaration:
void write_bits(char *device_loc, char *address, char *width, unsigned int writeval);
unsigned int read_bits(char *device_loc, char *address, char *width);
void printBits(unsigned int num);
unsigned int modifyBit(unsigned int x, unsigned char position, bool newState);


void write_bits(char *device_loc, char *address, char *width, unsigned int writeval)
{
  int fd;
  void *map_base, *virt_addr;
  off_t target;
  int access_width;
  access_width = tolower(width[0]);
  char *device;
  device = strdup(device_loc);
  printf("device: %s\n", device);
  target = strtoul(address, 0, 0);
  printf("address: 0x%08x\n", (unsigned int)target);
  printf("access width: ");
  if (access_width == 'b')
    printf("byte (8-bits)\n");
  else if (access_width == 'h')
    printf("half word (16-bits)\n");
  else if (access_width == 'w')
    printf("word (32-bits)\n");
  else {
    printf("word (32-bits)\n");
    access_width = 'w';
  }
  if ((fd = open(device_loc, O_RDWR | O_SYNC)) == -1)
    FATAL;
  printf("character device %s opened.\n", device_loc);
  fflush(stdout);
  /* map one page */
  map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map_base == (void *)-1)
    FATAL;
  printf("Memory mapped at address %p.\n", map_base);
  fflush(stdout);
  /* calculate the virtual address to be accessed */
  virt_addr = map_base + target;
  /* Write */
  switch (access_width) {
  case 'b':
    printf("Write 8-bits value 0x%02x to 0x%08x (0x%p)\n",
	   (unsigned int)writeval, (unsigned int)target,
	   virt_addr);
    *((uint8_t *) virt_addr) = writeval;
#if 0
    read_result = *((uint8_t *) virt_addr);
    printf("Written 0x%02x; readback 0x%02x\n", writeval, read_result);
#endif
    break;
  case 'h':
    printf("Write 16-bits value 0x%04x to 0x%08x (0x%p)\n",
	   (unsigned int)writeval, (unsigned int)target,
	   virt_addr);
    /* swap 16-bit endianess if host is not little-endian */
    writeval = htols(writeval);
    *((uint16_t *) virt_addr) = writeval;
#if 0
    read_result = *((uint16_t *) virt_addr);
    printf("Written 0x%04x; readback 0x%04x\n", writeval, read_result);
#endif
    break;
  case 'w':
    printf("Write 32-bits value 0x%08x to 0x%08x (0x%p)\n",
	   (unsigned int)writeval, (unsigned int)target,
	   virt_addr);
    /* swap 32-bit endianess if host is not little-endian */
    writeval = htoll(writeval);
    *((uint32_t *) virt_addr) = writeval;
#if 0
    read_result = *((uint32_t *) virt_addr);
    printf("Written 0x%08x; readback 0x%08x\n", writeval, read_result);
#endif
    break;
  }
  fflush(stdout);
};

unsigned int read_bits(char *device_loc, char *address, char *width)
{
  uint32_t read_result;
  int fd;
  void *map_base, *virt_addr;
  off_t target;
  int access_width;
  access_width = tolower(width[0]);
  char *device;
  device = strdup(device_loc);
  printf("device: %s\n", device);
  target = strtoul(address, 0, 0);
  printf("address: 0x%08x\n", (unsigned int)target);

  printf("access width: ");
  if (access_width == 'b')
    printf("byte (8-bits)\n");
  else if (access_width == 'h')
    printf("half word (16-bits)\n");
  else if (access_width == 'w')
    printf("word (32-bits)\n");
  else {
    printf("word (32-bits)\n");
    access_width = 'w';
  }
  if ((fd = open(device_loc, O_RDWR | O_SYNC)) == -1)
    FATAL;
  printf("character device %s opened.\n", device_loc);
  fflush(stdout);

  /* map one page */
  map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map_base == (void *)-1)
    FATAL;
  printf("Memory mapped at address %p.\n", map_base);
  fflush(stdout);
  /* calculate the virtual address to be accessed */
  virt_addr = map_base + target;
  /* Read */
  switch (access_width) {
  case 'b':
    read_result = *((uint8_t *) virt_addr);
    printf
      ("Read 8-bits value at address 0x%08x (%p): 0x%02x\n",
       (unsigned int)target, virt_addr,
       (unsigned int)read_result);
    break;
  case 'h':
    read_result = *((uint16_t *) virt_addr);
    /* swap 16-bit endianess if host is not little-endian */
    read_result = ltohs(read_result);
    printf
      ("Read 16-bit value at address 0x%08x (%p): 0x%04x\n",
       (unsigned int)target, virt_addr,
       (unsigned int)read_result);
    break;
  case 'w':
    read_result = *((uint32_t *) virt_addr);
    /* swap 32-bit endianess if host is not little-endian */
    read_result = ltohl(read_result);
    printf
      ("Read 32-bit value at address 0x%08x (%p): 0x%08x\n",
       (unsigned int)target, virt_addr,
       (unsigned int)read_result);
    
    break;
  default:
    fprintf(stderr, "Illegal data type '%c'.\n",
	    access_width);
    exit(2);
  }
  fflush(stdout);
  return (unsigned int)read_result;
};


void printBits(unsigned int num){
  unsigned int size = sizeof(unsigned int);
  unsigned int maxPow = 1<<(size*8-1);
  int i=0;
  for(; i<size*8; ++i){
    // print last bit and shift left.
    printf("%u ",num&maxPow ? 1 : 0);
    num = num<<1;
  }
  fflush(stdout);
};

unsigned int modifyBit(unsigned int x, unsigned char position, bool newState)
{
  printf("Position: ");
  printf("%02x", position);
  printf("\n");
  printf("New state: ");
  if(newState)
    printf("yes \n");
  else
    printf("no \n");
  fflush(stdout);
  int mask = 1 << position;
  int state = newState; // relies on true = 1 and false = 0                                                                                                       
  return (x & ~mask) | (-state & mask);
};

int main(int argc, char **argv)
{
  /* not enough arguments given? */
  if (argc < 3) {
    fprintf(stderr,
            "\nUsage:\t%s <device> <address> [[type] data]\n"
            "\tdevice  : character device to access\n"
            "\taddress : memory address to access\n"
            "\ttype    : access operation type : [b]yte, [h]alfword, [w]ord\n",
            argv[0]);
    exit(1);
  }

  char *device  = argv[1];
  char *address = argv[2];
  char *width   = argv[3];
  unsigned int read_result;
  read_result = read_bits(device, address, width=width);
  printf("read_bits function executed.\n");
  fflush(stdout);
  printf("readval: ");
  printBits(read_result);
  printf("\n");
  fflush(stdout);
  unsigned char position = 4;
  unsigned int writeval = modifyBit(read_result, position, true); // add non trivial bits modification here                                                       
  printf("writeval: ");                                                                                                                                                 
  printBits(writeval);
  printf("\n");
  write_bits(device, address, width, writeval);
  return 0;
};
