#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <termios.h>
#include <unistd.h>

#include "cintelhex.h"

#define TTYS "/dev/ttyUSB0"

#define DEBUG(...) ({ printf("PROG: "); printf(__VA_ARGS__); })

/*
int inbuffer(int fd) {
  int xx;
  int rsize;
  char tmpbuff[100];

  xx=10;
  while (xx-->0){
    rsize=read(fd,tmpbuff,sizeof(tmpbuff)-1);
    if (rsize) {
      tmpbuff[rsize]='\0';
      printf("RETURNED: '%s'",tmpbuff);
    }
    usleep(10);
  }

  return 0;
}*/

static int s_programming(int fd, ihex_recordset_t* ihfd) {
  int i;
  char ch;
  char *data=NULL;

  ch='W';
  write(fd,&ch,1);
  sleep(1);

  uint32_t fsize=ihex_rs_get_size(ihfd);
  DEBUG("Send file size: %i\n", fsize);

  write(fd,(char*)(&fsize)+0,1);
  usleep(10000);
  write(fd,(char*)(&fsize)+1,1);
  usleep(10000);
  write(fd,(char*)(&fsize)+2,1);
  usleep(10000);
  write(fd,(char*)(&fsize)+3,1);
  usleep(10000);
  
  data=malloc(fsize);

  int res = ihex_mem_copy(ihfd, data, fsize, IHEX_WIDTH_8BIT,IHEX_ORDER_LITTLEENDIAN );
  if (res != 0) {
    //FIXME
    printf("ERR copy memory\n");
    return 1;
  }
    
  usleep(100000);

  for (i=0;i<fsize;i++) {
    if ((i%128)==0) {
      printf("Page %i\n",i/128);
      usleep(100000);
    }

    //printf("CH: %x\n",(unsigned char)&data[i]);
    //return 1;
    int xx=write(fd,&data[i],1);
    //printf("HEX: %x\n",(unsigned char)data[i]);
    if (xx<0) {
      perror("");
    }
    usleep(1000);
  }
  return 0;
}

int setterminal(int fd) {
  //Term in NON block
  struct termios newfd;
  tcgetattr(fd,&newfd);

  cfmakeraw(&newfd);
  //c_cc[VMIN]) and TIME (c_cc[VTIME]
  newfd.c_cc[VMIN]=0;
  newfd.c_cc[VTIME]=1;
  tcsetattr(fd,TCSANOW,&newfd);

  return 0;
}

static void s_ihex_printerr(ihex_error_t err) {

  printf("IHEX parcer error: ");
  switch( (int) err) {
    case IHEX_ERR_INCORRECT_CHECKSUM  : puts("Incorrect checksum"); break;
    case IHEX_ERR_NO_EOF              : puts("No EOF"); break; 
    case IHEX_ERR_PARSE_ERROR         : puts("Parse error"); break;
    case IHEX_ERR_WRONG_RECORD_LENGTH : puts("Wrong record length"); break;
    case IHEX_ERR_NO_INPUT            : puts("No input"); break;
    case IHEX_ERR_UNKNOWN_RECORD_TYPE : puts("Unknown record type"); break;
    case IHEX_ERR_PREMATURE_EOF       : puts("Premature EOF"); break;
    case IHEX_ERR_ADDRESS_OUT_OF_RANGE: puts("Addres out of range"); break;
    case IHEX_ERR_MMAP_FAILED         : puts("MMAP failed"); break;
    default:
      printf("No info about error number %i\n", (int) err);
  }
}

int main(int argc, char *argv[]) {
  int fd;
  int i;
  ihex_recordset_t* ihfd;
  char *filename=NULL;

  printf("PC file writeter\n");

  if (argc<=1) {
    printf("Usage %s <hex file>\n",argv[0]);
    return 1;
  }
  filename=argv[1];

  printf("Open ihex file: %s\n", filename);


  fd = open("/dev/ttyUSB0",O_RDWR);
  if (fd < 0) {
    printf("Can't ot open USB0 device\n");
    return 1;
  }

  ihfd = ihex_rs_from_file(filename);
  if (fd==NULL) {
    ihex_error_t err = ihex_errno();
    s_ihex_printerr(err);
    return 1;
  }


  setterminal(fd);

  i=300;
  while (i--) {
    char ch='A';
    write(fd, &ch, 1);
    usleep(10000);
  }

  s_programming(fd,ihfd);

  return 0;
}
