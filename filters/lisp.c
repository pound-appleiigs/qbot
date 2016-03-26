#include <stdio.h>

void main(int argc, char **argv) {
 register char ch;
 register char lc='\0';

 while((ch = getchar()) != EOF) {

 if(ch=='s') {
  if(lc!='s') {
   putchar('t');
  }
  fputs("hhh", stdout);
 }
 else if(ch=='S') {
  if(lc!='S') {
   putchar('T');
  }
  fputs("HHH", stdout);
 }
 else {
  putchar(ch);
 }
 lc=ch;
 }
}

