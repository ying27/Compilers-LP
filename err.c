/*
 * A n t l r  S e t s / E r r o r  F i l e  H e a d e r
 *
 * Generated from: karel.g
 *
 * Terence Parr, Russell Quong, Will Cohen, and Hank Dietz: 1989-2001
 * Parr Research Corporation
 * with Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <tuple>
using namespace std;

// struct to store information about tokens
typedef struct {
  string kind;
  string text;
} Attrib;

typedef std::tuple<int,int,int> i3tuple;

// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#define AST_FIELDS string kind; string text;
#include "ast.h"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr,int ttype, char *textt);
#define zzSET_SIZE 8
#include "antlr.h"
#include "ast.h"
#include "tokens.h"
#include "dlgdef.h"
#include "err.h"

ANTLRChar *zztokens[36]={
	/* 00 */	"Invalid",
	/* 01 */	"@",
	/* 02 */	"SPACE",
	/* 03 */	"NUM",
	/* 04 */	"WORLD",
	/* 05 */	"ROBOT",
	/* 06 */	"LEFT",
	/* 07 */	"RIGHT",
	/* 08 */	"UP",
	/* 09 */	"DOWN",
	/* 10 */	"BEGIN",
	/* 11 */	"DEFINE",
	/* 12 */	"OPAR",
	/* 13 */	"CPAR",
	/* 14 */	"MOVE",
	/* 15 */	"TURNLEFT",
	/* 16 */	"IF",
	/* 17 */	"ELSE",
	/* 18 */	"AND",
	/* 19 */	"OR",
	/* 20 */	"NOT",
	/* 21 */	"ENDINST",
	/* 22 */	"ISCLEAR",
	/* 23 */	"ANYBEEPERSINBAG",
	/* 24 */	"FOUNDBEEPER",
	/* 25 */	"OCLAU",
	/* 26 */	"CCLAU",
	/* 27 */	"COMA",
	/* 28 */	"WALLS",
	/* 29 */	"BEEPERS",
	/* 30 */	"END",
	/* 31 */	"ITERATE",
	/* 32 */	"PUTBEEPER",
	/* 33 */	"PICKBEEPER",
	/* 34 */	"TURNOFF",
	/* 35 */	"ID"
};
SetWordType zzerr1[8] = {0xc0,0x3,0x0,0x0, 0x0,0x0,0x0,0x0};
SetWordType zzerr2[8] = {0xc0,0x3,0x0,0x0, 0x0,0x0,0x0,0x0};
SetWordType zzerr3[8] = {0x0,0x8,0x0,0x30, 0x0,0x0,0x0,0x0};
SetWordType setwd1[36] = {0x0,0x1,0x0,0x0,0x0,0x2,0x0,
	0x0,0x0,0x0,0xe4,0xd4,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x8,0x8,0xd4,0xd4,0x0,
	0x0,0x0,0x0,0x0,0x0};
SetWordType zzerr4[8] = {0x0,0xe0,0x3,0xc0, 0xf,0x0,0x0,0x0};
SetWordType zzerr5[8] = {0x0,0xc0,0x1,0x80, 0xf,0x0,0x0,0x0};
SetWordType setwd2[36] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x3,0x3,0x0,0xf8,0xf4,
	0xf4,0xf4,0x40,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x3,0x3,0xf8,
	0xf4,0xf4,0xf4,0xf4,0xf4};
SetWordType zzerr6[8] = {0x0,0x0,0xc,0x0, 0x0,0x0,0x0,0x0};
SetWordType zzerr7[8] = {0x0,0x0,0xd0,0x1, 0x0,0x0,0x0,0x0};
SetWordType zzerr8[8] = {0x0,0x0,0xc0,0x1, 0x0,0x0,0x0,0x0};
SetWordType setwd3[36] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0xa,0x10,0x10,
	0x10,0x10,0x0,0x9,0x9,0x0,0x0,0x4,
	0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x10,
	0x10,0x10,0x10,0x10,0x10};
