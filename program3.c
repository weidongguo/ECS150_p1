#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void fork_variable_test(){
  printf("***Fork-variable-test***\n"); 
  int val = 50, pid;
  pid = fork();
  wait(NULL); // let child process finishes first 
  printf("[%s process] right after fork() -> val: %d\n", (pid == 0 ? "child":"parent"), val);
  
  /* only child process is going to do this, to show the independence of child and parent processes */
  if(pid == 0) {
    val = 100;//only child process modifies val;
  }
  printf("[%s process] after modifying val in child process -> val: %d\n", (pid == 0 ? "child":"parent"), val);
  
  if(pid == 0)
    exit(1);

}

void ppn(int *n) { 
  printf("file descriptor for the pipe is %d(read), %d(write)\n", n[0], n[1]);
}

void file_descriptor_test(){
  printf("***File-descriptor-test***\n");
  int pfd[2], pid; char buff[1024]; 
  pipe(pfd);
  pid = fork();
  wait(NULL); //let child process finishes first;

  printf("[%s process] ", (pid == 0 ? "child":"parent"));
  ppn(pfd);
  
  if( pid == 0){
    write(pfd[1], "[child process] writes to the pipe. ", 56-20);
    close(pfd[1]); 
    printf("pfd[1] = %d is closed in child process after writing something to it\n", pfd[1]);
    exit(0); 
  }
  else {
    write(pfd[1], "[parent process] writes to the pipe. ", 57-20);
  } 

  if( pid != 0 ) { //print the content of the pipe in parent's process  
    int nread = read(pfd[0], buff, 1024);
    
    write(1, "\nContents read from the pipe: \n\n{ ",50-16);
    write(1, buff, nread);
    write(1, "}\n", 2);
  }
}

int main() {
  fork_variable_test();
  write(1, "----------------\n", 30-13);
  file_descriptor_test();

  return 0;
}
