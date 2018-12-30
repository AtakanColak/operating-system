/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of
 * which can be found via http://creativecommons.org (and should be included as
 * LICENSE.txt within the associated archive or repository).
 */

#include "hilevel.h"

#define MAX_PROCESS 100
#define PID_CONSOLE 0

pcb_t pcb[MAX_PROCESS];
channel_t channel[MAX_PROCESS];

int free_stack_space = 0;
int executing = PID_CONSOLE;
int lpqn      = 0;

extern void     main_console();
extern uint32_t tos_usr;

int create_channel(uint32_t left, uint32_t right, uint32_t datasize) {
  for(int index = 0; index < MAX_PROCESS; index++) {
    if(channel[index].activity == CHANNEL_INACTIVE) {
      channel[index].activity = CHANNEL_ACTIVE;
      channel[index].left     = left;
      channel[index].right    = right;
      channel[index].datasize = datasize;
      channel[index].data     = malloc(datasize);
      return index;
    }
  }
  return -1;
}

void send_channel(int index, void * data) {
  memcpy(&channel[index].data, data, channel[index].datasize);
}

void * receive_channel(int index) {
  return &channel[index].data;
}

void terminate_channel(int index) {
  channel[index].activity = CHANNEL_INACTIVE;
  free(channel[index].data);
}

void initialize_process(int index) {
  pcb[index].pid    = index;
  pcb[index].status = STATUS_TERMINATED;
  pcb[index].tos    = ((uint32_t) &tos_usr) - ((uint32_t) (index * 0x1000));
  //memset(&pcb[index].ctx, 0, sizeof(pcb_t));
  pcb[index].ctx.sp = pcb[index].tos;
  pcb[index].ctx.cpsr = 0x50;
}

void initialize_array() {
  for(int i = 0; i < MAX_PROCESS; i++) {
    initialize_process(i);
    terminate_channel(i);
  }
}

int create_process() {
  for(int i = 0; i < MAX_PROCESS; i++) {
    if(pcb[i].status == STATUS_TERMINATED) {
      initialize_process(i);
      pcb[i].status = STATUS_CREATED;
      return i;
    }
  }
  return -1;
}

void queue_process(int index) {
  lpqn += 1;
  pcb[index].status   = STATUS_READY;
  pcb[index].priority.wait  = 0;
  pcb[index].priority.queue = lpqn;
}

void load_process(int index, void* program_address, int plvl) {
  pcb[index].ctx.pc   = ( uint32_t )( program_address );
  pcb[index].priority.priority = plvl;
  queue_process(index);
}

void copy_process(int child_index, int parent_index, ctx_t * ctx) {
  memcpy(&pcb[child_index].ctx, ctx, sizeof( ctx_t ));

  uint32_t stack_size = (uint32_t) (pcb[parent_index].tos - ctx->sp);

  pcb[child_index].ctx.sp = (uint32_t) pcb[child_index].tos - stack_size;

  memcpy((void *) pcb[child_index].ctx.sp, (void *) ctx->sp, stack_size);

  pcb[child_index].priority.priority = 1;

  pcb[child_index].ctx.gpr[0] = 0;
  ctx->gpr[0] = pcb[child_index].pid;

  queue_process(child_index);

}

void switch_process(ctx_t* ctx, int index) {
  if(executing != index) {
    memcpy( &pcb[ executing ].ctx, ctx, sizeof( ctx_t ) );
    queue_process(executing);
  }
  memcpy(ctx, &pcb[index].ctx, sizeof( ctx_t ) );
  pcb[index].status = STATUS_EXECUTING;
  pcb[index].priority.wait = 0;
  executing = index;
}

void terminate_process(int index) {
  initialize_process(index);
}

void time_tick() {
  for(int i = 0; i < MAX_PROCESS; i++) {
    if(pcb[i].status == STATUS_EXECUTING || pcb[i].status == STATUS_READY || pcb[i].status == STATUS_WAITING) {
      pcb[i].priority.wait += 1;
      if(pcb[i].status != STATUS_EXECUTING)  {
        pcb[i].priority.queue -= pcb[i].priority.priority * pcb[i].priority.priority;
        if(lpqn < pcb[i].priority.queue)       lpqn = pcb[i].priority.queue;
      }
    }
  }
}

int highest_priority_process() {
  int next = 0;
  for(int i = 0; i < MAX_PROCESS; i++) {
    if (pcb[i].status == STATUS_READY || pcb[i].status == STATUS_WAITING) {
      if((pcb[next].priority.queue > pcb[i].priority.queue) || ((pcb[next].priority.queue == pcb[i].priority.queue) &&
      (pcb[next].priority.wait * pcb[next].priority.priority < pcb[i].priority.wait * pcb[i].priority.priority))) {
        next = i;
      }
    }
  }
  return next;
}

void scheduler(ctx_t* ctx) {
  int next_index = highest_priority_process();

  if (pcb[executing].status != STATUS_EXECUTING) {
    switch_process(ctx, executing);
    return;
  }

  if(executing == next_index) return;

  if(pcb[executing].priority.wait > pcb[executing].priority.priority * pcb[executing].priority.priority) {
    switch_process(ctx, next_index);
  }
}

void svc_yield(ctx_t * ctx) {
  int next_index = highest_priority_process();
  switch_process(ctx, next_index);
}

void svc_write(ctx_t * ctx) {
  int   fd = ( int   )( ctx->gpr[ 0 ] );
  char*  x = ( char* )( ctx->gpr[ 1 ] );
  int    n = ( int   )( ctx->gpr[ 2 ] );

  for( int i = 0; i < n; i++ ) {
    PL011_putc( UART0, *x++, true );
  }

  ctx->gpr[ 0 ] = n;
}

void svc_fork(ctx_t * ctx) {
  int child_index = create_process();
  copy_process(child_index, executing, ctx);
}

void svc_exec(ctx_t * ctx) {
  void * pos = (void *) ctx->gpr[0];
  load_process(executing, pos, 1);
  switch_process(ctx, executing);
}

void svc_exit(ctx_t * ctx) {
  uint32_t index = (uint32_t) ctx->gpr[0];
  if(executing == index) {switch_process(ctx, highest_priority_process());}
  terminate_process(index);

}

void hilevel_handler_rst(ctx_t* ctx) {

  TIMER0->Timer1Load  = 0x00100000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

  GICC0->PMR          = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
  GICC0->CTLR         = 0x00000001; // enable GIC interface
  GICD0->CTLR         = 0x00000001; // enable GIC distributor

  //initialize_process(&main_console, &tos_console, &pcb[0], 0, 2);

  initialize_array();
  load_process(create_process(), &main_console, 1);

  int_enable_irq();
  scheduler(ctx);
  return;
}

void hilevel_handler_irq(ctx_t* ctx) {
  uint32_t id = GICC0->IAR;
  if( id == GIC_SOURCE_TIMER0 ) {
    time_tick();
    scheduler(ctx);
    TIMER0->Timer1IntClr = 0x01;

  }
  GICC0->EOIR = id;
  return;
}

void hilevel_handler_svc(ctx_t* ctx, uint32_t id) {
  switch( id ) {

    //SYS_YIELD
    case 0x00 : {
      svc_yield(ctx);
      break;
    }
    //SYS_WRITE
    case 0x01 : {
      svc_write(ctx);
      break;
    }

    //SYS_FORK
    case 0x03 : {
      svc_fork(ctx);
      break;
    }

    //SYS_EXIT      ( 0x04 )
    case 0x04 : {
      svc_exit(ctx);
      break;
    }


    //SYS_EXEC
    case 0x05 : {
      svc_exec(ctx);
      break;
    }

    // SYS_KILL      ( 0x06 )


    case 0x09 : { //0x09 => sleep()
      int ticks = (int) ctx->gpr[0];
      int prev = executing;
      svc_yield(ctx);
      pcb[prev].priority.queue += ticks;
      break;
    }

    // #define CHNL_CREATE    ( 0x0A )
    case 0x0A : {
      uint32_t left  = (uint32_t) ctx->gpr[0];
      uint32_t right = (uint32_t) ctx->gpr[1];
      uint32_t size  = (uint32_t) ctx->gpr[2];
      int channel_index = create_channel(left, right, size);
      ctx->gpr[0] = channel_index;
      break;
    }
    // #define CHNL_SEND      ( 0x0B )
    case 0x0B : {
      uint32_t index  = (uint32_t) ctx->gpr[0];
      void *   data = (void *) ctx->gpr[1];
      send_channel(index, data);
      break;
    }

    // #define CHNL_RECEIVE   ( 0x0C )
    case 0x0C : {
      uint32_t index  = (uint32_t) ctx->gpr[0];
      void * source = receive_channel(index);
      uint64_t int_source = (uint64_t) ((uintptr_t) source);
      uint32_t lower      = (uint32_t) (int_source && 0xFFFF);
      uint32_t higher     = (uint32_t) (int_source >> 32);
      ctx->gpr[0]     = lower;
      ctx->gpr[1]     = higher;
      break;
    }

    // #define CHNL_KILL      ( 0x0D )
    case 0x0D : {
      uint32_t index  = (uint32_t) ctx->gpr[0];
      terminate_channel(index);
      break;
    }
    //#define SYS_PID       ( 0x0E )
    case 0x0E : {
      ctx->gpr[0] = executing;
      break;
    }
    default   : { // 0x?? => unknown/unsupported
      break;
    }
  }
  return;
}
