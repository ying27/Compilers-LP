/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   antlr -gt karel.g
 *
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
#define GENAST

#include "ast.h"

#define zzSET_SIZE 8
#include "antlr.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

#include "ast.c"
zzASTgvars

ANTLR_INFO

#include <cstdlib>
#include <cmath>

//global structures: AST, and domini
AST *root;

map<string, AST *> mdefines;

enum torient {DLEFT,DRIGHT,DUP,DDOWN};
//enum torient {DLEFT,DDOWN,DRIGHT,DDUP};
class Casilla {
  private:
  int beepers = 0;
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
  
public:
  
	Casilla(){ 
    left = false;
    right = false;
    up = false;
    down = false;
    beepers = 0;
  }
  
	void setBeepers(int k) {
    beepers = k;
  }
  
    void addBeeper(){
    beepers++;
  }
  
    void subsBeeper(){
    beepers--;
  }
  
	void setWall(string w) {
    if(w == "up") up = true;
    else if (w == "down") down = true;
    else if (w == "left") left = true;
    else if (w == "right") right = true;
    else cout << "direction not found" << endl;
  }
  
    int getBeepers(){
    return beepers;
  }
  
    bool hasWall_s(string w) {
    if(w == "up") return up;
    else if (w == "down") return down;
    else if (w == "left") return left;
    else if (w == "right") return right;
  }
  
    bool hasWall_i(int w) {
    if(w == DUP) return up;
    else if (w == DDOWN) return down;
    else if (w == DLEFT) return left;
    else if (w == DRIGHT) return right;
  }
};

struct {
  int gx,gy;                                    // dimensions del mon
  int posx,posy;                                 // posicio actual robot
  torient orient;                               // orientacio actual robot
  int nsens;                                    // nombre de sensors que te el robot
  vector< vector<Casilla> > mapa;
}domini;

// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
  if (type == ID) {
    attr->kind = "id";
    attr->text = text;
  }
  
	else {
    attr->kind = text;
    attr->text = "";
  }
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
  AST* as = new AST;
  as->kind = attr->kind; 
  as->text = attr->text;
  as->right = NULL; 
  as->down = NULL;
  return as;
}


/// create a new "list" AST node with one element
AST* createASTlist(AST *child) {
  AST *as=new AST;
  as->kind="list";
  as->right=NULL;
  as->down=child;
  return as;
}

/*
AST *findDefinition(string id) {
  AST *n = root->down->right->right->down;
  while (n != NULL and (n->down->text != id)) n = n->right;
  if (n == NULL) {cout << "NOT FOUND: " << id << " " << endl;}
  return n->down->right;
}*/

AST *findDefinition(string id) {
  return mdefines[id];
}	

/// get nth child of a tree. Count starts at 0.
/// if no such child, returns NULL
AST* child(AST *a,int n) {
  AST *c=a->down;
  for (int i=0; c!=NULL && i<n; i++) c=c->right;
  return c;
}


/// print AST, recursively, with indentation
void ASTPrintIndent(AST *a,string s)
{
  if (a==NULL) return;
  
	cout<<a->kind;
  if (a->text!="") cout<<"("<<a->text<<")";
  cout<<endl;
  
	AST *i = a->down;
  while (i!=NULL && i->right!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"  |"+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }
  
	if (i!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"   "+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }
}

/// print AST 
void ASTPrint(AST *a)
{
  while (a!=NULL) {
    cout<<" ";
    ASTPrintIndent(a,"");
    a=a->right;
  }
}

bool anyBeepersInBag(){
  return domini.nsens > 0;
}

bool foundBeeper(){
  return domini.mapa[domini.posx][domini.posy].getBeepers() > 0;
}

void pickbeeper(){
  domini.mapa[domini.posx][domini.posy].subsBeeper();
  domini.nsens++;
}

void putbeeper(){
  domini.mapa[domini.posx][domini.posy].addBeeper();
  domini.nsens--;
}

bool dinsDominis(int x, int y) {
  return (x >= 0 and y >= 0) and (x < domini.gx and y < domini.gy);
}

bool isClear(int x, int y, int orient) {
  int next_x = x;
  int next_y = y;
  int next_orient;
  
    if (orient == DUP) {
    next_x--;
    next_orient = DDOWN;
  } 
  
    else if (orient == DDOWN) {
    next_x++;
    next_orient = DUP;
  } 
  
    else if (orient == DLEFT) {
    next_y--;
    next_orient = DRIGHT;
  }
  
    else if (orient == DRIGHT) {
    next_y++;
    next_orient = DLEFT;
  }
  
    if (dinsDominis(next_x, next_y)) {
    return  not domini.mapa[x][y].hasWall_i(orient) and not domini.mapa[next_x][next_y].hasWall_i(next_orient);
  }
  else return false;
}

void move(){
  if (isClear(domini.posx, domini.posy, domini.orient)){
    if (domini.orient == DUP) {
      domini.posx--;
    } 
    
        else if (domini.orient == DDOWN) {
      domini.posx++;
    } 
    
        else if (domini.orient == DLEFT) {
      domini.posy--;
    }
    
        else if (domini.orient == DRIGHT) {
      domini.posy++;
    }
  }
}

bool avaluaCondicio(AST *a) {
  if (a->kind == "not") return not(avaluaCondicio(child(a,0)));
  else if (a->kind == "and") return avaluaCondicio(child(a,0)) and avaluaCondicio(child(a,1));
  else if (a->kind == "or") return avaluaCondicio(child(a,0)) or avaluaCondicio(child(a,1));
  
    else if (a->kind == "isClear") return isClear(domini.posx, domini.posy, domini.orient);
  else if (a->kind == "anyBeepersInBag") return anyBeepersInBag();
  else if (a->kind == "foundBeeper") return foundBeeper();
}

torient getDirectionFromString(string dir){
  if (dir == "up") return DUP;
  else if (dir == "down") return DDOWN;
  else if (dir == "left") return DLEFT;
  else return DRIGHT;
}

void execute(AST *a){
  if (a == NULL) return ;
  
    else if (a->kind == "iterate") {
    int n = atoi(a->down->kind.c_str());
    for (int j = 0; j < n; j++) {
      execute(a->down->right->down);
    }
  }
  
    else if (a->kind == "if") {
    if (avaluaCondicio(a->down)) execute(a->down->right->down);
    else if (a->right != NULL and a->right->kind == "else") {
      execute(a->right->down->down);
    }
  }
  
    else if (a->kind == "move") {
    move();
  }
  
    else if (a->kind == "id") {
    execute(findDefinition(a->text.c_str())->down);
  }
  
    else if (a->kind == "turnleft"){
    if (domini.orient == DUP) domini.orient = DLEFT;
    else if (domini.orient == DLEFT) domini.orient = DDOWN;
    else if (domini.orient == DDOWN) domini.orient = DRIGHT;
    else domini.orient = DUP;
  }
  
    else if (a->kind == "pickbeeper") pickbeeper();
  
    else if (a->kind == "putbeeper") putbeeper();
  
    else if (a->kind == "turnoff") cout << "powering off" << endl;
  
    execute(a->right);
}

void printKind(AST *a){
  cout << a->kind.c_str() << endl;
}

void setWallsRec(AST *a){
  if (a == NULL) return ;
  else {
    int x = atoi(a->kind.c_str());
    int y = atoi(a->right->kind.c_str());
    domini.mapa[x][y].setWall(a->right->right->kind.c_str());
  } 
  setWallsRec(a->right->right->right);
}

void setDefinitions(AST *a){
  
    if (a == NULL) return ;
  
    else if (a->kind == "walls") {
    setWallsRec(a->down);
  }
  
    else if (a->kind == "beepers") {
    int beepers = atoi(a->down->right->right->kind.c_str());
    int x = atoi(a->down->kind.c_str());
    int y = atoi(a->down->right->kind.c_str());
    
        domini.mapa[x][y].setBeepers(beepers);
  }
  
  	else if (a->kind == "define") {
    mdefines[a->down->text.c_str()] = a->down->right;
    //cout << "define: " << a->down->text.c_str() << endl;
  }
  setDefinitions(a->right);
}


void omplirDomini() {
  //world
  domini.gx = atoi(root->down->down->kind.c_str());
  domini.gy = atoi(root->down->down->right->kind.c_str());
  domini.mapa = vector<vector<Casilla> > (domini.gx, vector<Casilla> (domini.gy, Casilla()));
  
  	//robot
  domini.posx = atoi(root->down->right->down->kind.c_str());
  domini.posy = atoi(root->down->right->down->right->kind.c_str()); 
  domini.nsens = atoi(root->down->right->down->right->right->kind.c_str());
  domini.orient = getDirectionFromString(root->down->right->down->right->right->right->kind.c_str());
}

void novaPosicio () {
  omplirDomini();
  //definitions
  setDefinitions(root->down->right->right->down);
  //instructions
  execute(root->down->right->right->right->down); 
  cout << "Nova posicio: " << domini.posx << " " << domini.posy << endl;
  cout << "Orientation: " << domini.orient << endl;
  cout << "Num beepers: " << domini.nsens << endl;
}


int main() {
  root = NULL;
  ANTLR(karel(&root), stdin);
  ASTPrint(root);
  novaPosicio();
  cout << DLEFT << endl;
}

void
#ifdef __USE_PROTOS
karel(AST**_root)
#else
karel(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  dworld(zzSTR); zzlink(_root, &_sibling, &_tail);
  drobot(zzSTR); zzlink(_root, &_sibling, &_tail);
  definitions(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(BEGIN);  zzCONSUME;
  linstr(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(END); 
  (*_root)=createASTlist(_sibling);
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x1);
  }
}

void
#ifdef __USE_PROTOS
dworld(AST**_root)
#else
dworld(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(WORLD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x2);
  }
}

void
#ifdef __USE_PROTOS
drobot(AST**_root)
#else
drobot(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(ROBOT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==UP) ) {
      zzmatch(UP); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==DOWN) ) {
        zzmatch(DOWN); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==LEFT) ) {
          zzmatch(LEFT); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==RIGHT) ) {
            zzmatch(RIGHT); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr1,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x4);
  }
}

void
#ifdef __USE_PROTOS
directions(AST**_root)
#else
directions(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==LEFT) ) {
    zzmatch(LEFT); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  }
  else {
    if ( (LA(1)==RIGHT) ) {
      zzmatch(RIGHT); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==UP) ) {
        zzmatch(UP); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==DOWN) ) {
          zzmatch(DOWN); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {zzFAIL(1,zzerr2,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x8);
  }
}

void
#ifdef __USE_PROTOS
definitions(AST**_root)
#else
definitions(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x10) ) {
      deflist(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x20);
  }
}

void
#ifdef __USE_PROTOS
deflist(AST**_root)
#else
deflist(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==WALLS) ) {
    dwalls(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (LA(1)==BEEPERS) ) {
      dbeepers(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==DEFINE) ) {
        defines(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {zzFAIL(1,zzerr3,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x40);
  }
}

void
#ifdef __USE_PROTOS
dwalls(AST**_root)
#else
dwalls(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(WALLS); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OCLAU);  zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  directions(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==COMA) ) {
      zzmatch(COMA);  zzCONSUME;
      zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      directions(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzmatch(CCLAU);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x80);
  }
}

void
#ifdef __USE_PROTOS
dbeepers(AST**_root)
#else
dbeepers(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(BEEPERS); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x1);
  }
}

void
#ifdef __USE_PROTOS
defines(AST**_root)
#else
defines(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(DEFINE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OPAR);  zzCONSUME;
  linstr(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(CPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x2);
  }
}

void
#ifdef __USE_PROTOS
linstr(AST**_root)
#else
linstr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd2[LA(1)]&0x4) ) {
      atominst(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x8);
  }
}

void
#ifdef __USE_PROTOS
atominst(AST**_root)
#else
atominst(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==MOVE) ) {
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      zzmatch(MOVE); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(ENDINST);  zzCONSUME;
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==TURNLEFT) ) {
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        zzmatch(TURNLEFT); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(ENDINST);  zzCONSUME;
        zzEXIT(zztasp2);
        }
      }
    }
    else {
      if ( (LA(1)==PUTBEEPER) ) {
        {
          zzBLOCK(zztasp2);
          zzMake0;
          {
          zzmatch(PUTBEEPER); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
          zzmatch(ENDINST);  zzCONSUME;
          zzEXIT(zztasp2);
          }
        }
      }
      else {
        if ( (LA(1)==PICKBEEPER) ) {
          {
            zzBLOCK(zztasp2);
            zzMake0;
            {
            zzmatch(PICKBEEPER); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
            zzmatch(ENDINST);  zzCONSUME;
            zzEXIT(zztasp2);
            }
          }
        }
        else {
          if ( (LA(1)==IF) ) {
            {
              zzBLOCK(zztasp2);
              zzMake0;
              {
              si(zzSTR); zzlink(_root, &_sibling, &_tail);
              {
                zzBLOCK(zztasp3);
                zzMake0;
                {
                if ( (LA(1)==ELSE) ) {
                  sino(zzSTR); zzlink(_root, &_sibling, &_tail);
                }
                else {
                  if ( (setwd2[LA(1)]&0x10) ) {
                  }
                  else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
                }
                zzEXIT(zztasp3);
                }
              }
              zzEXIT(zztasp2);
              }
            }
          }
          else {
            if ( (LA(1)==ITERATE) ) {
              iterations(zzSTR); zzlink(_root, &_sibling, &_tail);
            }
            else {
              if ( (LA(1)==ID) ) {
                {
                  zzBLOCK(zztasp2);
                  zzMake0;
                  {
                  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
                  zzmatch(ENDINST);  zzCONSUME;
                  zzEXIT(zztasp2);
                  }
                }
              }
              else {
                if ( (LA(1)==TURNOFF) ) {
                  {
                    zzBLOCK(zztasp2);
                    zzMake0;
                    {
                    zzmatch(TURNOFF); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
                    zzmatch(ENDINST);  zzCONSUME;
                    zzEXIT(zztasp2);
                    }
                  }
                }
                else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
              }
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x20);
  }
}

void
#ifdef __USE_PROTOS
si(AST**_root)
#else
si(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(IF); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  boolean(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(OPAR);  zzCONSUME;
  linstr(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(CPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x40);
  }
}

void
#ifdef __USE_PROTOS
sino(AST**_root)
#else
sino(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(ELSE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OPAR);  zzCONSUME;
  linstr(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(CPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x80);
  }
}

void
#ifdef __USE_PROTOS
boolean(AST**_root)
#else
boolean(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  batom(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd3[LA(1)]&0x1) ) {
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        if ( (LA(1)==AND) ) {
          zzmatch(AND); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==OR) ) {
            zzmatch(OR); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp3);
        }
      }
      batom(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x2);
  }
}

void
#ifdef __USE_PROTOS
batom(AST**_root)
#else
batom(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==NOT) ) {
      zzmatch(NOT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (setwd3[LA(1)]&0x4) ) {
      }
      else {zzFAIL(1,zzerr7,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==ISCLEAR) ) {
      zzmatch(ISCLEAR); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==ANYBEEPERSINBAG) ) {
        zzmatch(ANYBEEPERSINBAG); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==FOUNDBEEPER) ) {
          zzmatch(FOUNDBEEPER); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {zzFAIL(1,zzerr8,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x8);
  }
}

void
#ifdef __USE_PROTOS
iterations(AST**_root)
#else
iterations(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(ITERATE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OPAR);  zzCONSUME;
  linstr(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(CPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x10);
  }
}
