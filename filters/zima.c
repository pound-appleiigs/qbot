#include <stdio.h>

void main(int argc, char **argv) {
 register char ch;

 while((ch = getchar()) != EOF) {

 if(ch=='s') {
  ch='z';
 }
 else if(ch=='S') {
  ch='Z';
 }

 putchar(ch);
 }
}

