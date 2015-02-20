#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[])
{
  int procNum = 1 ;
  int child[2] ;
  int numChildren = 0 ;
  int status ;
  int maxDepth ;
  assert(argc == 2);
  maxDepth = atoi(argv[1]); /* convert depth parameter */
  while (numChildren < 2)
  {
    if (numChildren == 0)
      printf("I am process %d, my process ID is %d"
      " and my parent's is %d.\n",
      procNum, getpid(), getppid());
    
    child[numChildren] = fork();
    if( child[numChildren] == 0)
    {
      numChildren = 0 ;
      procNum *= 2 ;
      if (! --maxDepth) exit(0);
    }
    else
    {
      numChildren++ ;
      procNum++ ;
    }
  }
  //while (numChildren) 
    wait(&status);

  return 0;
}
