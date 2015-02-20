#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<wait.h>

void print(int pnum) {
  printf("I am process %d, my process identifier is %d.\n", pnum, getpid());
}

int sizeOfPipe(int *pfd){
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
}


void twoChildren(int n ,int *pfd){
  if( n == 0)
    return;
  
  if( fork() == 0){ //first child
    write(pfd[1], "\0", 1);
    print(sizeOfPipe(pfd));
    twoChildren(n-1, pfd);
  }
  else if( fork() == 0){ //second child
    write(pfd[1], "\0", 1);
    print(sizeOfPipe(pfd));
    twoChildren(n-1, pfd);
  }
  //else{
    wait(NULL);
 // }
}


int main(int argc, char*argv[]){
  if(argc < 2) {
    printf("no command line argument provided.\nusage: process_tree [number].\n");
    return -1; 
  }
  int n = atoi(argv[1]);//convert the argument from string to actual number
  
  int pfd[2];
  pipe(pfd);
  write(pfd[1],"\0",1);
  print(sizeOfPipe(pfd)); 
  
  twoChildren(n-1, pfd);
  
  wait(NULL);  
  return 0;
}
