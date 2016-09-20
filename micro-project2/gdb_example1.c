#include "stdio.h"

void print_scrambled(char *message)
{
  register int i = 3;
  if (message == NULL) {
  	return;
  }
  do {

  		printf("%c", (*message)+i);

  } while (*++message);
  printf("\n");
}

int main()
{
  char * bad_message = NULL;
  char * good_message = "Hello, world.";

  print_scrambled(good_message);
  print_scrambled(bad_message);
}