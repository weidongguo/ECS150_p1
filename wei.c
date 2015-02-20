#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
int main()
{
  char c[] = "12d3cd";
  char d = '1';
  printf("%s\n%d\n", c, isdigit('9'));
  return 0;
}
