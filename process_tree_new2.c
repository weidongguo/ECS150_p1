#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<wait.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
/* need to use signal to control the behavior of the child processes */
short nChildDone = 0; 

void phandler(){
  nChildDone++;
}
void chandler() {
}

void received() {
  while(nChildDone != 2) 
    pause(); //make sure the both children have indeed sent signal
  nChildDone = 0;
}

void print(int pnum) {
  printf("I am process %d, my process identifier is %d.\n", pnum, getpid());
}

/*int sizeOfPipe(int *pfd){
  int nread = 0;char c;
  while( read(pfd[0], &c, 1) ){
    nread++; 
    if( c == '\0')
       break;
  }
  int i = 0;
  for(i = 0 ; i < nread; i++) {//refill the pipe
    write(pfd[1], "a", 1);
  }

  return nread;
}*/

void twoChildren(int n){
  if( n == 0)
    return;
  static struct sigaction act;
  act.sa_handler = phandler;
  sigaction(SIGUSR1, &act, NULL); //set up signal handler for parent and potentially for child processes
 

  pid_t pid1, pid2; 
  if( (pid1 = fork()) == 0 ){ // 1st child process
    act.sa_handler = chandler;
    sigaction(SIGUSR1, &act, NULL);
    
    kill(getppid(), SIGUSR1);//signal parent that I've just been created. 
    pause();//waiting for parent to let me fly out
    print(0);/*do something before flying out */
    twoChildren(n-1); //step down a level
    exit(1); 
  }
  else if( (pid2 = fork()) == 0 ){ // 2nd child process
    act.sa_handler = chandler;
    sigaction(SIGUSR1, &act, NULL);
    kill(getppid(), SIGUSR1);//signal parent that I've just been created.
    pause(); //waiting for parent to let me fly out.
    print(0); 
    twoChildren(n-1);//step down a level
    exit(1);   
  }
  else if( pid1 == -1 || pid2 == -1)
    perror("ERROR from fork()");
  else{ //parent's job
    printf("nChildDone: %d\n", nChildDone); 
    while(nChildDone != 2) 
      pause(); //make sure the both children have indeed sent signal to me, no need to call pause twice 
              //if the signal get back to me before the first call of pause()
    nChildDone = 0; 
    printf("passed pause()");
    kill(pid1, SIGUSR1); /*make sure both signal are sent*/
    kill(pid2, SIGUSR1); 
    printf("at wait()\n");
    while( wait(NULL) != - 1){ //wait until all child processes terminated
      if(errno == ECHILD) //ECHILD indicates no child processes
        break;
    }
  }
}

int main(int argc, char*argv[]){
  if(argc < 2) {
    printf("no command line argument provided.\nusage: process_tree [number].\n");
    return -1; 
  }
  int n = atoi(argv[1]);//convert the argument from string to actual number
  
  int pfd[2];
  pipe(pfd);
  //write(pfd[1],"\0",1);
  //print(sizeOfPipe(pfd)); 
  print(1); 
  
  sigset_t mask;
  sigfillset(&mask);
 
  twoChildren(n-1);
  

  execl("/bin/ps", "ps","-al", NULL);
  return 0;
}
