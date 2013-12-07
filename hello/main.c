/** 
 * Simple HELLO WORD for boot loader testing.
 */

#include <stdio.h>
#include <uart.h>
#include <inttypes.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
//////// UART /////////////////////

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
  stdioconf_stdio();
  wdt_enable(WDTO_2S);

  int i=0;
  while (1) {
    printf("Hello world in APPLICATION mode ;-) (%i)\n",i++);

    _delay_ms(1000);
  }



  return 0;
}

