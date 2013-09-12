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

int inbuffer(int fd) {
  int rsize;
  char tmpbuff[100];

  rsize=read(fd,tmpbuff,sizeof(tmpbuff)-1);
  if (rsize) {
    tmpbuff[rsize]='\0';
    printf("RETURNED: '%s'",tmpbuff);
  }

  return 0;
}

static s_programming(int fd, ihex_error_t ihfd) {
  char ch;

  ch='W';
  write(fd,&ch,1);
  sleep(1);

  inbuffer(fd);
  
  uint32_t fsize=ihex_rs_get_size(ihfd);
  DEBUG("Send file size: %i\n", fsize);

  write(fd,&fsize,4);
  sleep(1);
  inbuffer(fd);

}

int setterminal(int fd) {
  //Term in NON block
  struct termios newfd;
  tcgetattr(fd,&newfd);
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

  s_programming(fd,ihfd);

  return 0;
}
