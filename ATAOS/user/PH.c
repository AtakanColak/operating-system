#include "PH.h"

#define STDOUT_FILENO ( 1 )

void main_PH() {
  write( STDOUT_FILENO, "Philosopher Number ", 19 );
  char * pid_c = (char *) malloc(sizeof(char) * 3);
  int id = pid();
  itoa(pid_c, id);
  write( STDOUT_FILENO, pid_c, strlen(pid_c));
  write( STDOUT_FILENO, "\n", 1);

  int upper_channel = channel_create(id, (id % 16) + 1, sizeof(int));
  int lower_channel = (upper_channel + 15) % 16;
  char * chnlnam = (char *)malloc(sizeof(char) * 3);
  itoa(chnlnam, upper_channel);
  write( STDOUT_FILENO, "Channel Number ", 15 );
  write( STDOUT_FILENO, chnlnam, strlen(chnlnam));
  write( STDOUT_FILENO, "\n", 1);

  int one = 1;
  int zero = 0;
  //initialize forks to 1
  channel_send(upper_channel, (void *) &one);
  //channel_send(lower_channel, (void *) &one);
  yield();
  int result;
  //char * res = (char *)malloc(sizeof(char) * 3);
  //itoa(res, result);
  //write( STDOUT_FILENO, res, strlen(res));
  int hungry = 1;
  int first_fork = 0;
  while(hungry) {
    channel_receive(lower_channel, sizeof(int), (void *) &result);
    if(result == 1) {
      channel_send(lower_channel, (void *) &zero);
      first_fork = 1;
      while(first_fork) {
        sleep(3);
        channel_receive(upper_channel, sizeof(int), (void *) &result);
        if (result == 1) {
          channel_send(upper_channel, (void *) &zero);
          sleep(3);
          hungry = 0;
          first_fork = 0;
          channel_send(upper_channel, (void *) &one);
          channel_send(lower_channel, (void *) &one);
        }

      }
    }
  }
  write( STDOUT_FILENO, "A Philosopher finished eating\n", 30 );

  exit(id);
}
