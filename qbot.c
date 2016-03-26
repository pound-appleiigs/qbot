/* qbot irc bot.
 * Copyright 1996 James Sanford
 * Written by James Sanford
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MISCBUFSIZE 256
#define SOCKBUFSIZE 4096

#define MAXFILTERS 256

#define DEFAULT_IRCSERVER "irc.freei.net"
#define DEFAULT_NICK "QBot"
#define DEFAULT_IRCNAME "Only The Original - Since 1992"
#define DEFAULT_CHANNEL "#appleiigs"
#define DEFAULT_PORT 6665

#define VERSION "QBot 970209"

#define CONFFILE "qbot.conf"
#define QF_NAME "quotes"

char *progname;
char *nick;
char *ircserver;
char *ircname;
char *channel;
int port;

struct { char key; char *name; } filter[MAXFILTERS+1];

int fake;
int debug;

int iamquitting;

FILE *ircstreamin;
FILE *ircstreamout;

FILE *errlog;

void f_privmsg(char *, char *, char *);
void f_welcome(char *, char *, char *);
void f_nickinuse(char *, char *, char *);
void f_nick(char *, char *, char *);

extern char *qbot_io_readline(void);

struct { char *type; void(*function)(char *, char *, char *); } f_type[] = {
 { "PRIVMSG",	f_privmsg },
 { "001",	f_welcome },
 { "433",	f_nickinuse },
 { "NICK",	f_nick },
 { NULL,	NULL }
};


void showerror(char *errmsg) {
 if(errlog) {
  fprintf(errlog, "%s\n", errmsg);
  fflush(errlog);
 }
}

void show_qbstat(char *destination, char *expression) {
 char foomessage[MISCBUFSIZE];
 char *tcs1;
 int error = 0;

 if(expression[0]=='\0') {
  strcpy(foomessage, "total : ");
  if((qbot_gettotalquotes()) < 0) {
   error=1;
  }
  while((tcs1=qbot_io_readline())) {
   fprintf(ircstreamout, "PRIVMSG %s :%s%s%s\n",
    destination, error ? "error: " : "", foomessage, tcs1);
  }
  fflush(ircstreamout);
 }
 else if(expression[0]==' ') {
  sprintf(foomessage, "%s : ", &expression[1]);
  if((qbot_getquotestats(&expression[1])) < 0) {
   error=1;
  }
  while((tcs1=qbot_io_readline())) {
   fprintf(ircstreamout, "PRIVMSG %s :%s%s%s\n",
    destination, error ? "error: " : "", foomessage, tcs1);
  }
  fflush(ircstreamout);
 }
}


void outofmemory(void) {
 showerror("out of memory");
 exit(1);
}


void sigchld(void) {
 pid_t pid;

 while((pid=wait3(NULL, WNOHANG, NULL)) > 0);
}


void sighup(void) {
 if(readfilters(CONFFILE) < 0) {
  showerror("could not reread filters");
 }
 qbot_freestuff();
 if(qbot_init(QF_NAME, "") < 0) {
  char *tcs1;
  fprintf(stderr, "reload failed:");
  while((tcs1=qbot_io_readline())) {
   showerror(tcs1);
  }
  exit(1);
 }
}


void showfilteroutput(char *filters) {
 char tcsbuf[MISCBUFSIZE];
 char *tcs1;
 char *tcs2;
 int tfd[2];
 int iread;
 int iwrite;

 pipe(tfd);
 iwrite=tfd[1];

 while(*filters) {
  char *filtername=NULL;
  int theirstdin;
  int theirstdout;
  int tin1;

  for(tin1=0;filter[tin1].name;tin1++) {
   if(*filters==filter[tin1].key) {
    filtername=filter[tin1].name;
    break;
   }
  }
  if(!filtername) {
   close(iwrite);
   close(tfd[0]);
   fprintf(ircstreamout, "PRIVMSG %s :error: unknown filter '%c'\n",
    channel, *filters);
   fflush(ircstreamout);
   return;
  }
  theirstdin=tfd[0];
  pipe(tfd);
  theirstdout=tfd[1];

  if(!fork()) {
   close(iwrite);
   close(tfd[0]);
   dup2(theirstdin, STDIN_FILENO);
   close(theirstdin);
   dup2(theirstdout, STDOUT_FILENO);
   close(theirstdout);

   sprintf(tcsbuf, "filters/%s", filtername);
   if(execl(tcsbuf, filtername, NULL) < 0) {
    fprintf(stdout, "could not exec '%s'\n", filtername);
   }
   exit(0);
  }

  close(theirstdin);
  close(theirstdout);
  filters++;
 }
  
 iread=tfd[0];

 {
  fd_set toproc;
  fd_set fromproc;
  int maxfd;
  int doneread = 0;
  int donewrite = 0;
  int startnewline = 1;

  maxfd = ((iread > iwrite) ? iread : iwrite) + 1;

  FD_ZERO(&toproc);
  FD_ZERO(&fromproc);

  while(!doneread) {
   if(!donewrite) {
    FD_SET(iwrite, &toproc);
   }
   FD_SET(iread, &fromproc);

   if(select(maxfd, &fromproc, &toproc, NULL, NULL) < 0) {
    if(errno==EINTR) {
     continue;
    }
    close(iwrite);
    close(iread);
    return;
   }

   if(FD_ISSET(iwrite, &toproc)) {
    FD_CLR(iwrite, &toproc);
    if(!(tcs1=qbot_io_readline())) {
     close(iwrite);
     iwrite=0;
     donewrite=1;
    }
    else {
     char nline='\n';
     write(iwrite, tcs1, strlen(tcs1));
     write(iwrite, &nline, 1);
    }
   }

   if(FD_ISSET(iread, &fromproc)) {
    int nbx;
    FD_CLR(iread, &fromproc);
    if((nbx=read(iread, tcsbuf, MISCBUFSIZE-1)) < 0) {
     if(errno!=EINTR) {
      fputc('\n', ircstreamout);
      close(iwrite);
      iwrite=0;
      close(iread);
      return;
     }
    }
    else if(nbx==0) {
     close(iread);
     if(iwrite) {
      close(iwrite);
      iwrite=0;
     }
     doneread=1;
    }
    else {
     tcsbuf[nbx]='\0';
     tcs1=tcsbuf;
     while(tcs1 && *tcs1) {
      if(startnewline) {
       fprintf(ircstreamout, "PRIVMSG %s :", channel);
       startnewline=0;
      }
      if((tcs2=strchr(tcs1, '\n'))) {
       *tcs2++='\0';
       startnewline=1;
      }
      fputs(tcs1, ircstreamout);
      if(startnewline) {
       fputc('\n', ircstreamout);
      }
      tcs1=tcs2;
     }
    }
   }
  }
 }

 fflush(ircstreamout);
}


void showoutput(int error, int isstat, char *foomessage) {
 char *tcs1;

 while((tcs1=qbot_io_readline())) {
  fprintf(ircstreamout, "PRIVMSG %s :%s%s%s\n",
   channel, error ? "error: " : "", isstat ? foomessage : "", tcs1);
 }
 fflush(ircstreamout);
}


int hasspecialauth(char *userhost) {
 char *username;
 char *hostname;

 if(!(username=strchr(userhost, '@'))) {
  return 1;
 }
 *username='\0';

 if((username-userhost>=6 ? strcmp(username-6, "irsman") : 1) &&
    (username-userhost>=8 ? strcmp(username-8, "jsanford") : 1)) {
  return 0;
 }

 username++;
 hostname=username+strlen(username);

 if((username-userhost>=7 ? strcmp(hostname-7, "iag.net") : 1) &&
    (username-userhost>=8  ? strcmp(hostname-8, "iaxs.net") : 1)) {
  return 0;
 }

 return 1;
}


void f_nickinuse(char *from, char *to, char *message) {
 
 fprintf(stderr, "%s: nick in use\n", nick);
 exit(1);
}


void f_nick(char *from, char *to, char *message) {
 int tin1 = strlen(nick);

 if(!strncmp(from, nick, tin1) && from[tin1]=='!') {
  free(nick);
  if(!(nick=strdup(message))) {
   outofmemory();
  }
 }

}


void f_welcome(char *from, char *to, char *message) {

 if(!fake) {
  if(!debug) {
   if(fork()) {
    exit(0);
   } 
  }
#if 0
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
#endif
 }
 fprintf(ircstreamout, "MODE %s :+i\n", nick);
 if(channel) {
  fprintf(ircstreamout, "JOIN :%s\n", channel);
 }
 fflush(ircstreamout);
}


void f_privmsg(char *from, char *to, char *message) {
 char *tcs1;

 if(message[0]=='\001') {
  message++;
  if((tcs1=strchr(message, '\001'))) {
   *tcs1='\0';
  }
  if((tcs1=strchr(from, '!'))) {
   *tcs1='\0';
  }
  if(!strcasecmp(message, "version")) {
   qbot_version();
   fprintf(ircstreamout, "NOTICE %s :\001VERSION"
    " %s / %s - Written by James Sanford\001\n",
    from, VERSION, qbot_io_readline());
   fflush(ircstreamout);
  }
  else if(!strncasecmp(message, "ping ", 5)) {
   fprintf(ircstreamout, "NOTICE %s :\001PING %s\001\n", from, &message[5]);
   fflush(ircstreamout);
  }
  return;
 }
 if(!strcasecmp(nick, to)) {
  if (!strcmp(message, "help")) {
   char filterlist[MISCBUFSIZE];
   int tin1;
   for(tin1=0;filter[tin1].key;tin1++) {
    filterlist[tin1]=filter[tin1].key;
   }
   filterlist[tin1]='\0';
   if((tcs1=strchr(from, '!'))) {
    *tcs1='\0';
   }
   fprintf(ircstreamout, "PRIVMSG %s :qbot[%s] [expression] : [%s]"
    " are filters in any order and combination, expression should contain"
    " nicks and context words (enclosed in '') to search for delimited by"
    " &&, ||, !, and ()\n", from, filterlist, filterlist);
   fprintf(ircstreamout, "PRIVMSG %s :qbstat [expression] : return number"
    " of quotes that match expression\n", from);
   fprintf(ircstreamout, "PRIVMSG %s :(filters:", from);
   for(tin1=0;filter[tin1].key;tin1++) {
    fprintf(ircstreamout, " %c=%s", filter[tin1].key, filter[tin1].name);
   }
   fprintf(ircstreamout, ")\n");
   fflush(ircstreamout);
  }
  else if(!strncmp(message, "qbstat", 6)) {
   if((tcs1=strchr(from, '!'))) {
    *tcs1='\0';
   }
   show_qbstat(from, &message[6]);
  }
  else if(hasspecialauth(from)) {
   if(!strncmp(message, "nick ", 5)) {
    fprintf(ircstreamout, "NICK :%s\n", &message[5]);
    fflush(ircstreamout);
   }
   else if(!strncmp(message, "quit", 4)) {
    fprintf(ircstreamout, "QUIT :%s\n", message[4]==' '?&message[5]:"zap");
    fflush(ircstreamout);
    iamquitting=1;
   }
   else if(!strncmp(message, "say ", 4)) {
    fprintf(ircstreamout, "PRIVMSG %s :%s\n", channel, &message[4]);
    fflush(ircstreamout);
   }
   else if(!strncmp(message, "reload", 6)) {
    if((tcs1=strchr(from, '!'))) {
     *tcs1='\0';
    }
    fprintf(ircstreamout, "PRIVMSG %s :yes, sir.\n", from);
    fflush(ircstreamout);
    if(readfilters(CONFFILE) < 0) {
     showerror("could not reread filters");
    }
    qbot_freestuff();
    if(qbot_init(QF_NAME, "") < 0) {
     fprintf(stderr, "reload failed:");
     while((tcs1=qbot_io_readline())) {
      showerror(tcs1);
     }
     exit(1);
    }
   }
  }
  return;
 }
 if((tcs1=strchr(from, '!'))) {
  *tcs1='\0';
 }
 if(!strcasecmp(channel, to)) {
  if(!strncmp(message, "qbot", 4)) {
   char *arg;
   if((arg=strchr(&message[4], ' '))) {
    *arg++='\0';
   }
   if((!arg ? qbot_getquoteall() : qbot_getquote(arg)) < 0) {
    showoutput(1, 0, NULL);
   }
   showfilteroutput(&message[4]);
  }
  else if(!strncmp(message, "qbstat", 6)) {
   show_qbstat(channel, &message[6]);
  }
  else if(!strcmp(message, "qbtotal")) {
   show_qbstat(channel, &message[7]);
  }
  else {
   return;
  }
 }
 return;
}


int realqbot(void) {
 char tcsbuf[MISCBUFSIZE];
 char *from;
 char *type;
 char *to;
 char *message;
 int tin1;

 while(fgets(tcsbuf, MISCBUFSIZE, ircstreamin)) {
  tin1=strlen(tcsbuf)-1;
  while(tcsbuf[tin1]=='\r' || tcsbuf[tin1]=='\n') {
   tcsbuf[tin1]='\0';
   tin1--;
  }
  if(!(type=strchr(tcsbuf, ' '))) {
   continue;
  }
  *type++='\0';
  if(!strcmp(tcsbuf, "PING")) {
   fprintf(ircstreamout, "PONG %s\n", type);
   fflush(ircstreamout);
   continue;
  }
  if(!(to=strchr(type, ' '))) {
   continue;
  }
  *to++='\0';
  if(!(message=strchr(to, ' '))) {
   message=to;
   to=NULL;
  }
  else {
   *message++='\0';
  }

  if(message[0]==':') {
   message++;
  }

  from=tcsbuf;
  if(from[0]==':') {
   from++;
  }

  if(!(strcmp(tcsbuf, "ERROR"))) {
   if(!(strcmp(type, ":Closing")) && !(strcmp(to, "Link:"))) {
    char *tcs1;
    if((tcs1=strchr(message, ' ')) && tcs1[1]=='(') {
     if(iamquitting) {
      exit(0);
     }
     fclose(ircstreamin);
     fclose(ircstreamout);
     return 0;
    }
    {
     char tcsbuf2[MISCBUFSIZE];

     sprintf(tcsbuf2, "FATAL: %s %s %s", type, to, message);

     showerror(tcsbuf2);

     exit(1);
    }
   }
   {
    char tcsbuf2[MISCBUFSIZE];
    sprintf(tcsbuf2, "some error: %s %s %s", type, to, message);
    showerror(tcsbuf2);
   }
   continue;
  }

  for(tin1=0;f_type[tin1].type;tin1++) {
   if(!strcmp(type, f_type[tin1].type)) {
    f_type[tin1].function(from, to, message);
    break;
   }
  }
 }
 fclose(ircstreamin);
 fclose(ircstreamout);

 return 0;
}


int run_qbot(void) {
 struct sockaddr_in sa;
 struct hostent *hptr;
 int sbufsize;
 int sock;
 char *tcs1;

 if(!fake) {
 if((tcs1=strchr(ircserver, ':'))) {
  *tcs1++='\0';
  port=atoi(tcs1);
  if(!port) {
   fprintf(stderr, "%s: warning, invalid port '%s'  using default\n",
    progname, tcs1);
   port=DEFAULT_PORT;
  }
 }
 else if(!port) {
  port=DEFAULT_PORT;
 }

 if(!(hptr=gethostbyname(ircserver))) {
  perror("gethostbyname");
  return -1;
 }

 memset(&sa, 0, sizeof(sa));
 memcpy(&sa.sin_addr, hptr->h_addr, hptr->h_length);
 sa.sin_family = hptr->h_addrtype;
 sa.sin_port = htons(port);

 if((sock=socket(sa.sin_family, SOCK_STREAM, IPPROTO_TCP)) < 0) {
  perror("socket");
  return -1;
 }

 sbufsize=SOCKBUFSIZE;
 setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &sbufsize, sizeof(sbufsize));

 sbufsize=1;
 setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &sbufsize, sizeof(sbufsize));

 while(connect(sock, &sa, sizeof(sa)) < 0) {
  perror("connect");
  if(errno==ECONNREFUSED || errno == ETIMEDOUT) {
   close(sock);
   sleep (10);
   return 1;
  }
  close(sock);
  return -1;
 }

 ircstreamin=fdopen(sock, "r");
 ircstreamout=fdopen(sock, "w");
 } /* !fake */
 else {
  ircstreamin=stdin;
  ircstreamout=stdout;
 }

 {
  struct passwd *ouruser;

  ouruser=getpwuid(getuid());

  fprintf(ircstreamout, "USER %s %s %s :%s\nNICK %s\n",
   ouruser ? ouruser->pw_name : "qbot",
   ouruser ? ouruser->pw_name : "qbot",
   ouruser ? ouruser->pw_name : "qbot",
   ircname, nick);
  fflush(ircstreamout);
 }

 realqbot();

 if(fake) {
  fprintf(stderr, "blah\n");
  exit(1);
 }
 return 1;
}


int readfilters(char *filterconf) {
 FILE *f_files;
 char tcsbuf[MISCBUFSIZE];
 char *tcs1;
 char *tcs2;
 int cline=0;
 int filtnum=0;

 if((f_files = fopen(filterconf, "r")) == NULL) {
  return -1;
 }

 while(fgets(tcsbuf, MISCBUFSIZE, f_files)) {
  cline++;
  if(!(tcs1=strtok(tcsbuf, " \t\n"))) {
   continue;
  }
  if(tcs1[0]=='#' || tcs1[0]=='\n') {
   continue;
  }
  if(!(tcs2=strtok(NULL, " \t\n"))) {
   fprintf(stderr, "malformed line %d\n", cline);
   continue;
  }
  if(filtnum==MAXFILTERS) {
   fputs("reached max number of filters\n", stderr);
   break;
  }
  filter[filtnum].key=*tcs1;
  filter[filtnum++].name=strdup(tcs2);
 }
 filter[filtnum].key='\0';
 filter[filtnum].name=NULL;

 fclose(f_files);
 return 0;
}


int main(int argc, char **argv) {
 int wa=0;
 char *tcs1;

 if(!(progname=strdup(argv[0]))) {
  outofmemory();
 }

 while(++wa<argc) {
  if(argv[wa][0]=='-') {
   tcs1=&argv[wa][0];
   while(*++tcs1) {
    switch(*tcs1) {
     case 'n':
      if(++wa==argc) {
       fprintf(stderr, "%s: no nick given\n", progname);
       exit(1);
      }
      if(!(nick=strdup(argv[wa]))) {
       outofmemory();
      }
      break;
     case 'i':
      if(++wa==argc) {
       fprintf(stderr, "%s: no ircname given\n", progname);
       exit(1);
      }
      if(!(ircname=strdup(argv[wa]))) {
       outofmemory();
      }
      break;
     case 'c':
      if(++wa==argc) {
       fprintf(stderr, "%s: no channel given\n", progname);
       exit(1);
      }
      if(!(channel=strdup(argv[wa]))) {
       outofmemory();
      }
      break;
     case 'd':
      debug++;
      break;
     case 'f':
      fake++;
      break;
     case 'h':
      fprintf(stderr, "%s: list of command line options\n"
       " -n nick     set nick\n"
       " -i ircname  set ircname\n"
       " -c channel  set channel\n"
       " -d          increase debug level\n"
       " -f          turn on 'fake' mode -- don't connect to irc server\n"
       " ircserver   set ircserver\n", progname);
       exit(1);
      break;
     default:
      fprintf(stderr, "%s: unknown command line option '%s'\n",
       progname, tcs1);
      exit(1);
    }
   }
  }
  else {
   if(ircserver) {
    fprintf(stderr, "%s: two servers specified?\n", progname);
    exit(1);
   }
   if(!(ircserver=strdup(argv[wa]))) {
    outofmemory();
   }
  }
 }

 if(!ircserver) {
  if(!(ircserver=strdup(DEFAULT_IRCSERVER))) {
   outofmemory();
  }
 }

 if(!nick) {
  if(!(nick=strdup(DEFAULT_NICK))) {
   outofmemory();
  }
 }

 if(!ircname) {
  if(!(ircname=strdup(DEFAULT_IRCNAME))) {
   outofmemory();
  }
 }

 if(!channel) {
  if(!(channel=strdup(DEFAULT_CHANNEL))) {
   outofmemory();
  }
 }

 if(readfilters(CONFFILE) < 0) {
  fprintf(stderr, "warning: could not open %s, no filters enabled\n", CONFFILE);
 }

 if(qbot_init(QF_NAME, "") < 0) {
  fprintf(stderr, "init failed:");
  while((tcs1=qbot_io_readline())) {
   puts(tcs1);
  }
  exit(1);
 }

 if(!(errlog=fopen("qbot.errors", "w"))) {
  fprintf(stderr, "warning: could not open error log\n");
 }

 signal(SIGCHLD, (void(*)())sigchld);
 signal(SIGPIPE, SIG_IGN);
 signal(SIGHUP, (void(*)())sighup);

 while(run_qbot() > 0);

 return 0;
}

