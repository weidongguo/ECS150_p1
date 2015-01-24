#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
void indent(int n){
  for(; n > 0; n--){
    printf(" ");
  }
}

int isDir(struct stat buff){
  return ((buff.st_mode) & S_IFDIR);
}

void full_path(char*fullpath, char*path, char *d_name){
  int len = strlen(path);
  strcpy(fullpath, path);
 
  if(path[len-1] == '/') { //if last character is already /, not need to append one
    strcat(fullpath, d_name);
  }
  else{ //else append a /
    strcat(fullpath, "/");
    strcat(fullpath, d_name);
  }
}

void search(DIR *dirptr, char *path, char *target, FILE* pipefp, int level){
  struct dirent *dirEntry;
  struct stat buff;

  while( (dirEntry = readdir(dirptr)) != NULL){
    if( !strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, "..")) //need this, otherwise looping back to the current diretory itself 
      continue;
    int len = (strlen(path) + strlen(dirEntry->d_name) + 1);
    char fullpath[len+1];//+1 totake '/' into account if there isn't one at the end 
    full_path(fullpath, path, dirEntry->d_name); 
   
    if( stat(fullpath, &buff) !=0){ //error checking
      perror("Error"); 
      return ;
    }

    indent(level);//indent to align files in side a directory(folder) 

    if(isDir(buff)){
      printf("%s\n", dirEntry->d_name);//directory
      DIR *dirptrNext ;
      if( (dirptrNext = opendir(fullpath)) == NULL) {//in case error opening up a new directory
        perror("ERROR Opening Directory");
        return ;
      }
      search(dirptrNext, fullpath, target,pipefp, level + 2);
    }
    else{
      printf("%s\n", dirEntry->d_name); //regular files
    }  
    if(strcmp(dirEntry->d_name, target) == 0)
        fprintf(pipefp,"%s found at %s\n", target, fullpath); //store matched result in pipe
     
  }
  closedir(dirptr);//very essential if nesting down too a deep level b/c may be openning up too many directories
}

int main(int argc, char ** argv){
  if(argc<2){
    printf("no argument has provided.\n");
    return -1;
  }else if(argc<3){
    printf("no target file provided.\n");
    return -1;
  }
  char *path = argv[1];  
  char *target = argv[2];
  
  DIR *dirptr; 
  if( (dirptr = opendir(path)) == NULL){
    perror("ERROR Opening Directory");
    return -1;
  }
  int pfd[2];
  pipe(pfd);
  FILE *pipefp = fdopen(pfd[1], "w");
  search(dirptr,path,target,pipefp,0);  
  fclose(pipefp);
  close(pfd[1]);
  int nread; char c[512];
  printf("\nSearch result:\n");  //print search result stored in pipe;
  while( (nread = read(pfd[0], c, 512)) > 0){
    write(1,c,nread);
  }
  return 0;
}
