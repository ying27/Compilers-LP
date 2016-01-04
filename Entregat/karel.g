#header
<<
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
>>

<<
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
		//return domini.nsens == 0;
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
}
>>

#lexclass START
#token SPACE "[\ \n]" << zzskip();>>
#token NUM "[0-9]+"
#token WORLD "world"
#token ROBOT "robot"
#token LEFT "left"
#token RIGHT "right"
#token UP "up"
#token DOWN "down"
#token BEGIN "begin"
#token DEFINE "define"
#token OPAR "\{"
#token CPAR "\}"
#token MOVE "move"
#token TURNLEFT "turnleft"
#token IF "if"
#token ELSE "else"
#token AND "and"
#token OR "or"
#token NOT "not"
#token ENDINST ";"
#token ISCLEAR "isClear"
#token ANYBEEPERSINBAG "anyBeepersInBag"
#token FOUNDBEEPER "foundBeeper"
#token OCLAU "\["
#token CCLAU "\]"
#token COMA ","
#token WALLS "walls"
#token BEEPERS "beepers"
#token END "end"
#token ITERATE "iterate"
#token PUTBEEPER "putbeeper"
#token PICKBEEPER "pickbeeper"
#token TURNOFF "turnoff"


#token ID "[a-zA-Z][a-zA-Z0-9]*"
   karel: dworld drobot definitions BEGIN! linstr END! <<#0=createASTlist(_sibling);>>;

   dworld: WORLD^ NUM NUM;
   drobot: ROBOT^ NUM NUM NUM (UP|DOWN|LEFT|RIGHT);

   directions: LEFT|RIGHT|UP|DOWN;

   definitions: (deflist)* <<#0=createASTlist(_sibling);>>;
   deflist : dwalls|dbeepers|defines;

   dwalls: WALLS^ OCLAU! NUM NUM directions (COMA! NUM NUM directions)* CCLAU!;

   dbeepers: BEEPERS^ NUM NUM NUM;

   defines: DEFINE^ ID OPAR! linstr CPAR!;
   linstr: (atominst)* <<#0=createASTlist(_sibling);>>;
   atominst: (MOVE ENDINST!)|(TURNLEFT ENDINST!)|(PUTBEEPER ENDINST!)|(PICKBEEPER ENDINST!)|(si(sino|))|iterations|(ID ENDINST!)|(TURNOFF ENDINST!);
   si: IF^ boolean OPAR! linstr CPAR!;
   sino: ELSE^ OPAR! linstr CPAR!;
   boolean: batom ((AND^|OR^)batom)*;
   batom: (NOT^|) (ISCLEAR|ANYBEEPERSINBAG|FOUNDBEEPER);

   iterations: ITERATE^ NUM OPAR! linstr CPAR!;
