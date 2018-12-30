/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#include "PL050.h"

PL050_t* PS20 = ( PL050_t* )( 0x10006000 );
PL050_t* PS21 = ( PL050_t* )( 0x10007000 );

void    PL050_putc( PL050_t* d, uint8_t x ) {
  // wait while transmit register isn't empty 
  while( !( d->STAT & 0x40 ) );
  //            transmit x
  d->DATA = x;
}

uint8_t PL050_getc( PL050_t* d            ) {
  // wait while receive  register isn't  full
  while( !( d->STAT & 0x10 ) );
  //            recieve  r
  return d->DATA;
}
