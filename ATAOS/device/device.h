/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#ifndef __DEVICE_H
#define __DEVICE_H

#define RSVD(x,y,z) uint8_t reserved_##x[ z - y + 1 ]

#define RO volatile const /* read  only */
#define WO volatile       /* write only */
#define RW volatile       /* read/write */
#define WR volatile       /* write/read */

#endif
