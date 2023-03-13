#include <stdio.h>
#include <string.h>

int main(void)
{
  char *str = strdup("sihtgubed");
  char *stri = &str[8];
  char *buf[9];
  char **bufi;
  char **bufend;
  bufi = buf;
  bufend = &buf[9];

  while (bufi != bufend){
    *bufi = stri;
    bufi++;
    stri--;
  }

  while (bufi != buf){
    *(*bufi) -= 32;
    bufi--;
  }

  while (bufi != bufend){
    putchar(**bufi);
    bufi++;
  }

  putchar('\n');

  return 0;
}
