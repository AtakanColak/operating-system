/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#include "GIC.h"

GICC_t* GICC0 = ( GICC_t* )( 0x1E000000 );
GICD_t* GICD0 = ( GICD_t* )( 0x1E001000 );
GICC_t* GICC1 = ( GICC_t* )( 0x1E010000 );
GICD_t* GICD1 = ( GICD_t* )( 0x1E011000 );
GICC_t* GICC2 = ( GICC_t* )( 0x1E020000 );
GICD_t* GICD2 = ( GICD_t* )( 0x1E021000 );
GICC_t* GICC3 = ( GICC_t* )( 0x1E030000 );
GICD_t* GICD3 = ( GICD_t* )( 0x1E031000 );
