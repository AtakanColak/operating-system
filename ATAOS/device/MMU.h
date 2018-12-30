/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#ifndef __MMU_H
#define __MMU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "device.h"

//  enable MMU
void mmu_enable();
// disable MMU
void mmu_unable();

// flush   TLB
void mmu_flush();

// configure MMU: set page table pointer #0 to x
void mmu_set_ptr0( uint32_t* x );
// configure MMU: set page table pointer #1 to x
void mmu_set_ptr1( uint32_t* x );

// configure MMU: set 2-bit permission field of domain d to x
void mmu_set_dom( int d, uint8_t x );

#endif
