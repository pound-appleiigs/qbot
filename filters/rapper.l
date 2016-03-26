%e 2000
%p 5000
%n 1000
%k 500
%a 4000
%o 2000
BW [ 	]
EW [ 	.,;!?]

%{
	char buf[128];

%}

%%
neighborhood                    printf("hood");
female                          printf("ho");
Godzilla                        printf("Charles Barkley");
godzilla                        printf("Charles Barkley");
ah                              printf("i'ze");
fine                            printf("fly");
nice                            printf("fly");
my                              printf("mah");
crowd                           printf("crew");
clothes                         printf("threads");
booze                           printf("40");
breasts                         printf("titties");
tits                            printf("big-ass titties");
traveling                      printf("fuckin hoes all over da world and shit");
are                             printf("is");

real                            printf("real fuckin");
jawaid                          printf("hella skinny whitey");
Jawaid                          printf("hella skinny whitey");
gun                             printf("nine");
girlfriend                      printf("skanky bitch");
more                            printf("mo");
ask                             printf("ax");
trashing                        printf("dissin");
"making fun of"                 printf("dissin");
conservative                    printf("heartless racist bastard");
know                            printf("no whut time it is");
"with you"                      printf("witchu");
yeah                            printf("word");
cool                            printf("dope");
motherfucker                    printf("mo-fo");
bazyar                          printf("de Man");
Bazyar                          printf("de Man");
hey                             printf("yo");
Hey                             printf("Yo");
guys                            printf("niggaz");
America                         printf("AmeriKKKa");
america                         printf("ameriKKKa");
"Rush Limbaugh"                 printf("dat racist bastard");
"Ronald Reagan"                 printf("Satan");
"Reagan"                        printf("Satan");
welfare                         printf("salary");
white                           printf("racist oppressor");
male                            printf("bastard");
hanging                         printf("chillin'");
"hi there"                      printf("Yo, G!");
"how are you"                   printf("sup");
"how is"                        printf("sup wit");
"what's up"                     printf("whazzup");
file				printf("stash");
send				printf("t'row");
program				printf("honky code");
atlas				printf("Isaac");
unix				printf("slow mo-fo");
UNIX				printf("that slow mo-fo");
takes				printf("snatch'd");
Mexican				printf("wet-back");
mexican				printf("wet-back");
Italian				printf("greaser");
italian				printf("greaser");
take				printf(" snatch");
"don't"				printf(" duzn't");
jive				printf(" JIBE");
fool				printf(" honkyfool");
modem				printf(" doodad");
ing                             printf("in");
with                            printf(" wit'");
friends                         printf("posse");
group                           printf(" posse");
talk                            printf(" rap");
GNO                             printf("da plan");
th                              printf("d");
"e the "			printf("e da damn ");
"a the "			printf("a da damn ");
"t the "			printf("t da damn ");
"d the "			printf("d da damn ");
" man "                         printf(" brutha ");
woman				printf("ho");
girl				printf("goat");
something			printf("sumpn");
" lie "			printf(" honky jibe ");
-o-                     printf(" -on rebound- ");
-oo-			printf(" -check y'out latah-");
[a-b]"."		{ sprintf(buf, "%s Sheeeiit.",yytext); printf(buf); }
[e-f]"."		{ sprintf(buf, "%s 'n shit",yytext); printf(buf); }
[i-j]"."		{ sprintf(buf, "%s Ya' know?",yytext); printf(buf); }
[m-n]"."		{ sprintf(buf, "%s Ya damn whiteys!",yytext); printf(buf); }
[q-r]"."		{ sprintf(buf, "%s 'n shit",yytext); printf(buf); }
[u-v]"."		{ sprintf(buf, "%s Man!",yytext); printf(buf); }
[y-z]"."		{ sprintf(buf, "%s Yo, G, Whaddup!",yytext); printf(buf); }
Sure			printf("Sho' nuff");
sure			printf("sho' nuff");
\40+get				printf(" git");
"will have"			printf("gots'ta");
"will "				printf("gots'ta ");
"got to"			printf("gots'ta");
"I am"				printf("I's gots'ta be");
"am not"				printf("aint");
"is not"				printf("aint");
"are not"				printf("aint");
" are your"				printf(" is yo'");
" are you"				printf(" you is");
\40+hat\40+                     printf(" fedora ");
\40+shoe                    printf(" kicker");
haven't				printf("aint");
"have to"			printf("gotsta");
have				printf("gots");
" has"				printf(" gotsta");
"come over"			printf("mosey on down");
\40+come\40+                    printf(" mosey on down ");
!                       printf(".  Word, Niggaz! ");
buy				printf("steal");
\40+car\40+                     printf(" wheels ");
drive				printf("roll");
\40+eat\40+                     printf(" feed da bud ");
\40+black                   printf(" brutha ");
\40+negro                   printf(" brutha");
white 				printf("honky");
"person"			printf("rapper");
\40+thing                   printf(" wahtahmellun");
home					printf("plantation");
name				printf("dojigger");
\40+path                    printf(" alley");
computer			printf("clunker");
killed				printf("wasted");
president			printf("super-dude");
"prime minister"		printf("super honcho");
injured				printf("hosed");
government			printf("guvment");
knew				printf("knowed");
because				printf("a'cuz");
Because				printf("A'cuz");
your				printf("yo'");
Your				printf("Yo'");
four				printf("foe");
got				printf("gots");
aren't				printf("ain't");
young				printf("yung");
you				printf("ya");
You				printf("You's");
first				printf("fust");
police				printf("honky pigs");
\40+string                  printf(" chittlin");
\40+read		printf(" eyeball");
write				printf("scribble");
th				printf("d");
Th				printf("D");
\40+a\40+			printf(" some ");
tion				printf("shun");
\40+almost\40+			printf(" mos' ");
" from"			printf(" fum");
\40+because\40+		printf(" cuz' ");
you're		printf("youse");
You're		printf("Youse");
alright			printf("coo");
okay			printf("coo");
"er "			printf("a ");
known			printf("knode");
want			printf("wants'");
beat			printf("whup'");
exp			printf("sp");
exs			printf("s");
" exc"			printf(" s");
" ex"			printf(" es");
like			printf("likes");
did				printf("dun did");
"kind of"			printf("kinda");
women				printf("sistas");
" men "				printf(" bruthas ");
" mens "			printf(" bruthas ");
" man "				printf(" brutha ");
woman				printf("ho");
dead			printf("wasted");
good			printf("baad ass");
"open "				printf("jimmey ");
"opened "			printf("jimmey'd ");
" very"				printf(" real");
"per"				printf("puh'");
"pera"				printf("puh'");
"oar"				printf("o");
" can"				printf(" kin");
"just "				printf("plum ");
detroit			printf("Mo-town");
"western electric"		printf("da' cave");
" believe"			printf(" recon'");
" so "				printf(" so's ");
head				printf("'haid");
boss				printf("main man");
wife				printf("ho");
people				printf("bruthas an sistas");
money				printf("bre'd");
[a-z]":"		{	*(yytext+1) = ',';
				sprintf(buf, "%s dig dis:",yytext);
				printf(buf);
			}
amateur				printf("begina'");
radio					printf("beat box");
" of "				printf(" uh ");
what				printf("whut");
does				printf("duz");
was				printf("wuz");
" were"				printf(" wuz");
"understand it"			printf("dig it");
understand			printf("dig it");
" my"				printf(" mah");
" "[Ii]" "			printf(" ah ");
"meta"				printf("meta-fuckin");
"hair"			printf("strands");
"talk"			printf("rap");
"music"			printf("beat");
"basket"		printf("hoops");
"football"		printf("ball");
"friend"		printf("homey");
"school"		printf("farm");
"boss"			printf("Man");
"want to"		printf("wanna");
"wants to"		printf("be hankerin aftah");
"well"			printf("sheeit");
"Well"			printf("Sheeit");
"big"			printf("big-ass");
"bad"			printf("bad-ass");
"small"			printf("little-ass");
"sort of"		printf("radical");
" is "			printf(" be ");
water			printf("malt liquor");
book			printf("scribblin");
magazine		printf("issue of Playboy");
paper			printf("sheet");
break			printf("boogie");
Hi			printf("'Sup, dude");
VAX			printf("pink Cadillac");
.			printf(yytext);
\n			printf("\n");

%%

main()
{
	yylex();
}
