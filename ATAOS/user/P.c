#include "P.h"



void main_P() {
  write( STDOUT_FILENO, "Spawning philosophers\n", 22 );
  int indices[16];
  for(int i = 0; i < 16; i++) {
    int index = fork();
    if (index == 0) {
      exec(load("PH"));
    }
    else {
      indices[i] = index;
    }
  }

  //exit(pid());
}
