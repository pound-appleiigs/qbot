%{
 /* 
  * BIFF.X by Matt Welsh (mdw@tc.cornell.edu)
  *
  * $Id: biff.x,v 1.1 92/11/03 18:31:10 mdw Exp Locker: mdw $
  * $What: <@(#) biff.x,v	1.13> $
  *
  * Simulates our favorite guy, the B1FFSTER!!!!1!!!
  *
  * To compile, do the following:
  *
  *   lex biff.x
  *   cc -o Biff lex.yy.c -ll
  *
  * Yes, this is very poorly written, and yes, it was a late-night hack.
  * But it seems to work, mail me if it doesn't compile, or if you find 
  * any bugs or make any additions.
  * Happy BIFFing,
  * 
  * mdw
  * 
  */
%}

WC		[A-Za-z']
NW		[^A-Za-z']

%start		INW NIW 
%o 15000

%%

\\[^ \n]+	ECHO;

{NW}		{ BEGIN NIW; ECHO; }
"..."           { BEGIN NIW; printf((rand()%2) ? "...C00L HUH!?! " : "...!! "); }
"."/{NW}	{ BEGIN NIW; printf((rand()%2) ? "." : "!!" ); }
".\""           { BEGIN NIW; printf((rand()%2) ? "!!!\"" : "!1!!\"" ); }
"!"$            { BEGIN NIW; printf("!!!!!!!!!!1"); }
"!"+/{NW}       { BEGIN NIW; printf("!1!"); }
"?"/{NW}        { BEGIN NIW; printf("??!!"); }
"?"             { BEGIN NIW; printf("?!");  }
":)"            { BEGIN NIW; printf(";-)!!! "); }
":-)"           { BEGIN NIW; printf(";-)!!!!  "); }
"'"             { BEGIN NIW; printf("\""); }

"ove"           { BEGIN INW; printf("UV "); }
"ea"            { BEGIN INW; printf("EE"); }
"ies"           { BEGIN INW; printf("YS"); }
"please"        { BEGIN INW; printf("PLEEZ!"); }
"Please"        { BEGIN INW; printf("PLEEZ"); }
"Thanks"        { BEGIN INW; printf("THANX!!"); }
"thanks"        { BEGIN INW; printf("THANX"); }
"enough"        { BEGIN INW; printf("ENUF"); }
"Enough"        { BEGIN INW; printf("ENUFF"); }
"does"          { BEGIN INW; printf("DUZ"); }
"Does"          { BEGIN INW; printf("DUZ"); }
"fuck"/{NW}     { BEGIN INW; printf("FUCK !!!!!1 "); }
"Fuck"/{NW}     { BEGIN INW; printf("FUCK !!!!!1 "); }
"damn"          { BEGIN INW; printf("FUCK!! "); }
<NIW>"hell"/{NW}          { BEGIN INW; printf("FUCK!! "); }
"Damn"          { BEGIN INW; printf("FUCK!! "); }
"Shit"          { BEGIN INW; printf("FUCK!! "); }
"shit"          { BEGIN INW; printf("FUCK!! "); }
<NIW>"Hell"/{NW}          { BEGIN INW; printf("FUCK!! "); }
<NIW>"dick"/{NW}          { BEGIN INW; printf("FUCK!! "); }
<NIW>"Dick"/{NW}          { BEGIN INW; printf("FUCK!! "); }
<NIW>"mad"/{NW} { BEGIN INW; printf("PISSED 0FF!!!1! "); }
<NIW>"Mad"/{NW} { BEGIN INW; printf("PISSED 0FF!!!1! "); }
"ar"/{NW}       { BEGIN NIW; printf("RE"); }
"ain"/{NW}      { BEGIN NIW; printf("ANE"); }
"to"/{NW}       { BEGIN NIW; printf("2"); }
"too"/{NW}      { BEGIN NIW; printf("2"); }
"two"/{NW}      { BEGIN NIW; printf("TO "); }
"ic"/{NW}       { BEGIN NIW; printf("IK"); }
"le"/{NW}       { BEGIN NIW; printf("EL"); }
"by"/{NW}       { BEGIN NIW; printf("BUY"); }
"buy"/{NW}      { BEGIN NIW; printf("BY"); }
"bye"/{NW}      { BEGIN NIW; printf("BY"); }
"you"           { BEGIN INW; printf("U"); }
"cause"         { BEGIN INW; printf("CUZ"); }
<INW>"or"       { BEGIN INW; printf((rand()%2) ? "OR" : "ER"); }
"and"           { BEGIN INW; printf("&"); }
"biff"          { BEGIN INW; printf("B1FFSTER!!!!!"); }
"BIFF"          { BEGIN INW; printf("B1FFSTERE!!!1!"); }
"Biff"          { BEGIN INW; printf("B1FFSTERE!!!1!"); }
"tion"          { BEGIN INW; printf("SHUN"); }
"are"           { BEGIN INW; printf("R"); }
"cool"          { BEGIN INW; printf("C00L!!!"); }
"computer"      { BEGIN INW; printf("C-64"); }
"software"      { BEGIN INW; printf("WAREZ!1!!"); }
"program"       { BEGIN INW; printf("WAREZ!1!!"); }
"Mr"            { BEGIN INW; printf("D00D"); }
"man"/{NW}      { BEGIN INW; printf("D00D"); }
"Man" 		{ BEGIN INW; printf("D00D"); }
"some"          { BEGIN INW; printf("SUM"); }

"a"             { BEGIN INW; printf("A"); }
"b"             { BEGIN INW; printf("B"); }
"c"             { BEGIN INW; printf("C"); }
"d"             { BEGIN INW; printf("D"); }
"e"             { BEGIN INW; printf("E"); }
"f"             { BEGIN INW; printf("F"); }
"g"             { BEGIN INW; printf("G"); }
"h"             { BEGIN INW; printf("H"); }
"i"             { BEGIN INW; printf((rand()%2) ? "I" : "1"); }
"I"             { BEGIN INW; printf((rand()%2) ? "I" : "1"); }
"j"             { BEGIN INW; printf("J"); }
"k"             { BEGIN INW; printf("K"); }
"l"             { BEGIN INW; printf("L"); }
"m"             { BEGIN INW; printf("M"); }
"n"             { BEGIN INW; printf("N"); }
"o"             { BEGIN INW; printf("0"); }
"O" 		{ BEGIN INW; printf("0"); }
"ph"            { BEGIN INW; printf("F"); }
"Ph"            { BEGIN INW; printf("F"); }
"p"             { BEGIN INW; printf("P"); }
"q"             { BEGIN INW; printf("Q"); }
"r"             { BEGIN INW; printf("R"); }
"s"             { BEGIN INW; printf((rand()%2) ? "5" : "S"); }
"S"             { BEGIN INW; printf((rand()%2) ? "5" : "S"); }
"t"             { BEGIN INW; printf("T"); }
"u"             { BEGIN INW; printf("U"); }
"v"             { BEGIN INW; printf("V"); }
"w"             { BEGIN INW; printf("W"); }
"x"             { BEGIN INW; printf("X"); }
"y"             { BEGIN INW; printf("Y"); }
"z"             { BEGIN INW; printf("Z"); }

.		{ BEGIN INW; ECHO; }

%%
main()
{
  yylex();
  return(0);
}
