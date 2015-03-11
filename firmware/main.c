/** 
 * Boot.
 *
 * Self program
 */

#include <stdint.h>
#include <stdio.h>
#include <uart.h>
#include <inttypes.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
//////// UART /////////////////////

uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
void get_mcusr(void) \
       __attribute__((naked)) \
       __attribute__((section(".init3")));

void get_mcusr(void)
{
  mcusr_mirror = MCUSR;
  MCUSR = 0;
  wdt_disable();
}
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

static int writeall() {
  uint8_t buff[SPM_PAGESIZE];

  uint32_t bytes=0;
  uint32_t bi=0;
  int pages;
  int p;

  bytes|=getbyte()<<0;
  bytes|=getbyte()<<8;
  bytes|=(uint32_t)getbyte()<<16;
  bytes|=(uint32_t)getbyte()<<24;

  pages=bytes/SPM_PAGESIZE;

  bi=bytes%SPM_PAGESIZE-1;
  for (p=0;p<=pages;p++) {
    int i;

    for (i=0;i<SPM_PAGESIZE;i++) {
      buff[i]=getbyte();
      if ((p==pages) &&
          (i>=bi)
         ) break;
    }
    boot_program_page (p*SPM_PAGESIZE, buff);
  }

  return 0;
}

int main(void) {
  int byte;
  int progmode=0;
  int itr=10;

  uart_init();
  //stdioconf_stdio();

  //_delay_ms(10);
    
  while (itr-->0) {
    _delay_ms(100);
    byte = getbyte_nonblock();
    if (byte=='A') {
      bchar_put('X');
      progmode=1;
      break;
    }
  }
  void (*funcptr)( void ) = 0x0000;
  
  if (progmode!=1) {
    bchar_put('A');
    funcptr();
  }

  int i=0;
  while (1) {
    char ch;
    i++;
    ch=getbyte();
    if (ch == 'W') {
      writeall();
      funcptr();
    }
    if (ch == 'R') {
      funcptr();
    }

    _delay_ms(100);
  }

  return 0;
}

