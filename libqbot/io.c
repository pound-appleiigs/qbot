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


#include "libq.h"

struct qbio {
 char line[MISCBUFSIZE];
 struct qbio *next;
};

static struct qbio outofmemio = {
 "out of memory!", NULL
};

static struct qbio *current;
static struct qbio *last;

static struct qbio *tofree;
static struct qbio *tofreelast;

void qbot_io_start(void) {
 struct qbio *tempfree;

 current=NULL;

 while(tofree) {
  tempfree=tofree->next;
#ifdef DEBUG
  fprintf(stderr, "--remove %X\n", tofree);
#endif
  free(tofree);
  tofree=tempfree;
 }

}


void qbot_io_addline(char *line) {
 struct qbio *tempio;
 int tin1;

 tin1=strlen(line);

 if(tin1>=MISCBUFSIZE) {
  return;
 }

 tin1--;

 if(line[tin1]=='\n') {
  line[tin1]='\0';
 }
 if(!(tempio=malloc(sizeof(struct qbio)))) {
  qbot_io_start();
  current=&outofmemio;
  return;
 }

 strcpy(tempio->line, line);
 tempio->next=NULL;
 if(!tofree) {
  tofree=tempio;
  tofreelast=tempio;
 }
 else {
  tofreelast->next=tempio;
  tofreelast=tempio;
 }

 if(!current) {
  current=tempio;
  last=tempio;
 }
 else {
  last->next=tempio;
  last=tempio;
 }
#ifdef DEBUG
 fprintf(stderr, "--add %X\n", tempio);
#endif
}

char *qbot_io_readline(void) {
 struct qbio *tempio;

 if(current) {
  tempio=current;
  current=current->next;
  return tempio->line;
 }
 else {
  return NULL;
 }
}


void qbot_error(char *errstring) {
 qbot_io_start();
 qbot_io_addline(errstring);
}

