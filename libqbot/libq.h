/* qbot
 * Written by James Sanford
 * Copyright 1996 James Sanford
 *
 */


#ifdef ENABLE_DMALLOC
#include "../../software/t/malloc/dmalloc-3.1.3/dmalloc.h"
#endif

#define MISCBUFSIZE 256
#define DEFAULT_QHFILE "quotes.hash"

#define LIBVERSION 960724
#define HASHFILEVERSION 6

extern FILE *quotefiles;

extern int qb_totalquotes;

struct q_list {
 long offset;
 unsigned int displayed;
#ifdef DO_MD5
 char digest[16];
#endif
};

extern struct q_list *qinorder;


struct qa_nl {
 int qnum;
 struct qa_nl *next;
};

#define NICKSIZE 24

struct nick_list {
 char nick[NICKSIZE];
 int quotes;
 struct qa_nl *first;
 struct qa_nl *qll;
 struct nick_list *next;
};


struct qa_wl {
 int qnum;
 int wnum;
 struct qa_wl *next;
};

struct word_list {
 char *word;
 int occ;
 struct qa_wl *first;
 struct qa_wl *wll;
 struct word_list *next;
};

/* generic.c */
/* char *jscpy(char *, ...); */
/* void mergeanddestroyqanl(struct qa_nl **, struct qa_nl *); */
/* void mergeqanl(struct qa_nl **, struct qa_nl *); */
int qbot_addnicktolist(char *, struct qa_nl **);
int qbot_addwordstolist(char *, struct qa_nl **);
/* struct qa_nl *qbfm(char *); */
struct qa_nl *qbot_findmatch(char *);
int qbot_getquoteall(void); /* user */
int qbot_getquote(char *); /* user */
int qbot_getquotestats(char *); /* user, returns # */
int qbot_gettotalquotes(void); /* user, returns # */

/* init.c */
long qbot_readlong(FILE *);
int qbot_readhashfile(char *, long);
int qbot_init(char *, char *); /* user */

/* io.c */
void qbot_io_start(void);
void qbot_io_addline(char *);
char *qbot_io_readline(void);
void qbot_error(char *);

/* nick.c */
void qbot_stringlower(char *);
/* int stringhash(char *); */
struct nick_list *qbot_addnick(char *);
struct nick_list *qbot_findnick(char *);
void qbot_freequoterefs(struct qa_nl *);
void qbot_freestuff(void);
int qbot_writenicks(FILE *);

/* word.c */
/* int stringhash(char *); */
struct word_list *qbot_addword(char *);
struct word_list *qbot_findword(char *);
void qbot_freestuff(void);
int qbot_writewords(FILE *);

/* rehash.c */
int qbot_parsequote(void);
int qbot_writelong(FILE *, long);
int qbot_writequotes(char *, long);
int qbot_rebuildhashfile(char *, char *);


