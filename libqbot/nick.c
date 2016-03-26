/* qbot
 * Written by James Sanford
 * Copyright 1996 James Sanford
 *
 * nick hash stuff
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>

#include "libq.h"

/* #define NICKHASH_SIZE 5437 */
#define NICKHASH_SIZE 809

static struct nick_list nickhash[NICKHASH_SIZE];


void qbot_stringlower(char *tcs1) {
 char tc1;

 do {
  tc1=tolower(*tcs1);
  *tcs1=tc1;
 }
 while(*tcs1++);

}


static int stringhash(char *hs) {
 unsigned ti1 = 0;
 char *tcs1;

 tcs1=hs;

 while(*tcs1) {
  ti1 += (ti1 << 5) + *tcs1++;
 }

 return (ti1 % NICKHASH_SIZE);
}


struct nick_list *qbot_addnick(char *nick) {
 struct nick_list *testnickhash;

 qbot_stringlower(nick);

 testnickhash=&nickhash[stringhash(nick)];

 if(!testnickhash->quotes) {
  strcpy(testnickhash->nick, nick);
  return testnickhash;
 }
 while(strcasecmp(nick, testnickhash->nick)) {
  if(testnickhash->next) {
   testnickhash=testnickhash->next;
  }
  else {
   if(!(testnickhash->next=malloc(sizeof(struct nick_list)))) {
    qbot_error("out of memory");
    return NULL;
   }
   testnickhash=testnickhash->next;
   testnickhash->quotes=0;
   testnickhash->first=testnickhash->qll=NULL;
   testnickhash->next=NULL;
   strcpy(testnickhash->nick, nick);
   break;
  }
 }

 return testnickhash;
}


struct nick_list *qbot_findnick(char *nick) {
 struct nick_list *testnickhash;

 qbot_stringlower(nick);

 testnickhash=&nickhash[stringhash(nick)];

 while(strcmp(testnickhash->nick, nick)) {
  if(testnickhash->next) {
   testnickhash=testnickhash->next;
  }
  else {
   return NULL;
  }
 }

 return testnickhash;
}


void qbot_freequoterefs(struct qa_nl *fql) {
 struct qa_nl *freeql;

 while(fql) {
  freeql=fql;
  fql=fql->next;
  free(freeql);
 }
}

void qbot_freestuff(void) {
 int tin1;
 struct nick_list *testnickhash;
 struct nick_list *freenick;

 free(qinorder);
 qinorder=NULL;
 qb_totalquotes=0;

 if(quotefiles) {
  fclose(quotefiles);
  quotefiles=NULL;
 }

 tin1=NICKHASH_SIZE;
 do {
  tin1--;
  testnickhash=&nickhash[tin1];
  if(testnickhash->first) {
   qbot_freequoterefs(testnickhash->first);
   testnickhash->first=testnickhash->qll=NULL;
   testnickhash->nick[0]='\0';
   testnickhash->quotes=0;
   freenick=testnickhash;
   testnickhash=testnickhash->next;
   freenick->next=NULL;
   while(testnickhash) {
    qbot_freequoterefs(testnickhash->first);
    freenick=testnickhash;
    testnickhash=testnickhash->next;
    free(freenick);
   }
  }
 }
 while(tin1);

 qbot_freewords();
}

int qbot_writenicks(FILE *quotehashfiles) {
 int tin1;
 struct qa_nl *testll;
 struct nick_list *testnickhash;
 int nc;

 tin1=NICKHASH_SIZE;

 do {
  tin1--;
  testnickhash=&nickhash[tin1];
  while(testnickhash && testnickhash->quotes) {
   nc=strlen(testnickhash->nick);
   if(nc>=NICKSIZE) {
    char tcsbuf[MISCBUFSIZE*2];
    sprintf(tcsbuf, "encountered long nick '%s'", testnickhash->nick);
    qbot_error(tcsbuf);
    return -1;
   }
   if(!fputc(nc, quotehashfiles)) {
    qbot_error("error writing");
    return -1;
   }
   if(!fwrite(testnickhash->nick, nc, 1, quotehashfiles)) {
    qbot_error("error writing");
    return -1;
   }
   if(qbot_writelong(quotehashfiles, testnickhash->quotes) < 0) {
    return -1;
   }
   testll=testnickhash->first;
   while(testll) {
    if(qbot_writelong(quotehashfiles, testll->qnum) < 0) {
     return -1;
    }
    testll=testll->next;
   }
   testnickhash=testnickhash->next;
  }
 }
 while(tin1);

 return 0;
}

