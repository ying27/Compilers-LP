#ifndef tokens_h
#define tokens_h
/* tokens.h -- List of labelled tokens and stuff
 *
 * Generated from: karel.g
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * ANTLR Version 1.33MR33
 */
#define zzEOF_TOKEN 1
#define SPACE 2
#define NUM 3
#define WORLD 4
#define ROBOT 5
#define LEFT 6
#define RIGHT 7
#define UP 8
#define DOWN 9
#define BEGIN 10
#define DEFINE 11
#define OPAR 12
#define CPAR 13
#define MOVE 14
#define TURNLEFT 15
#define IF 16
#define ELSE 17
#define AND 18
#define OR 19
#define NOT 20
#define ENDINST 21
#define ISCLEAR 22
#define ANYBEEPERSINBAG 23
#define FOUNDBEEPER 24
#define OCLAU 25
#define CCLAU 26
#define COMA 27
#define WALLS 28
#define BEEPERS 29
#define END 30
#define ITERATE 31
#define PUTBEEPER 32
#define PICKBEEPER 33
#define TURNOFF 34
#define ID 35

#ifdef __USE_PROTOS
void karel(AST**_root);
#else
extern void karel();
#endif

#ifdef __USE_PROTOS
void dworld(AST**_root);
#else
extern void dworld();
#endif

#ifdef __USE_PROTOS
void drobot(AST**_root);
#else
extern void drobot();
#endif

#ifdef __USE_PROTOS
void directions(AST**_root);
#else
extern void directions();
#endif

#ifdef __USE_PROTOS
void definitions(AST**_root);
#else
extern void definitions();
#endif

#ifdef __USE_PROTOS
void deflist(AST**_root);
#else
extern void deflist();
#endif

#ifdef __USE_PROTOS
void dwalls(AST**_root);
#else
extern void dwalls();
#endif

#ifdef __USE_PROTOS
void dbeepers(AST**_root);
#else
extern void dbeepers();
#endif

#ifdef __USE_PROTOS
void defines(AST**_root);
#else
extern void defines();
#endif

#ifdef __USE_PROTOS
void linstr(AST**_root);
#else
extern void linstr();
#endif

#ifdef __USE_PROTOS
void atominst(AST**_root);
#else
extern void atominst();
#endif

#ifdef __USE_PROTOS
void si(AST**_root);
#else
extern void si();
#endif

#ifdef __USE_PROTOS
void sino(AST**_root);
#else
extern void sino();
#endif

#ifdef __USE_PROTOS
void boolean(AST**_root);
#else
extern void boolean();
#endif

#ifdef __USE_PROTOS
void batom(AST**_root);
#else
extern void batom();
#endif

#ifdef __USE_PROTOS
void iterations(AST**_root);
#else
extern void iterations();
#endif

#endif
extern SetWordType zzerr1[];
extern SetWordType zzerr2[];
extern SetWordType zzerr3[];
extern SetWordType setwd1[];
extern SetWordType zzerr4[];
extern SetWordType zzerr5[];
extern SetWordType setwd2[];
extern SetWordType zzerr6[];
extern SetWordType zzerr7[];
extern SetWordType zzerr8[];
extern SetWordType setwd3[];
