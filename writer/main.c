#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <termios.h>
#include <unistd.h>

#define TTYS "/dev/ttyUSB0"

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

int main(void) {

  printf("PC file writeter\n");

  int fd = open("/dev/ttyUSB0",O_RDWR);
  if (fd < 0) {
    printf("Can't ot open USB0 device\n");
    return 1;
  }

  setterminal(fd);
  while (1) {
    char ch='W';

    write(fd,&ch,1);

    inbuffer(fd);

    sleep(3);
  }


  return 0;
}
