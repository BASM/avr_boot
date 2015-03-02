#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <getopt.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#include <termios.h>
#include <unistd.h>

#include "cintelhex.h"

#include "prog.h"

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

#define PERUD 20000

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
  usleep(PERUD);
  write(fd,(char*)(&fsize)+1,1);
  usleep(PERUD);
  write(fd,(char*)(&fsize)+2,1);
  usleep(PERUD);
  write(fd,(char*)(&fsize)+3,1);
  usleep(PERUD);
  
  data=malloc(fsize);

  int res = ihex_mem_copy(ihfd, data, fsize, IHEX_WIDTH_8BIT,IHEX_ORDER_LITTLEENDIAN );
  if (res != 0) {
    //FIXME
    printf("ERR copy memory\n");
    return 1;
  }
    
  usleep(PERUD*10);

  for (i=0;i<fsize;i++) {
    if ((i%128)==0) {
      printf("Page %i\n",i/128);
      usleep(PERUD*10);
    }

    //printf("CH: %x\n",(unsigned char)&data[i]);
    //return 1;
    int xx=write(fd,&data[i],1);
    //printf("HEX: %x\n",(unsigned char)data[i]);
    if (xx<0) {
      perror("");
    }
    usleep(PERUD/10);
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
static int tty_CTRL(int fdtty, int sercmd, int stat);
static int tty_DTR(int fdtty, int stat);
static int tty_RTS(int fdtty, int stat);

/* Convert CHAN_TYPE to TIOCM */
static int chantotiocm(CHAN_TYPE type) {
  switch(type) {
    case CHAN_RTS: return TIOCM_RTS;
    case CHAN_DTR: return TIOCM_DTR;
  }
  return 0;
}

int device_hardreset(prog *self) {
  int st;
  int type;

  CHECK_FD(self->fdtty);

  if (self->reset_invert==0) st=1;
  else                       st=0;

  type=chantotiocm(self->reset_chan);

  tty_CTRL(self->fdtty,type, st);
  usleep(self->reset_time);
  tty_CTRL(self->fdtty,type,!st);
}

int device_chanopen(prog *self) {
  int type;

  type=chantotiocm(self->reset_chan);
  tty_CTRL(self->fdtty,type,self->reset_invert);
  return 0;
}

static int writeflash_ihex(const char *ttyname, const char *ihexfile) {

}

static int tty_CTRL(int fdtty, int sercmd, int stat) {
  if (stat==0) return ioctl(fdtty, TIOCMBIC, &sercmd);
  else         return ioctl(fdtty, TIOCMBIS, &sercmd);
  return -1;
}

static int tty_DTR(int fdtty, int stat) {
  return tty_CTRL(fdtty,TIOCM_DTR,stat);
}

static int tty_RTS(int fdtty, int stat) {
  return tty_CTRL(fdtty,TIOCM_RTS,stat);
}

static int tty_config(int fdtty, int bound) {

  return 0;
}

int prog_init(prog *self) {
  memset(self,0,sizeof(prog));
  self->reset_chan= CHAN_DTR;
  self->reset_time= 10000;

  self->reset_invert=0;

  self->fdtty =-1;
  self->name_tty="/dev/ttyUSB0";
  return 0;
}


static tty_open(prog *self) {

  self->fdtty = open(self->name_tty,O_RDWR);
  if (self->fdtty < 0) {
    printf("%s",self->name_tty);
    perror("");
    return 1;
  }
  return 0;
}

static usage(char *name) {
  printf("%s [Options] <filename.ihex>\n",name);
  puts("Options:");
  puts(" -h --   -- help message");
  puts(" -u --   -- only unlock MK (release Reset pin)");
  puts(" -r  -- set reset pin RTS or DTR (default DTR)");
  puts(" --port -p");
  puts("      set tty device name (default /dev/ttyUSB0)");
  puts(" --invert -i");
  puts("      invert reset pin set 0--assert,1--clear");
}

int main(int argc, char *argv[]) {
  int i;
  int c;
  char *filename=NULL;
  int   option_index=0;

  prog  self_obj;
  prog *self=&self_obj;
  prog_init(self);

  static struct option long_options[] =
  {
    {"help"     , no_argument      , NULL, 'h'},
    {"unlock"   , no_argument      , NULL, 'u'},
    {"invert"   , no_argument      , NULL, 'i'},
    {"port"     , required_argument, NULL, 'p'},
    {"reset"    , required_argument, NULL, 'r'},
    {0, 0, 0, 0}
  };
  
  printf("PC file writeter\n");


  while(1) {
    c=getopt_long(argc, argv, "hup:r:i", long_options, &option_index);
    if (c==-1) break;

    switch (c) {
      case 'h': usage(argv[0]); return 0;  break;
      case 'u': self->opt_onlyunlock=1;    break;
      case 'p': self->name_tty=optarg;     break;
      case 'r': {
                  if (!strcmp("DTR",optarg))
                    self->reset_chan=CHAN_DTR;
                  else if (!strcmp("RTS",optarg))
                    self->reset_chan=CHAN_RTS;
                  else {
                    printf("Reset channel DTR or RTS supported\n");
                    usage(argv[0]);
                    return 1;
                  }
                }
                break;
      case 'i': self->reset_invert=1;     break;
      default:
                usage(argv[0]);
                printf("======\n");
                printf("??? ARG: %i (%s)\n",optind,argv[optind]);
                return 1;
    }
  }
  printf("Config:\n");
  printf("* TTY device: %s\n", self->name_tty);
  printf("* Reset chan: %s\n", (self->reset_chan==CHAN_DTR)?"DTR":"RTS");
  printf("* Reset invert: %s\n", (self->reset_invert)?"yes":"no");

  if (tty_open(self)) return 1;

  if (self->opt_onlyunlock==1) {
    device_chanopen(self);
    return 0;
  }

  if ( (argc-1)==optind) {
    filename=argv[optind];
  } else { 
    usage(argv[0]);
    return 1;
  }

  self->ihfd = ihex_rs_from_file(filename);
  if (self->ihfd==NULL) {
    ihex_error_t err = ihex_errno();
    s_ihex_printerr(err);
    return 1;
  }

  setterminal(self->fdtty);

  device_hardreset(self);

  i=10;
  while (i--) {
    char ch='A';
    write(self->fdtty, &ch, 1);
    usleep(10000);
  }
  sleep(1);

  s_programming(self->fdtty,self->ihfd);
 //*/
  return 0;
}
