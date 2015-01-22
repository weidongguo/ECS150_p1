#include <signal.h>
#include <stdio.h>

int flag = 0;

void handler(int signum){
  flag = 1;
}

int main(){
  int count = 0, pid, pid2;
  struct sigaction act;
  act.sa_handler = handler;
  sigaction(SIGALRM, &act, NULL);
 
  if((pid = fork()) == 0){ //first child hits this case, won't try to fork() another child in else if
    for(;;){
      if(flag){
        printf("Count is %d, my pid is %d\n", count, getpid());
        count = 0;
        flag = 0;
      }
      count++;
    }
  }
  else if( (pid2 = fork()) == 0){ //second child
    for(;;){
      if(flag){
        printf("Count is %d, my pid is %d\n", count, getpid());
        count = 0;
        flag = 0;
      }
      count++;
    }
  }

  else{
    int i;
    for(i = 0; i<5; i++) {
      sleep(5);
      kill(pid, SIGALRM);
      kill(pid2,SIGALRM);
    }
  }
  sleep(1);//let the child prints everything first, so the shell won't take over before the child finish
  return 0;
}
