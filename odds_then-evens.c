/* Weidong Guo*/
#include <unistd.h>
#include <fcntl.h>

void printLines(int srcFd,int destFd, int parity) {
  int line = 1, nread; char buff; 
  while( (nread = read(srcFd, &buff, 1)) > 0){
    if(buff == '\n')
      line++;
    if(line % 2 == parity)
      write(destFd, &buff, nread);
  }
  if( lseek(srcFd, 0,0) == -1) //rewind the src file for next read 
    write(destFd, "ERROR: can't seek back, file maybe removed\n",63-20);
}


int main(int argc, char **argv){
  int fd;
  if(argc != 2){ //make sure a file is provided to read from
    write(1, "ERROR: no command line argument provided to read\n", 64-15);
    return -1;
  }
  
  if( (fd = open(argv[1], 0)) == -1){ // open for read-only
    write(1, "ERROR: can't open the file\n", 42-15); 
    return -1;
  }
  /* prints odd line first to stdout */
  printLines(fd, 1, 1); 
  /* prints even line afterward to stdout*/ 
  printLines(fd, 1, 0);
  

  close(fd);
  return 0;
}
