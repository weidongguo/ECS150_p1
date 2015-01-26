#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<wait.h>
#include<errno.h>
#include<ctype.h>
void print(int pnum) {
  printf("I am process %d, my process identifier is %d.\n", pnum, getpid());
}

int twoToTheN(int n){
  int val = 1; 
  for(; n > 0; n--)
    val = val * 2;
  return val;
}

int firstNum(char*str){
  int i;
  for(i = 0; str[i] != '\0'; i++)
    if(isdigit(str[i]))
      break;
  return atoi(str+i);
}
void fill(short *ar, int size){
  int i = 0;
  for(i = 0; i < size; i++){
    ar[i] = 0;
  }
}

void printPipe(int fd, int level){ //annoying, get rid of duplicate line
  char c[2]; short visited[twoToTheN(level)-1];
  fill(visited, twoToTheN(level));
  char str[1024];
  str[0] = '\0'; 
  c[1] = '\0';
  while( read(fd, c, 1) > 0){
    strcat(str,c); 
    if( c[0] == '\n'){
      if(!visited[firstNum(str)]){
        visited[firstNum(str)] = 1;
        printf("%s",str);
      }
      str[0] = '\0'; 
    }
  }
}


int main(int argc, char*argv[]){
  if(argc < 2) {
    printf("no command line argument provided.\nusage: process_tree [number].\n");
    return -1; 
  }
  int n = atoi(argv[1]);//convert the argument from string to actual number
  int pfd[2], p[2];
  pipe(pfd); pipe(p);
  FILE *fp = fdopen(pfd[1], "w");
  int pid[2]; 
  int nChild = 0, pNum = 1;
  int ancestor = getpid();
  while( nChild < 2 ){ // create 2 child processes
    if(nChild == 0){
      fprintf(fp,"I am process %d, my process identifier is %d.\n", pNum, getpid());
      pNum = pNum * 2 ;
    }
    if( nChild == 1) 
      pNum = pNum  + 1; // *2+1 from original
    
    if( (pid[nChild] = fork()) == 0){ // child process's turn
      nChild = 0; // i am gonna become a parent
      if( --n == 0){
        exit(1);
      }
    }
    else{
      nChild++;
    }
      
  }
  close(pfd[1]);
  fclose(fp);
  wait(NULL);
  //waitAll();
  if(ancestor == getpid()) {
    int standardout = dup(1); 
    printf("\r"); 
    
    dup2(p[1], 1);

    printPipe(pfd[0], n);//output all goes to the pipe
    close(1);
    close(p[1]);

    dup2(standardout, 1); 
    
    dup2(p[0], 0); 
    execl("/usr/bin/sort", "sort", "-n", "-k 4", NULL); 
    
  }
  
  return 0;
}
