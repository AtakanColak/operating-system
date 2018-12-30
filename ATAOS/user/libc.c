/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of
 * which can be found via http://creativecommons.org (and should be included as
 * LICENSE.txt within the associated archive or repository).
 */

#include "libc.h"

extern int  atoi( char* x        ) {
  char* p = x; bool s = false; int r = 0;

  if     ( *p == '-' ) {
    s =  true; p++;
  }
  else if( *p == '+' ) {
    s = false; p++;
  }

  for( int i = 0; *p != '\x00'; i++, p++ ) {
    r = s ? ( r * 10 ) - ( *p - '0' ) :
            ( r * 10 ) + ( *p - '0' ) ;
  }

  return r;
}

extern void itoa( char* r, int x ) {
  char* p = r; int t, n;

  if( x < 0 ) {
     p++; t = -x; n = t;
  }
  else {
          t = +x; n = t;
  }

  do {
     p++;                    n /= 10;
  } while( n );

    *p-- = '\x00';

  do {
    *p-- = '0' + ( t % 10 ); t /= 10;
  } while( t );

  if( x < 0 ) {
    *p-- = '-';
  }

  return;
}

void yield() {
  asm volatile( "svc %0     \n" // make system call SYS_YIELD
              :
              : "I" (SYS_YIELD)
              : );

  return;
}

void wait() {
  asm volatile( "svc %0     \n" // make system call SYS_WAIT
              :
              : "I" (SYS_WAIT)
              : );

  return;
}

void sleep(int ticks) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  ticks
                "svc %0     \n" // make system call SYS_SLEEP
              :
              : "I" (SYS_SLEEP), "r" (ticks)
              : "r0" );

  return;
}

int write( int fd, const void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_WRITE), "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int  read( int fd,       void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_READ
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_READ),  "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int fork() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_FORK
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_FORK)
              : "r0" );

  return r;
}

void exit( int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "svc %0     \n" // make system call SYS_EXIT
              :
              : "I" (SYS_EXIT), "r" (x)
              : "r0" );

  return;
}

void exec( const void* x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 = x
                "svc %0     \n" // make system call SYS_EXEC
              :
              : "I" (SYS_EXEC), "r" (x)
              : "r0" );

  return;
}

int  kill( int pid, int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 =  pid
                "mov r1, %3 \n" // assign r1 =    x
                "svc %1     \n" // make system call SYS_KILL
                "mov %0, r0 \n" // assign r0 =    r
              : "=r" (r)
              : "I" (SYS_KILL), "r" (pid), "r" (x)
              : "r0", "r1" );

  return r;
}

void nice( int pid, int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  pid
                "mov r1, %2 \n" // assign r1 =    x
                "svc %0     \n" // make system call SYS_NICE
              :
              : "I" (SYS_NICE), "r" (pid), "r" (x)
              : "r0", "r1" );

  return;
}

// int  read( int fd,       void* x, size_t n ) {
//   int r;
//
//   asm volatile( "mov r0, %2 \n" // assign r0 = fd
//                 "mov r1, %3 \n" // assign r1 =  x
//                 "mov r2, %4 \n" // assign r2 =  n
//                 "svc %1     \n" // make system call SYS_READ
//                 "mov %0, r0 \n" // assign r  = r0
//               : "=r" (r)
//               : "I" (SYS_READ),  "r" (fd), "r" (x), "r" (n)
//               : "r0", "r1", "r2" );
//
//   return r;
// }


int channel_create( int left, int right, int size) {
  int index;
  asm volatile( "mov r0, %2 \n" // assign r0 =  left
                "mov r1, %3 \n" // assign r1 =  right
                "mov r2, %4 \n" // assign r2 = datasize
                "svc %1     \n" // make system call CHNL_CREATE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (index)
              : "I" (CHNL_CREATE), "r" (left), "r" (right), "r" (size)
              : "r0", "r1", "r2" );
  return index;
}

void channel_send( int index, void * data) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  index
                "mov r1, %2 \n" // assign r1 =  data
                "svc %0     \n" // make system call CHNL_SEND
              :
              : "I" (CHNL_SEND), "r" (index), "r" (data)
              : "r0", "r1");
}

void channel_receive(int index, int datasize, void * data) {
  uint32_t lower, higher;
  asm volatile( "mov r0, %1 \n" // assign r0 =  index
                "svc %2     \n" // make system call CHNL_RECEIVE
                "mov %0, r0 \n" // assign r  = r0
                "mov %1, r1 \n" // assign r  = r1
              : "=r" (lower), "=r" (higher)
              : "I" (CHNL_RECEIVE), "r" (index)
              : "r0");
  uint64_t address = (((uint64_t) higher) << 32) || ((uint64_t) lower);
  void * source = (void *) ((uintptr_t) address);
  memcpy(data, &source, datasize);
}

void channel_terminate( int index ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  index
                "svc %0     \n" // make system call CHNL_KIL
              :
              : "I" (CHNL_KILL), "r" (index)
              : "r0" );
  return;
}

int pid() {
  int r;
  asm volatile( "svc %1     \n" // make system call SYS_PID
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_PID)
              : "r0" );

  return r;
}

//
// void channel_send (void * data, int datasize, int channel_id) {
//
// }
