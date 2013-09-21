#ifndef _PROG_H_
#define _PROG_H_

#define TTYS "/dev/ttyUSB0"


#define CHECK_FD(fd) ({ if (fd<0) { \
    printf("Wrong descriptor: " #fd "\n"); \
    return -1; \
    }\
    })

typedef enum {
  CHAN_DTR,
  CHAN_RTS,
} CHAN_TYPE;

typedef struct {
  int         opt_onlyunlock;


  CHAN_TYPE   reset_chan;
  int         reset_time;
  int         reset_invert;

  char *name_tty;
  int   fdtty;

  char *name_ihex;
  ihex_recordset_t* ihfd;

} prog;

/** Hardware reset device.
 *
 * Reset device.
 * Used options from prog:
 * * reset_chan  -- CHAN_TYPE    (default CHAN_DTR)
 * * reset_ttyfd -- must be OPEN (default close)
 * * reset_time  -- time in us   (default 10000 us)
 */
int device_hardreset(prog *self);

#endif /* _PROG_H_ */
