/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of
 * which can be found via http://creativecommons.org (and should be included as
 * LICENSE.txt within the associated archive or repository).
 */

#ifndef __HILEVEL_H
#define __HILEVEL_H

// Include functionality relating to newlib (the standard C library).

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Include functionality relating to the   kernel.

#include "lolevel.h"
#include     "int.h"

#include "PL011.h"
#include "SP804.h"
#include   "GIC.h"

typedef int pid_t;

typedef enum {
  STATUS_CREATED,
  STATUS_READY,
  STATUS_EXECUTING,
  STATUS_WAITING,
  STATUS_TERMINATED
} status_t;

typedef enum {
  CHANNEL_ACTIVE,
  CHANNEL_INACTIVE
} activity_t;

typedef struct {
  activity_t activity;
  uint32_t left, right, datasize;
  void * data;
} channel_t;

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

typedef struct {
  int wait, queue, priority;
} priority_t;

typedef struct {
     uint32_t tos;
     priority_t priority;
     pid_t    pid;
  status_t status;
     ctx_t    ctx;
} pcb_t;

#endif
