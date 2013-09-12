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

static void putint(int number) {
  char buff[5];
  int i,j;

  i=0;
  while (number!=0) {
    buff[i]=number%10;
    number/=10;
    i++;
  }
  while (i>0) putchar('0'+buff[i--]);
}

static writeall() {
  int pages;
  pages=getch();
  
  puts("Pages:");
  putint(pages);


  return 0;
}

int main(void) {

  uart_init();
  uart_stdio();

  char buff[SPM_PAGESIZE];

  puts("BOOT LOADER mode");
  //printf("We a in BOOT LOADER mode\n");
  //printf("%x\n", &main);


  int i=0;
  while (1) {
    char ch;
    i++;
    //ch=2;
    ch=getch();

    fputs("Pressed key__ ",stdout);
    putchar(ch);
    if (ch == 'W') {
      puts("xxIn WRITE MODE\n");
      //writeall();
    }
    puts("");//%i\n",ch);

//    memset(buff,ch,SPM_PAGESIZE);
//    boot_program_page(i*SPM_PAGESIZE,buff);

    //sleep(1);
    _delay_ms(100);
  }



  return 0;
}

