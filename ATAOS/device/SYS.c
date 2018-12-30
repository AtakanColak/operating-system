/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#include "SYS.h"

SYSCONF_t* SYSCONF  = ( SYSCONF_t* )( 0x10000000 );

 uint32_t* SYSCTRL0 = (  uint32_t* )( 0x10001000 );
 uint32_t* SYSCTRL1 = (  uint32_t* )( 0x1001A000 );
