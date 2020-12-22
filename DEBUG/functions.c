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

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, __FILE__, errno, strerror(errno)); } while(0)

#define MAP_SIZE (32*1024UL)
#define MAP_MASK (MAP_SIZE - 1)

int write_bits(char *device_loc, char *address, unsigned int writeval, int verbose)
{
  char *device = strdup(device_loc);
  off_t target = strtoul(address, 0, 0);
  printf("device: %s\n", device);
  printf("address: 0x%08x\n", (unsigned int)target);

  /* open device */
  int fd = open(device_loc, O_RDWR | O_SYNC);
if (fd == -1) {
  FATAL;
  return -1;
 }
 if (verbose>0){
printf("character device %s opened.\n", device_loc);
fflush(stdout);
 }
/* map one page */
void *map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
if (map_base == (void *)-1) {
  FATAL;
  close(fd);
  return -1;
 }
 if(verbose>0){
printf("Memory mapped at address %p.\n", map_base);
fflush(stdout);
 }
/* calculate the virtual address to be accessed */
void *virt_addr = map_base + target;

/* Write */
 if (verbose>0){
printf("Write 32-bits value 0x%08x to 0x%08x (0x%p)\n",
       (unsigned int)writeval, (unsigned int)target,
       virt_addr);
 fflush(stdout);
 }
/* swap 32-bit endianess if host is not little-endian */
writeval = htoll(writeval);
*((uint32_t *) virt_addr) = writeval;
 close(fd);
return 0;
};

int read_bits(char *device_loc, char *address, uint32_t *read_result, int verbose)
{
  char *device = strdup(device_loc);
  off_t target = strtoul(address, 0, 0);
  if(verbose>0){
  printf("device: %s\n", device);
  printf("address: 0x%08x\n", (unsigned int)target);
  }
  /* open device */
  int fd = open(device_loc, O_RDWR | O_SYNC);
  if ( fd == -1){
    FATAL;
    return -1;
  }
  if(verbose>0){
  printf("character device %s opened.\n", device_loc);
  fflush(stdout);
  }
  /* map one page */
  void *map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map_base == (void *)-1){
    FATAL;
    close(fd);
    return -1;
  }
  if(verbose>0){
  printf("Memory mapped at address %p.\n", map_base);
  fflush(stdout);
  }
  /* calculate the virtual address to be accessed */
  void *virt_addr = map_base + target;

  /* Read */
  uint32_t read_result_tmp = *((uint32_t *) virt_addr);

/* swap 32-bit endianess if host is not little-endian */
  read_result_tmp = ltohl(read_result_tmp);
  if(verbose>0){
  printf
    ("Read 32-bit value at address 0x%08x (%p): 0x%08x\n",
     (unsigned int)target, virt_addr,
     (unsigned int)read_result_tmp);

  fflush(stdout);
  }
  *read_result = read_result_tmp;
  close(fd);
  return 0;
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
