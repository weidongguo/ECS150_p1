/* Weidong Guo*/
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>

void waitCheck(){
  if(wait(NULL) == -1)
    perror("wait()");
  return;
}

void reverse(char *str){
  int len = strlen(str), i,j;
  for(i = 0, j = len-1; i <= len/2 -1 ; i++, j--) {
    char temp;
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
  }
}

void uppercase(char *str){
  int len = strlen(str), i;
  for(i = 0; i < len ; i++) {
    if( str[i] >= 'a' && str[i] <= 'z')
      str[i] = str[i] - ('a' - 'A'); 
  }
}

void ppn(int *n) { //for testing only
  printf("file descriptor for the pipe is %d(read), %d(write)\n", n[0], n[1]);
}

int main()
{
  char ch; int p2id, len = 0;  
  int pfd[2], pfd2to1[2]; 
  pipe(pfd); pipe(pfd2to1);
  
  write(1, "Please enter a string: ", 36-13); 
  
  while(1) {
    read(0, &ch, 1);
    if( ch == '\n') {
      write(pfd[1], "\0", 1);//append '\0' so can use string.h functions to process the string 
      break;
    }
    len++;
    write(pfd[1], &ch, 1);
    write(1, &ch, 1);
  }
  write(1, "\n", 1); 
  char str[len+1];
  if( (p2id = fork()) == 0 ) { // ***start process 2
    close(pfd[1]); // ensure no read block from pipe
    read(pfd[0], str, len + 1);
    char originalStr[len+1];
    strcpy(originalStr, str);
    reverse(str); 
    printf("%s\n", str);

    close(pfd[0]);//reuse the spot on the per-process file descriptor table copied from parent
    pipe(pfd);//resue the variable pfd , piping from process 2 to 3
    write(pfd[1], str, len + 1); // pass the string in the pipe between process 2 and 3
    int pfd3to2[2], p3id; 
    pipe(pfd3to2); //set up pipe come from process 3 back to process 2
    if( (p3id = fork()) == 0) { //***start process 3
      close(pfd[1]); //ensure no read block from pipe
      read(pfd[0], str, len + 1); 
      uppercase(str); 
      printf("%s\n", str);
      
      write(pfd3to2[1], str, len + 1); //put the string to the pipe back to process 2
      close(pfd3to2[1]); //close file descriptor in process 3 has nothing to do with its parent p2
      close(pfd3to2[0]);
    } // ***end process 3

    if( p3id != 0) {//want only process 2 to do this
      waitCheck();//let process 3 finishes first 
      char newStr[len*2+1];
      close(pfd3to2[1]); //ensure no read block
      read(pfd3to2[0], newStr, len+1);
      strcat(newStr, originalStr); 
      printf("%s\n", newStr);
      close(pfd3to2[0]);//close file descriptor for piping from process 3 to 2 
      
      write(pfd2to1[1], newStr, len*2+1); // pipe output all the way back to process 1
    } 
  
  } //*end process 2
  if( p2id != 0) {  //force only process 1 to run this
    waitCheck(); //let processes 2 finishes first
    close(pfd[0]);
    close(pfd[1]);
    char finalStr[len*2+1];
    close(pfd2to1[1]); //ensure no read blcok
    read(pfd2to1[0], finalStr, len*2+1);
    printf("compare: %d\n", strcmp(finalStr, str)); 
    close(pfd2to1[0]);
  }   
  return 0;
}
