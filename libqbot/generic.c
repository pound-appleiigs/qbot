/* qbot
 * Written by James Sanford
 * Copyright 1996 James Sanford
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


#define ZAPLIST(__l) { struct qa_nl *free__l; while(__l) { free__l=__l; \
                       __l=(__l)->next; free(free__l); } }

#define ZAPWLIST(__l) { struct qa_wl *free__l; while(__l) { free__l=__l; \
                       __l=(__l)->next; free(free__l); } }


#include "libq.h"

FILE *quotefiles;

int qb_totalquotes;

struct q_list *qinorder;

static char *jscpy(char *, ...);

static char *jscpy(char *destination, ...) {
 va_list copylist;
 register char ch;
 char *ccs;

 va_start(copylist, destination);

 while((ccs = va_arg(copylist, char *)) != NULL) {
  while((ch = *(ccs++))) {
   *(destination++) = ch;
  }
 }

 va_end(copylist);

 *destination = '\0';
 return destination;
}


static void mergeanddestroyqanl(struct qa_nl **result, struct qa_nl *destroy) {
 struct qa_nl *respos;
 struct qa_nl *testll;
 struct qa_nl *lastpos;

 lastpos=NULL;
 respos=*result;

 while(respos && destroy) {
  if(respos->qnum > destroy->qnum) {
   testll=destroy;
   destroy=destroy->next;
   free(testll);
  }
  else if(respos->qnum < destroy->qnum) {
   lastpos=respos;
   respos=respos->next;
  }
  else {
   if(!lastpos) {
    *result=respos->next;
   }
   else {
    lastpos->next=respos->next;
   }
   testll=respos;
   respos=respos->next;
   free(testll);
   testll=destroy;
   destroy=destroy->next;
   free(testll);
  }
 }
 while(destroy) {
  testll=destroy;
  destroy=destroy->next;
  free(testll);
 }
}


static void mergeqanl(struct qa_nl **result, struct qa_nl *toadd) {
 struct qa_nl *lastpos;
 struct qa_nl *respos;
 struct qa_nl *testll;

 lastpos=NULL;
 respos=*result;

 while(toadd) {
  if(!respos) {
   if(!lastpos) {
    *result=toadd;
   }
   else {
    lastpos->next=toadd;
   }
   return;
  }
  if(respos->qnum > toadd->qnum) {
   if(!lastpos) {
    *result=toadd;
   }
   else {
    lastpos->next=toadd;
   }
   lastpos=toadd;
   testll=toadd->next;
   toadd->next=respos;
   toadd=testll;
  }
  else if(respos->qnum < toadd->qnum) {
   lastpos=respos;
   respos=respos->next;
  }
  else {
   testll=toadd;
   toadd=toadd->next;
   free(testll);
  }
 }
}


int qbot_addnicktolist(char *nick, struct qa_nl **destlist) {
 char tcsbuf[MISCBUFSIZE*2];
 struct nick_list *testnick;
 struct qa_nl *testll;
 struct qa_nl *testlist;
 struct qa_nl *newlist;

 if(!(testnick=qbot_findnick(nick))) {
  if(*destlist) {
   testlist=(*destlist)->next;
   free(*destlist);
   *destlist=NULL;
   while(testlist) {
    testll=testlist;
    testlist=testlist->next;
    free(testll);
   }
  }
  sprintf(tcsbuf, "%s who?", nick);
  qbot_error(tcsbuf);
  return -1;
 }
 testlist=*destlist;
 if(!testlist) {
  if(!(testlist=malloc(sizeof(struct qa_nl)))) {
   qbot_error("out of memory");
   return -1;
  }
  *destlist=testlist;
  testlist->qnum=testnick->first->qnum;
  newlist=testnick->first->next;
  while(newlist) {
   if(!(testlist->next=malloc(sizeof(struct qa_nl)))) {
/* free *destlist */
    ZAPLIST(*destlist);
    qbot_error("out of memory");
    return -1;
   }
   testlist=testlist->next;
   testlist->qnum=newlist->qnum;
   newlist=newlist->next;
  }
  testlist->next=NULL;
 }
 else {
  struct qa_nl *lastlist=NULL;
  newlist=testnick->first;
  while(testlist && newlist) {
   if(testlist->qnum > newlist->qnum) {
    newlist=newlist->next;
   }
   else if(testlist->qnum < newlist->qnum) {
    if(*destlist==testlist) {
     *destlist=testlist->next;
    }
    else {
     lastlist->next=testlist->next;
    }
    testll=testlist;
    testlist=testlist->next;
    free(testll);
   }
   else {
    lastlist=testlist;
    testlist=testlist->next;
    newlist=newlist->next;
   }
  }
  if(lastlist) {
   lastlist->next=NULL;
  }
  while(testlist) {
   if(*destlist==testlist) {
    *destlist=NULL;
   }
   testll=testlist;
   testlist=testlist->next;
   free(testll);
  }
 }
 return 0;
}


int qbot_addwordstolist(char *words, struct qa_nl **destlist) {
 char tcsbuf[MISCBUFSIZE*2];
 struct word_list *testword;
 struct qa_wl *reallist = NULL;
 struct qa_wl *testlist = NULL;
 struct qa_nl *testlistn;
 struct qa_wl *newlist;
 struct qa_wl *testll;
 struct qa_nl *testlln;
 char *tcs1;
 char *tcs2;

 tcs2=words;

 while(*tcs2) {
  while(*tcs2 && !((*tcs2 >= 48 && *tcs2 <= 57) ||
                  (*tcs2 >= 65 && *tcs2 <= 90) ||
                  (*tcs2 >= 97 && *tcs2 <= 122))) {
   tcs2++;
  }
  tcs1=tcs2;
  while(*tcs1 && ((*tcs1 >=48 && *tcs1 <= 57) || (*tcs1 >= 65 && *tcs1 <= 90) || (*tcs1 >= 97 && *tcs1 <= 122))) {
   tcs1++;
  }
  if(*tcs1) {
   *tcs1++='\0';
  }

  if(tcs2[0]=='\0') {
   tcs2=tcs1;
   continue;
  }

  if(!(testword=qbot_findword(tcs2))) {
   ZAPLIST(*destlist);
   ZAPWLIST(reallist);
   sprintf(tcsbuf, "'%s'?", tcs2);
   qbot_error(tcsbuf);
   return -1;
  }

  if(!reallist) {
   struct qa_wl *testlist2=NULL;
   struct qa_wl *wordlist = testword->first;

   while(wordlist) {
    if(!testlist2) {
     if(!(reallist=testlist2=malloc(sizeof(struct qa_wl)))) {
      qbot_error("out of memory");
      return -1;
     }
    }
    else {
     if(!(testlist2->next=malloc(sizeof(struct qa_wl)))) {
      ZAPWLIST(reallist);
      qbot_error("out of memory");
      return -1;
     }
     testlist2=testlist2->next;
    }
    testlist2->qnum=wordlist->qnum;
    testlist2->wnum=wordlist->wnum;
    wordlist=wordlist->next;
   }
   testlist2->next=NULL;
  }
  else {
   struct qa_wl *lastlist=NULL;
   newlist=testword->first;
   testlist=reallist;
   while(newlist && testlist) {
    if(newlist->qnum < testlist->qnum) {
     newlist=newlist->next;
    }
    else if(newlist->qnum > testlist->qnum) {
     if(reallist==testlist) {
      reallist=testlist->next;
     }
     else {
      lastlist->next=testlist->next;
     }
     testll=testlist;
     testlist=testlist->next;
     free(testll);
    }
    else {
     if(newlist->wnum < testlist->wnum+1) {
      newlist=newlist->next;
     }
     else if(newlist->wnum > testlist->wnum+1) {
      if(reallist==testlist) {
       reallist=testlist->next;
      }
      else {
       lastlist->next=testlist->next;
      }
      testll=testlist;
      testlist=testlist->next;
      free(testll);
     }
     else {
      testlist->wnum++;
      lastlist=testlist;
      testlist=testlist->next;
      newlist=newlist->next;
     }
    }
   }
   if(lastlist) {
    lastlist->next=NULL;
   }
   while(testlist) {
    if(reallist==testlist) {
     reallist=NULL;
    }
    testll=testlist;
    testlist=testlist->next;
    free(testll);
   }
  }
  if(!reallist) {
   ZAPLIST(*destlist);
   return 0;
  }
  tcs2=tcs1;
 }

 if(!reallist) {
  ZAPLIST(*destlist);
  return 0;
 }

 testlistn=*destlist;
 if(!testlistn) {
  if(!(testlistn=malloc(sizeof(struct qa_nl)))) {
   ZAPWLIST(reallist);
   qbot_error("out of memory");
   return -1;
  }
  *destlist=testlistn;
  testlistn->qnum=reallist->qnum;
  newlist=reallist->next;
  while(newlist) {
   if(testlistn->qnum != newlist->qnum) {
    if(!(testlistn->next=malloc(sizeof(struct qa_nl)))) {
 /* free *destlist */
 /* free reallist */
     ZAPLIST(*destlist);
     ZAPWLIST(reallist);
     qbot_error("out of memory");
     return -1;
    }
    testlistn=testlistn->next;
    testlistn->qnum=newlist->qnum;
   }
   newlist=newlist->next;
  }
  testlistn->next=NULL;
 }
 else {
  struct qa_nl *lastlist=NULL;
  newlist=reallist;
  while(testlistn && newlist) {
   if(testlistn->qnum > newlist->qnum) {
    newlist=newlist->next;
   }
   else if(testlistn->qnum < newlist->qnum) {
    if(*destlist==testlistn) {
     *destlist=testlistn->next;
    }
    else {
     lastlist->next=testlistn->next;
    }
    testlln=testlistn;
    testlistn=testlistn->next;
    free(testlln);
   }
   else {
    lastlist=testlistn;
    testlistn=testlistn->next;
/*    newlist=newlist->next; */
   }
  }
  if(lastlist) {
   lastlist->next=NULL;
  }
  while(testlistn) {
   if(*destlist==testlistn) {
    *destlist=NULL;
   }
   testlln=testlistn;
   testlistn=testlistn->next;
   free(testlln);
  }
 }

 ZAPWLIST(reallist);

 return 0;
}


static struct qa_nl *qbfm(char *str1) {
 char tcsbuf[MISCBUFSIZE];
 char *tcs1;
 char *start;
 char *end;
 char *transit=NULL;
 char *str1end;

 int qlevel=0;
 int slevel=-1;

 struct qa_nl *locallist=NULL;

 tcs1=str1;

#ifdef DEBUG
 fprintf(stderr, "-- %s\n", str1);
 fflush(stdout);
#endif

 {
 int innot=0;
 int nlevel=-1;

 while(*tcs1) {
  switch(*tcs1) {
   case '(':
    if(innot && nlevel==-1) {
     nlevel=qlevel;
    }
    qlevel++;
    break;
   case ')':
    qlevel--;
    if(qlevel<0) {
     qbot_error("() mismatch");
     errno=1;
     return NULL;
    }
    if(nlevel==qlevel) {
     innot=0; 
     nlevel=-1;
    }
    else if(nlevel==-1) {
     innot=0;
    }
    break;
   case '\'':
   case '\"':
    while(*++tcs1 && *tcs1!='\'' && *tcs1!='\"');
    if(!*tcs1) {
     qbot_error("'/\" mismatch");
     errno=1;
     return NULL;
    }
    break;
   case '!':
    if(!innot) {
     innot=1;
    }
    break;
   case '|':
    if(tcs1[1]=='|') {
     if(nlevel==-1) {
      innot=0;
      if(slevel==-1 || qlevel<slevel) {
       slevel=qlevel;
       transit=tcs1;
      }
     }
     tcs1++;
    }
    else {
     qbot_error("|?");
     errno=1;
     return NULL;
    }
    break;
   case '&':
    if(tcs1[1]=='&') {
     if(nlevel==-1) {
      innot=0;
     }
    }
    break;
   default:
    if(nlevel==-1) {
     innot=0;
    }
    break;
  }
  tcs1++;
 }
 }
 str1end=tcs1;

 if(qlevel!=0) {
  qbot_error("() mismatch");
  errno=1;
  return NULL;
 }

 if(!transit) {
  struct qa_nl *notlist=NULL;

  int npos=0;

  int nicknext=1;
  int lastwasop=1;
  int tonot=0;

  tcs1=str1;


  do {
   switch(*tcs1) {
    case '(':
    case ')':
    case ' ':
    case '&':
    case '\0':
     if(npos) {
      tcsbuf[npos]='\0';
      npos=0;
      if(qbot_addnicktolist(tcsbuf, &locallist) < 0) {
/* free locallist */
/* free notlist */
       ZAPLIST(locallist);
       ZAPLIST(notlist);
       errno=1;
       return NULL;
      }
      if(locallist==NULL) {
/* this just means no matches */
/* free notlist */
       ZAPLIST(notlist);
       errno=0;
       return NULL;
      }
     }
     break;
   }
   switch(*tcs1) {
    case '(':
     if(tonot) {
      struct qa_nl *localnot=NULL;
      char *tcstart;

      tcs1++;
      tcstart=tcs1;

      while(*tcs1) {
       if(*tcs1=='(') {
        qlevel++;
       }
       else if(*tcs1==')') {
        if(!qlevel) {
         *tcs1='\0';
         tcs1++;
         break;
        }
        qlevel--;
       }
       tcs1++;
      }

      if(!(localnot=qbfm(tcstart)) && errno) {
/* free locallist */
/* free notlist */
       ZAPLIST(locallist);
       ZAPLIST(notlist);
/* (carry over errno from last) */
       return NULL;
      }
      mergeqanl(&notlist, localnot);

      tonot=0;
      lastwasop=0;
      nicknext=1;
      break;
     }
     if(!lastwasop) {
      qbot_error("expected operator");
/* free locallist */
/* free notlist */
      ZAPLIST(locallist);
      ZAPLIST(notlist);
      errno=1;
      return NULL;
     }
     break;
    case ')':
     if(lastwasop) {
      qbot_error("expected nick");
/* free locallist */
/* free notlist */
      ZAPLIST(locallist);
      ZAPLIST(notlist);
      errno=1;
      return NULL;
     }
     nicknext=1;
     break;
    case ' ':
     nicknext=1;
     break;
    case '&':
     if(tcs1[1]=='&') {
      if(lastwasop) {
       qbot_error("expected nick");
/* free locallist */
/* free notlist */
       ZAPLIST(locallist);
       ZAPLIST(notlist);
       errno=1;
       return NULL;
      }
      lastwasop=1;
      nicknext=1;
      tcs1++;
     }
     else {
      qbot_error("&?");
/* free locallist */
/* free notlist */
      ZAPLIST(locallist);
      ZAPLIST(notlist);
      errno=1;
      return NULL;
     }
     break;
    case '\0':
     if(lastwasop) {
      qbot_error("expected nick");
/* free locallist */
/* free notlist */
      ZAPLIST(locallist);
      ZAPLIST(notlist);
      errno=1;
      return NULL;
     }
     break;
    case '\'':
    case '\"':
     if(nicknext && !lastwasop) {
      qbot_error("expected operator");
/* free locallist */
/* free notlist */
      ZAPLIST(locallist);
      ZAPLIST(notlist);
      errno=1;
      return NULL;
     }
     tcs1++;
     while(*tcs1 && *tcs1!='\'' && *tcs1!='\"') {
      tcsbuf[npos++]=*tcs1++;
     }
     if(!*tcs1) {
      qbot_error("' mismatch");
/* free locallist */
/* free notlist */
      ZAPLIST(locallist);
      ZAPLIST(notlist);
      errno=1;
      return NULL;
     }
     tcsbuf[npos]='\0';
     npos=0;

     if(tonot) {
      struct qa_nl *localnot=NULL;
      if(qbot_addwordstolist(tcsbuf, &localnot) < 0) {
/* free locallist */
/* free notlist */
       ZAPLIST(locallist);
       ZAPLIST(notlist);
       errno=1;
       return NULL;
      }
      mergeqanl(&notlist, localnot);
     }
     else {
      if(qbot_addwordstolist(tcsbuf, &locallist) < 0) {
/* free locallist */
/* free notlist */
       ZAPLIST(locallist);
       ZAPLIST(notlist);
       errno=1;
       return NULL;
      }
      if(locallist==NULL) {
/* this just means no matches */
/* free notlist */
       ZAPLIST(notlist);
       errno=0;
       return NULL;
      }
     }
     tonot=0;
     lastwasop=0;
     nicknext=1;
     break;
    case '!':
     if(tonot) {
      qbot_error("expected nick or subexpression");
/* free locallist */
/* free notlist */
      ZAPLIST(locallist);
      ZAPLIST(notlist);
      errno=1;
      return NULL;
     }
     if(nicknext) {
      if(!lastwasop) {
       qbot_error("expected operator");
/* free locallist */
/* free notlist */
       ZAPLIST(locallist);
       ZAPLIST(notlist);
       errno=1;
       return NULL;
      }
      tonot=1;
      break;
     }
    default:
     if(nicknext && !lastwasop) {
      qbot_error("expected operator");
/* free locallist */
/* free notlist */
      ZAPLIST(locallist);
      ZAPLIST(notlist);
      errno=1;
      return NULL;
     }
     if(tonot) {
      struct qa_nl *localnot=NULL;

      while(*tcs1 && *tcs1!=' ' && *tcs1!='&' && *tcs1!='(' && *tcs1!=')') {
       tcsbuf[npos++]=*tcs1++;
      }
      tcsbuf[npos]='\0';
      npos=0;

      if(qbot_addnicktolist(tcsbuf, &localnot) < 0) {
/* free locallist */
/* free notlist */
       ZAPLIST(locallist);
       ZAPLIST(notlist);
       errno=1;
       return NULL;
      }
      mergeqanl(&notlist, localnot);

      tonot=0;
      lastwasop=0;
      nicknext=1;
      break;
     }
     lastwasop=0;
     nicknext=0;
     tcsbuf[npos++]=*tcs1;
     break;
   }
  }
  while(*tcs1++);

  if(!locallist) {
   int tin1;
   struct qa_nl *testlist;

   if(!(locallist=malloc(sizeof (struct qa_nl)))) {
/* free notlist */
    ZAPLIST(notlist);
    qbot_error("out of memory");
    errno=1;
    return NULL;
   }
   testlist=locallist;
   testlist->qnum=0;
   for(tin1=1;tin1<qb_totalquotes;tin1++) {
    if(!(testlist->next=malloc(sizeof (struct qa_nl)))) {
/* free locallist */
/* free notlist */
     ZAPLIST(locallist);
     ZAPLIST(notlist);
     qbot_error("out of memory");
     errno=1;
     return NULL;
    }
    testlist=testlist->next;
    testlist->qnum=tin1;
   }
   testlist->next=NULL;
  }
  mergeanddestroyqanl(&locallist, notlist);
 }
 else {
  struct qa_nl *locallist2=NULL;

  if(slevel>0) {
   tcs1=str1;
   while(tcs1<str1end) {
    switch(*tcs1) {
     case '(':
      qlevel++;
      if(qlevel==slevel) {
       if(tcs1>transit) {
	tcs1=str1end;
       }
       else {
	start=tcs1;
       }
      }
      break;
     case ')':
      if(qlevel==slevel) {
       end=tcs1;
      }
      qlevel--;
      break;
     case '\'':
     case '\"':
      while(*++tcs1 && *tcs1!='\'' && *tcs1!='\"');
      break;
     default:
      break;
    }
    tcs1++;
   }
   *start++='\0';
   *end++='\0';
  }

  *transit='\0';
  transit+=2;
  if(slevel>0) {
   jscpy(tcsbuf, str1, start, end, NULL);
#ifdef ORERROR
   if(!(locallist2=qbfm(tcsbuf)) && errno) {
/* free locallist */
    ZAPLIST(locallist);
    return NULL;
   }
#else
   locallist2=qbfm(tcsbuf);
#endif
   mergeqanl(&locallist, locallist2);
 /* hack */
   jscpy(tcsbuf, str1, "(", transit, ")", end, NULL);
#ifdef ORERROR
   if(!(locallist2=qbfm(tcsbuf)) && errno) {
/* free locallist */
    ZAPLIST(locallist);
    return NULL;
   }
#else
   locallist2=qbfm(tcsbuf);
#endif
   mergeqanl(&locallist, locallist2);
  }
  else {
   strcpy(tcsbuf, str1);
#ifdef ORERROR
   if(!(locallist2=qbfm(tcsbuf)) && errno) {
/* free locallist */
    ZAPLIST(locallist);
    return NULL;
   }
#else
   locallist2=qbfm(tcsbuf);
#endif
   mergeqanl(&locallist, locallist2);
   strcpy(tcsbuf, transit);
#ifdef ORERROR
   if(!(locallist2=qbfm(tcsbuf)) && errno) {
/* free locallist */
    ZAPLIST(locallist);
    return NULL;
   }
#else
   locallist2=qbfm(tcsbuf);
#endif
   mergeqanl(&locallist, locallist2);
  }
 }
#ifdef DEBUG
 {
  struct qa_nl *tempdump;
  int nm1=0;
  tempdump=locallist;
  printf("matches:");
  while(tempdump) {
   printf(" %d", tempdump->qnum);
   tempdump=tempdump->next;
   nm1++;
  }
  printf("\n%d matches\n", nm1);
 }
#endif
 errno=0;
 return locallist;
}


struct qa_nl *qbot_findmatch(char *criteria) {
 struct qa_nl *finalmatch;

 if(strlen(criteria)>=MISCBUFSIZE) {
  qbot_error("request too long");
  errno=1;
  return NULL;
 }

 return (!(finalmatch=qbfm(criteria)) && errno) ? NULL : finalmatch;
}


int qbot_getquoteall(void) {
 char tcsbuf[MISCBUFSIZE];
 int wquote;

 if(!qb_totalquotes) {
  qbot_error("no quotes?");
  return -1;
 }

 wquote=(random() % qb_totalquotes);

 qinorder[wquote].displayed++;

 fseek(quotefiles, qinorder[wquote].offset, 0);

 qbot_io_start();
 while(fgets(tcsbuf, MISCBUFSIZE, quotefiles) &&
       !(tcsbuf[0]=='%' && tcsbuf[1]=='%')) {
  qbot_io_addline(tcsbuf);
 }

 return 0;
}


int qbot_getquote(char *criteria) {
 char tcsbuf[MISCBUFSIZE];
 int wquote;
 int matches=0;
 int mqnum;
 unsigned int lowestdisplayed=-1;
 struct qa_nl *firstmatch;
 struct qa_nl *matchlist;
 struct qa_nl *testll;
 struct qa_nl *freell;

 if(criteria[0]=='\0') {
  return(qbot_getquoteall());
 }

 if(!(matchlist=qbot_findmatch(criteria)) && errno) {
  return -1;
 }

 if(!matchlist) {
  qbot_error("no matches");
  return -1;
 }

 testll=matchlist;
 firstmatch=NULL;
 matchlist=NULL;
 matches=0;

 while(testll) {
  if(qinorder[testll->qnum].displayed < lowestdisplayed) {
   lowestdisplayed=qinorder[testll->qnum].displayed;
   ZAPLIST(firstmatch);
   firstmatch=matchlist=testll;
   testll=testll->next;
   matchlist->next=NULL;
   matches=1;
  }
  else if(qinorder[testll->qnum].displayed == lowestdisplayed) {
   matchlist->next=testll;
   matchlist=testll;
   testll=testll->next;
   matchlist->next=NULL;
   matches++;
  }
  else {
   freell=testll;
   testll=testll->next;
   free(freell);
  }
 }

#ifdef DEBUG
 fprintf(stderr, "- %d unshown\n", matches);
#endif

 wquote=(random() % matches);

 testll=firstmatch;
 matches=0;

 while(testll) {
  if(matches==wquote) {
   mqnum=testll->qnum;
  }
  matches++;
  freell=testll;
  testll=testll->next;
  free(freell);
 }

 qinorder[mqnum].displayed++;
 fseek(quotefiles, qinorder[mqnum].offset, 0);

 qbot_io_start();
 while(fgets(tcsbuf, MISCBUFSIZE, quotefiles) && !(tcsbuf[0]=='%' && tcsbuf[1]=='%')) {
  qbot_io_addline(tcsbuf);
 }

 return 0;
}


int qbot_getquotestats(char *criteria) {
 char tcsbuf[MISCBUFSIZE];
 struct qa_nl *matchlist;
 struct qa_nl *testll;
 int matches=0;

 if(criteria[0]=='\0') {
  return(qbot_gettotalquotes());
 }

 if(!(matchlist=qbot_findmatch(criteria)) && errno) {
  return -1;
 }

 while(matchlist) {
  matches++;
  testll=matchlist;
  matchlist=matchlist->next;
  free(testll);
 }

 sprintf(tcsbuf, "%d quote%s", matches, (matches!=1)?"s":"");
 qbot_io_start();
 qbot_io_addline(tcsbuf);

 return matches;
}


int qbot_gettotalquotes(void) {
 char tcsbuf[MISCBUFSIZE];

 sprintf(tcsbuf, "%d quote%s", qb_totalquotes, (qb_totalquotes!=1)?"s":"");
 qbot_io_start();
 qbot_io_addline(tcsbuf);

 return qb_totalquotes;
}


void qbot_shutdown(void) {
 if(qinorder) {
  qbot_freestuff();
 }
 qbot_io_start();
}


int qbot_version(void) {
 char tcsbuf[MISCBUFSIZE];

 sprintf(tcsbuf, "libqbot %d", LIBVERSION);
 qbot_io_start();
 qbot_io_addline(tcsbuf);

 return 0;
}


