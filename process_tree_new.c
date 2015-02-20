#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<wait.h>

typedef struct node_t{
  int pid; 
  struct node_t *right; 
  struct node_t *left;
}node_t;

node_t * newNode(){
  node_t *ptr = (node_t*)malloc(sizeof(node_t));
  ptr->left = NULL;
  ptr->right = NULL;
  ptr->pid = -1;//indicate not set
  return ptr;
}


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


void twoChildren(int n ,int *pfd, node_t *center){
  if( n == 0)
    return;
  
  if( fork() == 0){ //first(left) child
    write(pfd[1], "\0", 1);
    print(sizeOfPipe(pfd));
    center->left = newNode();
    (center->left)->pid = getpid(); 

    twoChildren(n-1, pfd, center->left );
    exit(1);
  }
  else if( fork() == 0){ //second(child) child
    write(pfd[1], "\0", 1);
    print(sizeOfPipe(pfd));
    center->right = newNode();
    (center->right)->pid = getpid();
    twoChildren(n-1, pfd, center->right);
    exit(1);
  }
  else{
    wait(NULL);
  }
}

void depthFirstTraversal(node_t *head){
  if(head == NULL)
    return;
  printf("pid: %d\n", head->pid);
  depthFirstTraversal(head->left);
  depthFirstTraversal(head->right);
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
  
  node_t head; 
  head.pid = getpid();

  twoChildren(n-1, pfd, &head);
  
  printf("\n......\n");
  
  wait(NULL); 
  depthFirstTraversal(&head);

  return 0;
}
