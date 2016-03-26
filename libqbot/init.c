/* qbot
 * Written by James Sanford
 * Copyright 1996 James Sanford
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include "libq.h"


long qbot_readlong(FILE *fros) {
 unsigned char tc[4];

 if(!fread(tc, 4, 1, fros)) {
  qbot_error("unexpected eof");
  return -1;
 }

 return(tc[0] + (tc[1]<<8) + (tc[2]<<16) + (tc[3]<<24));
}


int qbot_readhashfile(char *qhfile, long quotefilelen) {
 FILE *qhfiles;
 int tin1;
 int tin2;
 char nick[NICKSIZE];
 char word[MISCBUFSIZE];
 struct nick_list *tempnick;
 struct word_list *tempword;
 long realfilelen;

 if(!(qhfiles=fopen(qhfile, "r"))) {
  qbot_error("could not open the hash file");
  return -1;
 }

 if((tin1=qbot_readlong(qhfiles)) < 0) {
  fclose(qhfiles);
  return -1;
 }

 if(tin1!=HASHFILEVERSION) {
  qbot_error("quotes.hash version difference");
  fclose(qhfiles);
  return -1;
 }

 if((realfilelen=qbot_readlong(qhfiles)) < 0) {
  fclose(qhfiles);
  return -1;
 }

 if(realfilelen!=quotefilelen) {
  qbot_error("hash file does not correspond to quotes file");
  fclose(qhfiles);
  return -1;
 }

 if((qb_totalquotes=qbot_readlong(qhfiles)) < 0) {
  qb_totalquotes=0;
  fclose(qhfiles);
  return -1;
 }

 if(!(qinorder=malloc(qb_totalquotes*sizeof(struct q_list)))) {
  qbot_error("out of memory");
  fclose(qhfiles);
  return -1;
 }

 for(tin1=0;tin1<qb_totalquotes;tin1++) {
  if((qinorder[tin1].offset=qbot_readlong(qhfiles)) < 0) {
   qbot_freestuff();
   fclose(qhfiles);
   return -1;
  }
  qinorder[tin1].displayed=0;
 }

 tin2=0;
 while((tin1=fgetc(qhfiles))) {
  if(tin1>=NICKSIZE) {
   qbot_error("nick too long - I'm aborting because I'm lame");
   qbot_freestuff();
   fclose(qhfiles);
   return -1;
  }
  if(!(fread(nick, tin1, 1, qhfiles))) {
   qbot_error("unexpected eof");
   qbot_freestuff();
   fclose(qhfiles);
   return -1;
  }
  nick[tin1]='\0';

  if(!(tempnick=qbot_addnick(nick))) {
   qbot_freestuff();
   fclose(qhfiles);
   return -1;
  }

  if((tempnick->quotes=tin1=qbot_readlong(qhfiles)) < 0) {
   qbot_freestuff();
   fclose(qhfiles);
   return -1;
  }

  while(tin1--) {
   if(!tempnick->qll) {
    if(!(tempnick->first=tempnick->qll=malloc(sizeof (struct qa_nl)))) {
     qbot_error("out of memory");
     qbot_freestuff();
     fclose(qhfiles);
     return -1;
    }
   }
   else {
    if(!(tempnick->qll->next=malloc(sizeof (struct qa_nl)))) {
     qbot_error("out of memory");
     fclose(qhfiles);
     return -1;
    }
    tempnick->qll=tempnick->qll->next;
   }
   if((tempnick->qll->qnum=qbot_readlong(qhfiles)) < 0) {
    tempnick->qll->next=NULL;
    qbot_freestuff();
    fclose(qhfiles);
    return -1;
   }
   if(tempnick->qll->qnum>qb_totalquotes) {
    qbot_error("invalid quote reference.");
    tempnick->qll->next=NULL;
    qbot_freestuff();
    fclose(qhfiles);
    return -1;
   }
  }
  tempnick->qll->next=NULL;
 }

 while((tin1=fgetc(qhfiles)) != EOF) {
  if(!(fread(word, tin1, 1, qhfiles))) {
   qbot_error("unexpected eof");
   qbot_freestuff();
   fclose(qhfiles);
   return -1;
  }
  word[tin1]='\0';

  if(!(tempword=qbot_addword(word))) {
   qbot_freestuff();
   fclose(qhfiles);
   return -1;
  }

  if((tin1=qbot_readlong(qhfiles)) < 0) {
   qbot_freestuff();
   fclose(qhfiles);
   return -1;
  }

  while(tin1--) {
   if(!tempword->wll) {
    if(!(tempword->first=tempword->wll=malloc(sizeof (struct qa_wl)))) {
     qbot_error("out of memory");
     qbot_freestuff();
     fclose(qhfiles);
     return -1;
    }
   }
   else {
    if(!(tempword->wll->next=malloc(sizeof (struct qa_wl)))) {
     qbot_error("out of memory");
     qbot_freestuff();
     fclose(qhfiles);
     return -1;
    }
    tempword->wll=tempword->wll->next;
   }
   if(((tempword->wll->qnum=qbot_readlong(qhfiles)) < 0) ||
      ((tempword->wll->wnum=qbot_readlong(qhfiles)) < 0)) {
    tempword->wll->next=NULL;
    qbot_freestuff();
    fclose(qhfiles);
    return -1;
   }
   if(tempword->wll->qnum>qb_totalquotes) {
    qbot_error("invalid quote reference.");
    tempword->wll->next=NULL;
    qbot_freestuff();
    fclose(qhfiles);
    return -1;
   }
   tempword->occ++;
  }
  tempword->wll->next=NULL;
 }
 fclose(qhfiles);
 return 0;
}


int qbot_init(char *qfile, char *qhfile) {
 char tcsname[MISCBUFSIZE];
 char *tcs1;
 struct stat statbuf;
 int qftime;
 long qflen;

 if(!qhfile || qhfile[0]=='\0') {
  qhfile=DEFAULT_QHFILE;
 }
 if((qhfile[0]!='/') && (tcs1=strrchr(qfile, '/'))) {
  if((tcs1-qfile) + strlen(qhfile) >= MISCBUFSIZE) {
   qbot_error("path/filename too long");
   return -1;
  }
  strncpy(tcsname, qfile, (tcs1-qfile)+1);
  strcpy(&tcsname[(tcs1-qfile)+1], qhfile);
 }
 else {
  strcpy(tcsname, qhfile);
 }

 if(qinorder) {
  qbot_freestuff();
 }

/* determine if hash file needs to be rebuilt */
 if(stat(qfile, &statbuf) < 0) {
  qbot_error("yikes!  who stole the quotes file?!!");
  return -1;
 }

 qftime=statbuf.st_mtime;
 qflen=statbuf.st_size;

 if((stat(tcsname, &statbuf) < 0) || qftime > statbuf.st_mtime) {
  if(qbot_rebuildhashfile(qfile, tcsname) < 0) {
   return -1;
  }
 }
 else if(qbot_readhashfile(tcsname, qflen) < 0) {
  fprintf(stderr, "rebuilding hashfile (%s)\n", qbot_io_readline());
  if(qbot_rebuildhashfile(qfile, tcsname) < 0) {
   return -1;
  }
 }

 srandom(qb_totalquotes+getpid());

 if(!(quotefiles=fopen(qfile, "r"))) {
  qbot_error("yikes!  who stole the quotes file?!!");
  return -1;
 }

 return 0;
}

