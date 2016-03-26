/* qbot
 * Written by James Sanford
 * Copyright 1996 James Sanford
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "libq.h"

#define INCREMENT_SIZE 256

static int cqline;

static int cqbt;


int qbot_parsequote(void) {
 char cqd[MISCBUFSIZE];
 long cqloc;
 int cqword=0;
 char *tcs1;
 char *tcs2;

 cqloc=ftell(quotefiles);

 if(!fgets(cqd, MISCBUFSIZE, quotefiles)) {
  return 0;
 }
 cqline++;

 if(cqd[0]=='%' && cqd[1]=='%') {
  sprintf(cqd, "warning, null quote at %d", cqline);
  qbot_io_addline(cqd);
  return 1;
 }

 if(qb_totalquotes==cqbt) {
  cqbt+=INCREMENT_SIZE;
  if(!(qinorder=realloc(qinorder, cqbt*sizeof(struct q_list)))) {
   qbot_error("out of memory");
   return -1;
  }
 }
 qinorder[qb_totalquotes].offset=cqloc;
 qinorder[qb_totalquotes].displayed=0;

 do {
  if(cqd[0]=='<') {
   if((tcs2=strchr(&cqd[1], '>'))) {
    *tcs2++='\0';
    tcs1=&cqd[1];
   }
   else {
    sprintf(cqd, "malformed at %d", cqline);
    qbot_error(cqd);
    return -1;
   }
  }
  else if(cqd[0]=='*') {
   if(cqd[1]==' ') {
    if((tcs2=strchr(&cqd[2], ' '))) {
     *tcs2++='\0';
     tcs1=&cqd[2];
    }
    else {
     sprintf(cqd, "malformed at %d", cqline);
     qbot_error(cqd);
     return -1;
    }
   }
   else {
    tcs1=NULL;
    tcs2=&cqd[0];
   }
  }
  else if(cqd[0]=='-') {
   if((tcs2=strchr(&cqd[1], '-'))) {
    *tcs2++='\0';
    if((tcs1=strchr(&cqd[1], ':'))) {
     *tcs1='\0';
    }
    tcs1=&cqd[1];
   }
   else {
    sprintf(cqd, "malformed at %d", cqline);
    qbot_error(cqd);
    return -1;
   }
  }
  else {
   sprintf(cqd, "warning, malformed at %d", cqline);
   qbot_io_addline(cqd);
   tcs1=NULL;
   tcs2=&cqd[0];
  }

  if(tcs1 && (strlen(tcs1) >= NICKSIZE)) {
   sprintf(cqd, "encountered long nick at %d", cqline);
   qbot_io_addline(cqd);
   tcs1=NULL;
  }

  if(tcs1) {
   struct nick_list *tempnick;

   if(!(tempnick=qbot_addnick(tcs1))) {
    return -1;
   }

   if(!tempnick->qll) {
    if(!(tempnick->first=tempnick->qll=malloc(sizeof (struct qa_nl)))) {
     qbot_error("out of memory");
     return -1;
    }
    tempnick->qll->qnum=qb_totalquotes;
    tempnick->quotes=1;
    tempnick->qll->next=NULL;
   }
   else if(tempnick->qll->qnum != qb_totalquotes) {
    if(!(tempnick->qll->next=malloc(sizeof (struct qa_nl)))) {
     qbot_error("out of memory");
     return -1;
    }
    tempnick->qll=tempnick->qll->next;
    tempnick->qll->next=NULL;
    tempnick->qll->qnum=qb_totalquotes;
    tempnick->quotes++;
   }
  }

  while(*tcs2) {
   while(*tcs2 && !((*tcs2 >= 48 && *tcs2 <= 57) || (*tcs2 >= 65 && *tcs2 <= 90) || (*tcs2 >= 97 && *tcs2 <= 122))) {
    tcs2++;
   }
   tcs1=tcs2;
   while(*tcs1 && ((*tcs1 >= 48 && *tcs1 <= 57) || (*tcs1 >= 65 && *tcs1 <= 90) || (*tcs1 >= 97 && *tcs1 <= 122))) {
    tcs1++;
   }
   if(*tcs1) {
    *tcs1++='\0';
   }

   if(*tcs2) {
    struct word_list *tempword;

    if(!(tempword=qbot_addword(tcs2))) {
     return -1;
    }

    if(!tempword->wll) {
     if(!(tempword->first=tempword->wll=malloc(sizeof (struct qa_wl)))) {
      qbot_error("out of memory");
      return -1;
     }
     tempword->occ=1;
     tempword->wll->qnum=qb_totalquotes;
     tempword->wll->wnum=cqword;
     tempword->wll->next=NULL;
    }
    else {
     if(!(tempword->wll->next=malloc(sizeof (struct qa_wl)))) {
      qbot_error("out of memory");
      return -1;
     }
     tempword->wll=tempword->wll->next;
     tempword->occ++;
     tempword->wll->qnum=qb_totalquotes;
     tempword->wll->wnum=cqword;
     tempword->wll->next=NULL;
    }

    cqword++;
   }
   tcs2=tcs1;
  }

  cqline++;
 }
 while(fgets(cqd, MISCBUFSIZE, quotefiles) && !(cqd[0]=='%' && cqd[1]=='%'));

 qb_totalquotes++;

 return 1;
}


int qbot_writelong(FILE *fros, long wval) {
 unsigned char tc[4];

 tc[0]=wval;
 tc[1]=wval>>8;
 tc[2]=wval>>16;
 tc[3]=wval>>24;

 if(!fwrite(tc, 4, 1, fros)) {
  qbot_error("error writing");
  return -1;
 }

 return 0;
}


int qbot_writequotes(char *qhfile, long quotefilelen) {
 FILE *quotehashfiles;
 int tin1;

 if(!(quotehashfiles=fopen(qhfile, "w"))) {
  qbot_error("could not open hash file for writing");
  return -1;
 }

 if(qbot_writelong(quotehashfiles, HASHFILEVERSION) < 0) {
  goto cleanupend;
 }
 if(qbot_writelong(quotehashfiles, quotefilelen) < 0) {
  goto cleanupend;
 }
 if(qbot_writelong(quotehashfiles, qb_totalquotes) < 0) {
  goto cleanupend;
 }
 for(tin1=0;tin1<qb_totalquotes;tin1++) {
  if(qbot_writelong(quotehashfiles, qinorder[tin1].offset) < 0) {
   goto cleanupend;
  }
 }

 if(qbot_writenicks(quotehashfiles) < 0) {
  goto cleanupend;
 }

 fputc('\0', quotehashfiles);

 if(qbot_writewords(quotehashfiles) == 0) {
  fflush(quotehashfiles);
  fclose(quotehashfiles);
  return 0;
 }

cleanupend:
 fclose(quotehashfiles);
 unlink(qhfile);
 return -1;
}


int qbot_rebuildhashfile(char *qfile, char *qhfile) {
 char cqd[MISCBUFSIZE];
 int tin1;

 if(!(quotefiles=fopen(qfile, "r"))) {
  qbot_error("yikes!  who stole the quotes file?!!");
  return -1;
 }

 do {
  cqline++;
 }
 while(fgets(cqd, MISCBUFSIZE, quotefiles) && !(cqd[0]=='%' && cqd[1]=='%'));

 if(!(qinorder=malloc(INCREMENT_SIZE * sizeof(struct q_list)))) {
  qbot_error("out of memory");
  fclose(quotefiles);
  return -1;
 }
 cqbt=INCREMENT_SIZE;

 qbot_io_start();

 while((tin1=qbot_parsequote()) > 0);
 if(tin1 < 0) {
  fclose(quotefiles);
  return -1;
 }

 if(qbot_writequotes(qhfile, ftell(quotefiles)) < 0) {
  fclose(quotefiles);
  return -1;
 }

 fclose(quotefiles);
 return 0;
}

