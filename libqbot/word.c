/* qbot
 * Written by James Sanford
 * Copyright 1996 James Sanford
 *
 * word hash stuff
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "libq.h"

#define WORDHASH_SIZE 5437
/* #define WORDHASH_SIZE 809 */

static struct word_list wordhash[WORDHASH_SIZE];

static int stringhash(char *hs) {
 unsigned ti1 = 0;
 char *tcs1;

 tcs1=hs;

 while(*tcs1) {
  ti1 += (ti1 << 5) + *tcs1++;
 }

 return (ti1 % WORDHASH_SIZE);
}


/* only call with qbot_stringlowered stuff */
struct word_list *qbot_addword(char *word) {
 struct word_list *testwordhash;

 qbot_stringlower(word);

 testwordhash=&wordhash[stringhash(word)];

 if(!testwordhash->word) {
  if(!(testwordhash->word=strdup(word))) {
   qbot_error("out of memory");
   return NULL;
  }
  testwordhash->occ=0;
  return testwordhash;
 }
 while(strcmp(word, testwordhash->word)) {
  if(testwordhash->next) {
   testwordhash=testwordhash->next;
  }
  else {
   if(!(testwordhash->next=malloc(sizeof(struct word_list)))) {
    qbot_error("out of memory");
    return NULL;
   }
   testwordhash=testwordhash->next;
   testwordhash->occ=0;
   testwordhash->first=testwordhash->wll=NULL;
   testwordhash->next=NULL;
   if(!(testwordhash->word=strdup(word))) {
    qbot_error("out of memory");
    return NULL;
   }
   break;
  }
 }

 return testwordhash;
}


struct word_list *qbot_findword(char *word) {
 struct word_list *testwordhash;

 qbot_stringlower(word);

 testwordhash=&wordhash[stringhash(word)];

 if(testwordhash->word) {
  while(strcmp(testwordhash->word, word)) {
   if(testwordhash->next) {
    testwordhash=testwordhash->next;
   }
   else {
    return NULL;
   }
  }

  return testwordhash;
 }
 else {
  return NULL;
 }
}


int qbot_writewords(FILE *quotehashfiles) {
 int tin1;
 struct qa_wl *testll;
 struct word_list *testwordhash;
 int nc;

 tin1=WORDHASH_SIZE;

 do {
  tin1--;
  testwordhash=&wordhash[tin1];
  while(testwordhash && testwordhash->word) {
   nc=strlen(testwordhash->word);
   if(!fputc(nc, quotehashfiles)) {
    qbot_error("error writing");
    return -1;
   }
   if(!fwrite(testwordhash->word, nc, 1, quotehashfiles)) {
    qbot_error("error writing");
    return -1;
   }
   if(qbot_writelong(quotehashfiles, testwordhash->occ) < 0) {
    return -1;
   }
   testll=testwordhash->first;
   while(testll) {
    if((qbot_writelong(quotehashfiles, testll->qnum) < 0) ||
       (qbot_writelong(quotehashfiles, testll->wnum) < 0)) {
     return -1;
    }
    testll=testll->next;
   }
   testwordhash=testwordhash->next;
  }
 }
 while(tin1);

 return 0;
}

void qbot_freewordrefs(struct qa_wl *fwl) {
 struct qa_wl *freewl;

 while(fwl) {
  freewl=fwl;
  fwl=fwl->next;
  free(freewl);
 }
}

void qbot_freewords(void) {
 int tin1;
 struct word_list *testwordhash;
 struct word_list *freeword;
 
 tin1=WORDHASH_SIZE;
 do {
  tin1--;
  testwordhash=&wordhash[tin1];
  if(testwordhash->word) {
   free(testwordhash->word);
   testwordhash->word=NULL;
   qbot_freewordrefs(testwordhash->first);
   testwordhash->first=testwordhash->wll=NULL;
   testwordhash->occ=0;
   freeword=testwordhash;
   testwordhash=testwordhash->next;
   freeword->next=NULL;
   while(testwordhash) {
    free(testwordhash->word);
    testwordhash->word=NULL;
    qbot_freewordrefs(testwordhash->first);
    freeword=testwordhash;
    testwordhash=testwordhash->next;
    free(freeword);
   }
  }
 }
 while(tin1);
}

