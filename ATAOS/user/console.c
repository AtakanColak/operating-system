/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of
 * which can be found via http://creativecommons.org (and should be included as
 * LICENSE.txt within the associated archive or repository).
 */

#include "console.h"

/* The following functions are special-case versions of a) writing, and
 * b) reading a string from the UART (the latter case returning once a
 * carriage return character has been read, or a limit is reached).
 */

extern void puts( char* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    PL011_putc( UART1, x[ i ], true );
  }
}

void gets( char* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    x[ i ] = PL011_getc( UART1, true );

    if( x[ i ] == '\x0A' ) {
      x[ i ] = '\x00'; break;
    }
  }
}

/* Since we lack a *real* loader (as a result of also lacking a storage
 * medium to store program images), the following function approximates
 * one: given a program name from the set of programs statically linked
 * into the kernel image, it returns a pointer to the entry point.
 */

extern void main_P();
extern void main_PH();
extern void main_P3();
extern void main_P4();
extern void main_P5();


extern void* load( char* x ) {
  if     ( 0 == strcmp( x, "P3" ) ) {
    return &main_P3;
  }
  else if( 0 == strcmp( x, "P4" ) ) {
    return &main_P4;
  }
  else if( 0 == strcmp( x, "P5" ) ) {
    return &main_P5;
  }
  else if( 0 == strcmp( x, "P" ) ) {
    return &main_P;
  }
  else if( 0 == strcmp( x, "PH" ) ) {
    return &main_PH;
  }

  return NULL;
}

/* The behaviour of a console process can be summarised as an infinite
 * loop over three main steps, namely
 *
 * 1. write a command prompt then read a command,
 * 2. split the command into space-separated tokens using strtok, then
 * 3. execute whatever steps the command dictates.
 *
 * As is, the console only recognises the following commands:
 *
 * a. execute <program name>
 *
 *    This command will use fork to create a new process; the parent
 *    (i.e., the console) will continue as normal, whereas the child
 *    uses exec to replace the process image and thereby execute a
 *    different (named) program.  For example,
 *
 *    execute P3
 *
 *    would execute the user program named P3.
 *
 * b. terminate <process ID>
 *
 *    This command uses kill to send a terminate or SIG_TERM signal
 *    to a specific process (identified via the PID provided); this
 *    acts to forcibly terminate the process, vs. say that process
 *    using exit to terminate itself.  For example,
 *
 *    terminate 3
 *
 *    would terminate the process whose PID is 3.
 */

void break_line() {
  PL011_putc( UART1, '\n', true );
}

void main_console() {
  char* p, x[ 1024 ];

  while( 1 ) {
    break_line();
    puts( "shell$ ", 7 ); gets( x, 1024 ); p = strtok( x, " " );

    char * input = strtok( NULL, " "  );

    if     ( 0 == strcmp( p, "execute"   ) ) {
      pid_t pid = fork();
      if( 0 == pid ) {
        exec( load( input ));
      }
    }
    else if( 0 == strcmp( p, "terminate" ) ) {
      puts("terminating ",12);
      puts(input, strlen(input));
      puts("\n",1);
      int pid = atoi(input);
      exit(pid);
      //kill( pid, s );
    }
    else {
      puts( "unknown command\n", 16 );
    }
  }

  exit( EXIT_SUCCESS );
}
