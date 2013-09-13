/** 
 * Boot.
 *
 * Self program
 */

#include <stdio.h>
#include <uart.h>
#include <inttypes.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
//////// UART /////////////////////

BOOTLOADER_SECTION

///////////////////////////////////

static void boot_program_page (uint32_t page, uint8_t *buf)
{
  uint16_t i;

  // Disable interrupts.

  cli();
  eeprom_busy_wait ();

  boot_page_erase (page);
  boot_spm_busy_wait ();      // Wait until the memory is erased.

  for (i=0; i<SPM_PAGESIZE; i+=2)
  {
    // Set up little-endian word.

    uint16_t w = *buf++;
    w += (*buf++) << 8;

    boot_page_fill (page + i, w);
  }

  boot_page_write (page);     // Store buffer in flash page.
  boot_spm_busy_wait();       // Wait until the memory is written.

  // Reenable RWW-section again. We need this if we want to jump back
  // to the application after bootloading.

  boot_rww_enable ();

  // Re-enable interrupts (if they were ever enabled).

}

static void putint(unsigned int number) {
  unsigned char buff[5];
  int i;

  if (number==0) {
    putchar('0');
    return;
  }

  i=0;
  while (number>0) {
    buff[i]=number%10;
    if (buff[i]>10) buff[i]=0;
    number/=10;
    i++;
  }
  i--;
  while (i>=0) putchar('0'+(unsigned int)buff[i--]);
}

static void puthex(unsigned char number) {
  unsigned char buff[5];
  int i;
  
  if (number==0) {
    putchar('0');
    putchar('0');
    return;
  }

  i=0;
  while (number>0) {
    buff[i]=number%16;
    if (buff[i]>16) buff[i]=0;
    number/=16;
    i++;
  }
  i--;
  while (i>=0) {
    unsigned char ch=buff[i--];
    if (ch>=10) putchar('A'+ch-10);
    else       putchar('0'+ch);
  }
}
static int writeall() {
  char buff[SPM_PAGESIZE];

  puts("In write mode");
  uint32_t bytes=0;
  uint32_t bi=0;
  int pages;
  int x;
  int p;

  bytes|=getbyte()<<0;
  bytes|=getbyte()<<8;
  bytes|=(uint32_t)getbyte()<<16;
  bytes|=(uint32_t)getbyte()<<24;

  pages=bytes/SPM_PAGESIZE;

  fputs("Wait:",stdout);
  putint(bytes);
  puts("");

  fputs("Pages:",stdout);
  putint(pages);
  puts("");

  bi=bytes%SPM_PAGESIZE-1;
  for (p=0;p<=pages;p++) {
    int i;

    fputs("Write page:",stdout);
    putint(p);
    puts("");

    for (i=0;i<SPM_PAGESIZE;i++) {
      buff[i]=getbyte();
//#puthex(buff[i]);
      if ( 
          (p==pages) &&
          (i>=bi)
         ) break;
    }
    boot_program_page (p*SPM_PAGESIZE, buff);
  }



  return 0;
}

int main(void) {

  uart_init();
  uart_stdio();

  void (*funcptr)( void ) = 0x0000;

  puts("BOOT LOADER mode");
  //printf("We a in BOOT LOADER mode\n");
  //printf("%x\n", &main);

  int i=0;
  while (1) {
    char ch;
    i++;
    //ch=2;
    ch=getch();
    //fputs("Pressed key__ ",stdout);
    putchar(ch);
    if (ch == 'W') {
      puts("xxIn WRITE MODE\n");
      writeall();
      puts("Try to run application...\n");
      funcptr();
    }
    if (ch == 'R') {
      funcptr();
    }
    puts("");


    //sleep(1);
    _delay_ms(100);
  }



  return 0;
}

