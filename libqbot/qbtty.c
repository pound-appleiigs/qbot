/* test qbot :) */

#include <stdio.h>

#ifdef ENABLE_DMALLOC
#include "../../software/t/malloc/dmalloc-3.1.3/dmalloc.h"
#endif


int main(int argc, char **argv) {
 char x[256];
 char *tcs1;
 int tin1;

 if(argc<2) {
  argv[1]="quotes";
 }
 if(argc<3) {
  argv[2]="";
 }
 if(qbot_init(argv[1], argv[2]) < 0) {
  fprintf(stderr, "some error occured.\n");
  while((tcs1=qbot_io_readline())) {
   puts(tcs1);
  }
  return 1;
 }

 putc('>',stdout);
 putc(' ',stdout);
 while(gets(x)) {
  if(!strncmp(x, "stat", 4)) {
   if(x[4]==' ') {
    if((tin1=qbot_getquotestats(&x[5])) < 0) {
     printf("error: ");
    }
   }
   else {
    if(qbot_gettotalquotes() < 0) {
     printf("error: ");
    }
   }
  }
  else if(qbot_getquote(x) < 0) {
   printf("error: ");
  }
  while((tcs1=qbot_io_readline())) {
   puts(tcs1);
  }
  putc('>',stdout);
  putc(' ',stdout);
 }

#ifdef ENABLE_DMALLOC
 qbot_shutdown();
#endif

 return 0;
}

