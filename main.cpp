#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

/*
{Sample Program
 reads 3 integers and tests operators
 reads 3 real numbers and tests operators
 reads 3 booleans and writes which is true and if they are all equal
 reads n and loops and prints all number from 0 to n
}
real x :=3.5;
bool t := true;
int r:= 1;
x:= 9.5;
{read x;}
int z;
z := 5;
int s;
s := r + z;
{int s := r + z;}
real tmp;
{x := z;}
tmp := 10.2 / 3.5;
int w := 1 + z + 3;
write x;
write s;
write t;
write tmp;
int ia; int ib; int ic;
read ia; read ib; read ic;
write ia+ib+ic; write ia-ib-ic; write ia*ib*ic; write ia/ib/ic;
real ra; real rb; real rc;
read ra; read rb; read rc;
write ra+rb+rc; write ra-rb-rc; write ra*rb*rc; write ra/rb/rc;
bool ba; bool bb; bool bc;
read ba; read bb; read bc;
if ba then write 1 end;
if bb then write 2 end;
if bc then write 3 end;
if ba = bb then
    if ba = bc then
        write 123
    end
end;
int n; int i; bool equal:= false;
i := 0;
read n;
repeat
    write i;
     equal:= true;
     write equal
until equal

*/

// sequence of statements separated by ;
// no procedures - no declarations
// all variables are integers
// variables are declared simply by assigning values to them :=
// if-statement: if (boolean) then [else] end
// repeat-statement: repeat until (boolean)
// boolean only in if and repeat conditions < = and two mathematical expressions
// math expressions integers only, + - * / ^
// I/O read write
// Comments {}

////////////////////////////////////////////////////////////////////////////////////
// Strings /////////////////////////////////////////////////////////////////////////

bool Equals(const char* a, const char* b)
{
    return strcmp(a, b)==0;
}

bool StartsWith(const char* a, const char* b)
{
    int nb=strlen(b);
    return strncmp(a, b, nb)==0;
}

void Copy(char* a, const char* b, int n=0)
{
    if(n>0) {strncpy(a, b, n); a[n]=0;}
    else strcpy(a, b);
}

void AllocateAndCopy(char** a, const char* b)
{
    if(b==0) {*a=0; return;}
    int n=strlen(b);
    *a=new char[n+1];
    strcpy(*a, b);
}

////////////////////////////////////////////////////////////////////////////////////
// Input and Output ////////////////////////////////////////////////////////////////

#define MAX_LINE_LENGTH 10000

struct InFile
{
    FILE* file;
    int cur_line_num;

    char line_buf[MAX_LINE_LENGTH];
    int cur_ind, cur_line_size;

    InFile(const char* str) {file=0; if(str) file=fopen(str, "r"); cur_line_size=0; cur_ind=0; cur_line_num=0;}
    ~InFile(){if(file) fclose(file);}

    void SkipSpaces()
    {
        while(cur_ind<cur_line_size)
        {
            char ch=line_buf[cur_ind];
            if(ch!=' ' && ch!='\t' && ch!='\r' && ch!='\n') break;
            cur_ind++;
        }
    }

    bool SkipUpto(const char* str)
    {
        while(true)
        {
            SkipSpaces();
            while(cur_ind>=cur_line_size) {if(!GetNewLine()) return false; SkipSpaces();}

            if(StartsWith(&line_buf[cur_ind], str))
            {
                cur_ind+=strlen(str);
                return true;
            }
            cur_ind++;
        }
        return false;
    }

    bool GetNewLine()
    {
        cur_ind=0; line_buf[0]=0;
        if(!fgets(line_buf, MAX_LINE_LENGTH, file)) return false;
        cur_line_size=strlen(line_buf);
        if(cur_line_size==0) return false; // End of file
        cur_line_num++;
        return true;
    }

    char* GetNextTokenStr()
    {
        SkipSpaces();
        while(cur_ind>=cur_line_size) {if(!GetNewLine()) return 0; SkipSpaces();}
        return &line_buf[cur_ind];
    }

    void Advance(int num)
    {
        cur_ind+=num;
    }
};

struct OutFile
{
    FILE* file;
    OutFile(const char* str) {file=0; if(str) file=fopen(str, "w");}
    ~OutFile(){if(file) fclose(file);}

    void Out(const char* s)
    {
        fprintf(file, "%s\n", s); fflush(file);
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Compiler Parameters /////////////////////////////////////////////////////////////

struct CompilerInfo
{
    InFile in_file;
    OutFile out_file;
    OutFile debug_file;

    CompilerInfo(const char* in_str, const char* out_str, const char* debug_str)
                : in_file(in_str), out_file(out_str), debug_file(debug_str)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Scanner /////////////////////////////////////////////////////////////////////////

#define MAX_TOKEN_LEN 40

// Add three new tokens INTNUM, REALNUM, BOOL,
enum TokenType{
                IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
                ASSIGN, EQUAL, LESS_THAN,
                PLUS, MINUS, TIMES, DIVIDE, POWER, ABST,
                SEMI_COLON,
                LEFT_PAREN, RIGHT_PAREN,
                LEFT_BRACE, RIGHT_BRACE,
                ID, NUM, INTNUM, REALNUM, BOOL,
                ENDFILE, ERROR
              };

// Used for debugging only /////////////////////////////////////////////////////////
const char* TokenTypeStr[]=
            {
                "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
                "Assign", "Equal", "LessThan",
                "Plus", "Minus", "Times", "Divide", "Power", "Abst",
                "SemiColon",
                "LeftParen", "RightParen",
                "LeftBrace", "RightBrace",
                "ID", "Num", "int", "real", "bool",
                "EndFile", "Error"
            };

struct Token
{
    TokenType type;
    char str[MAX_TOKEN_LEN+1];

    Token(){str[0]=0; type=ERROR;}
    Token(TokenType _type, const char* _str) {type=_type; Copy(str, _str);}
};

//add new tokens to reserved words int, real, bool, true, false
const Token reserved_words[]=
{
    Token(IF, "if"),
    Token(THEN, "then"),
    Token(ELSE, "else"),
    Token(END, "end"),
    Token(REPEAT, "repeat"),
    Token(UNTIL, "until"),
    Token(READ, "read"),
    Token(WRITE, "write"),
    Token(INTNUM, "int"),
    Token(REALNUM, "real"),
    Token(BOOL, "bool"),
    Token(BOOL, "true"),
    Token(BOOL, "false")

};
const int num_reserved_words=sizeof(reserved_words)/sizeof(reserved_words[0]);

// if there is tokens like < <=, sort them such that sub-tokens come last: <= <
// the closing comment should come immediately after opening comment
const Token symbolic_tokens[]=
{
    Token(ASSIGN, ":="),
    Token(EQUAL, "="),
    Token(LESS_THAN, "<"),
    Token(PLUS, "+"),
    Token(MINUS, "-"),
    Token(TIMES, "*"),
    Token(DIVIDE, "/"),
    Token(POWER, "^"),
    Token(ABST, "&"),
    Token(SEMI_COLON, ";"),
    Token(LEFT_PAREN, "("),
    Token(RIGHT_PAREN, ")"),
    Token(LEFT_BRACE, "{"),
    Token(RIGHT_BRACE, "}")
};
const int num_symbolic_tokens=sizeof(symbolic_tokens)/sizeof(symbolic_tokens[0]);

inline bool IsDigit(char ch){return (ch>='0' && ch<='9');}
// new function to detect dots
inline bool isDot(char ch){ return (ch == '.');}
inline bool IsLetter(char ch){return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'));}
inline bool IsLetterOrUnderscore(char ch){return (IsLetter(ch) || ch=='_');}

void GetNextToken(CompilerInfo* pci, Token* ptoken)
{
    ptoken->type=ERROR;
    ptoken->str[0]=0;

    int i;
    char* s=pci->in_file.GetNextTokenStr();
    if(!s)
    {
        ptoken->type=ENDFILE;
        ptoken->str[0]=0;
        return;
    }

    for(i=0;i<num_symbolic_tokens;i++)
    {
        if(StartsWith(s, symbolic_tokens[i].str))
            break;
    }

    if(i<num_symbolic_tokens)
    {
        if(symbolic_tokens[i].type==LEFT_BRACE)
        {
            pci->in_file.Advance(strlen(symbolic_tokens[i].str));
            if(!pci->in_file.SkipUpto(symbolic_tokens[i+1].str)) return;
            return GetNextToken(pci, ptoken);
        }
        ptoken->type=symbolic_tokens[i].type;
        Copy(ptoken->str, symbolic_tokens[i].str);
    }
    else if(IsDigit(s[0]))
    {   //to define the number weather it is a real or int
        bool isReal = false;
        int j=1;
        while(IsDigit(s[j]) || isDot(s[j]))
        {
            if(isDot(s[j]) && isReal)throw runtime_error("It's not real number");
            if(isDot(s[j]))isReal = true;
            j++;
        }
        if(isReal)ptoken->type=REALNUM;
        else ptoken->type=INTNUM;

        Copy(ptoken->str, s, j);
    }
    else if(IsLetterOrUnderscore(s[0]))
    {
        int j=1;
        bool isBool = false;
        while(IsLetterOrUnderscore(s[j]))
        {
            //to detect true and false in program
            if(s[j]=='t' && s[j+1]=='r' && s[j+2]=='u' && s[j+3]=='e')
            {
                ptoken->type=BOOL;
                isBool = true;
            }
            if(s[j]=='f' && s[j+1]=='a' && s[j+2]=='l' && s[j+3]=='s' &&  s[j+4]=='e')
            {
                ptoken->type=BOOL;
                isBool = true;
            }
            j++;
        }
        if(!isBool)ptoken->type=ID;

        Copy(ptoken->str, s, j);

        for(i=0;i<num_reserved_words;i++)
        {
            if(Equals(ptoken->str, reserved_words[i].str))
            {
                ptoken->type=reserved_words[i].type;
                break;
            }
        }
    }

    int len=strlen(ptoken->str);
    if(len>0) pci->in_file.Advance(len);
}

////////////////////////////////////////////////////////////////////////////////////
// Parser //////////////////////////////////////////////////////////////////////////

// program -> stmtseq
// stmtseq -> stmt { ; stmt }
// stmt -> ifstmt | repeatstmt | typestmt | assignstmt | readstmt | writestmt
// ifstmt -> if exp then stmtseq [ else stmtseq ] end
// repeatstmt -> repeat stmtseq until expr
// typestmt -> Int identifier | Real identifier | Boolean identifier{ ; typestmt }
// assignstmt -> identifier := expr
// readstmt -> read identifier
// writestmt -> write expr
// expr -> mathexpr [ (<|=) mathexpr ]
// mathexpr -> term { (+|-) term }    left associative
// term -> abs { & abs }            left associative
// abs -> factor { (*|/) factor }    left associative
// factor -> newexpr { ^ newexpr }    right associative
// newexpr -> ( mathexpr ) | number | identifier

enum NodeKind{
                IF_NODE, REPEAT_NODE, ASSIGN_NODE, READ_NODE, WRITE_NODE,
                OPER_NODE, NUM_NODE, ID_NODE
             };

// Used for debugging only /////////////////////////////////////////////////////////
const char* NodeKindStr[]=
            {
                "If", "Repeat", "Assign", "Read", "Write",
                "Oper", "Num", "ID"
            };

// Add new expresions INTEGER, BOOLEAN, REAL
enum ExprDataType {VOID, INTEGER, BOOLEAN, REAL};

// Used for debugging only /////////////////////////////////////////////////////////
const char* ExprDataTypeStr[]=
            {
                "Void", "Integer", "Boolean", "Real"
            };

#define MAX_CHILDREN 3

struct TreeNode
{
    TreeNode* child[MAX_CHILDREN];
    TreeNode* sibling; // used for sibling statements only

    // to define the type of real and int node when token is ID_NODE
    TokenType type;

    NodeKind node_kind;

    union{TokenType oper; int num; double num1; bool bl; char* id; }; // defined for expression/int/identifier only
    ExprDataType expr_data_type; // defined for expression/int/identifier/real/boolean only

    int line_num;

    TreeNode() {int i; for(i=0;i<MAX_CHILDREN;i++) child[i]=0; sibling=0; expr_data_type=VOID;}
};

struct ParseInfo
{
    Token next_token;
};

void Match(CompilerInfo* pci, ParseInfo* ppi, TokenType expected_token_type)
{
    pci->debug_file.Out("Start Match");

    if(ppi->next_token.type!=expected_token_type) throw 0;
    GetNextToken(pci, &ppi->next_token);

    fprintf(pci->debug_file.file, "[%d] %s (%s)\n", pci->in_file.cur_line_num, ppi->next_token.str, TokenTypeStr[ppi->next_token.type]); fflush(pci->debug_file.file);
}

TreeNode* Expr(CompilerInfo*, ParseInfo*);

// newexpr -> ( mathexpr ) | number | identifier
TreeNode* NewExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start NewExpr");

    // if the token type real add expr data type with real
    if(ppi->next_token.type==REALNUM)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=NUM_NODE;
        tree->expr_data_type = REAL;
        char* num_str=ppi->next_token.str;
        //add
        tree->num1=atof(num_str);
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }
    // if the token type int add expr data type with integer
    if(ppi->next_token.type==INTNUM)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=NUM_NODE;
        tree->expr_data_type = INTEGER;
        char* num_str=ppi->next_token.str;
        tree->num=0; while(*num_str) tree->num=tree->num*10+((*num_str++)-'0');
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }
    // same with bool
    if(ppi->next_token.type==BOOL)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=NUM_NODE;
        tree->expr_data_type = BOOLEAN ;
        char* num_str=ppi->next_token.str;
        if(!strcmp(num_str,"true"))tree->bl =true;
        if(!strcmp(num_str,"false"))tree->bl =false;
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if(ppi->next_token.type==ID)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=ID_NODE;
        AllocateAndCopy(&tree->id, ppi->next_token.str);
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if(ppi->next_token.type==LEFT_PAREN)
    {
        Match(pci, ppi, LEFT_PAREN);
        TreeNode* tree=Expr(pci, ppi);
        Match(pci, ppi, RIGHT_PAREN);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    throw 0;
    return 0;
}

// factor -> newexpr { ^ newexpr }    right associative
TreeNode* Factor(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Factor");

    TreeNode* tree=NewExpr(pci, ppi);

    if(ppi->next_token.type==POWER)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        pci->debug_file.Out("End Factor");
        return new_tree;
    }
    pci->debug_file.Out("End Factor");
    return tree;
}

// term -> factor { (*|/) factor }    left associative
TreeNode* Term(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Term");

    TreeNode* tree=Factor(pci, ppi);

    while(ppi->next_token.type==TIMES || ppi->next_token.type==DIVIDE)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End Term");
    return tree;
}

// term -> abs { & abs }
TreeNode* Absolute(CompilerInfo* pci, ParseInfo* ppi)
{
    //Write start absolute in the debug file
    pci->debug_file.Out("Start Absolute");
    //Make tree and call the term function
    TreeNode* tree=Term(pci, ppi);
    //while next token = ABST do the following
    while(ppi->next_token.type == ABST)
    {
        //Allocate new tree in mem
        TreeNode* new_tree=new TreeNode;
        //Make the node kind of the tree OPER_NODE
        new_tree->node_kind=OPER_NODE;
        //Make the oper of the tree the next_token.type
        new_tree->oper=ppi->next_token.type;
        //Make the line_num of the tree the cur_line_num
        new_tree->line_num=pci->in_file.cur_line_num;

        //Make the Zero child of the tree -> Term(pci, ppi) function
        new_tree->child[0]=tree;
        //Match till u find the corrosponding token
        Match(pci, ppi, ppi->next_token.type);
        //Make the first child of the tree -> Term(pci, ppi) function
        new_tree->child[1]=Term(pci, ppi);
        //Make the tree equal the new tree
        tree=new_tree;
    }
    //Write End Absolute in the debug file
    pci->debug_file.Out("End Absolute");
    return tree;
}

// mathexpr -> term { (+|-) term }    left associative
TreeNode* MathExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start MathExpr");

    TreeNode* tree=Absolute(pci, ppi);

    while(ppi->next_token.type==PLUS || ppi->next_token.type==MINUS)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Absolute(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End MathExpr");
    return tree;
}

// expr -> mathexpr [ (<|=) mathexpr ]
TreeNode* Expr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Expr");

    TreeNode* tree=MathExpr(pci, ppi);

    if(ppi->next_token.type==EQUAL || ppi->next_token.type==LESS_THAN)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=MathExpr(pci, ppi);

        pci->debug_file.Out("End Expr");
        return new_tree;
    }
    pci->debug_file.Out("End Expr");
    return tree;
}

// writestmt -> write expr
TreeNode* WriteStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start WriteStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=WRITE_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, WRITE);
    tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End WriteStmt");
    return tree;
}

// readstmt -> read identifier
TreeNode* ReadStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start ReadStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=READ_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, READ);
    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);

    pci->debug_file.Out("End ReadStmt");
    return tree;
}

// assignstmt -> identifier := expr
TreeNode* AssignStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start AssignStmt");


    TreeNode* tree=new TreeNode;

    tree->node_kind=ASSIGN_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);
    Match(pci, ppi, ASSIGN); tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End AssignStmt");
    return tree;
}

// Add real token for grammer and see if their assignment after the real token
TreeNode* Real(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Real Var");

    TreeNode* tree = new TreeNode();
    tree->expr_data_type = REAL;
    tree->type = REALNUM;
    tree->node_kind = ID_NODE;

    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, REALNUM);

    if (ppi->next_token.type == ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);

    // check for assignment when declartion
    if (ppi->next_token.type == ASSIGN)
    {
        TreeNode* assignNode = new TreeNode();
        assignNode->node_kind=ASSIGN_NODE;
        assignNode->expr_data_type = REAL;
        assignNode->line_num=pci->in_file.cur_line_num;
        AllocateAndCopy(&assignNode->id, tree->id);
        Match(pci, ppi, ASSIGN);
        assignNode->child[0]=Expr(pci, ppi);
        tree->child[0]=assignNode;
    }


    pci->debug_file.Out("End Real Var");
    return tree;
}
// Add Integer token for grammer and see if their assignment after the Integer token
TreeNode* Integer(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Integer Var");

    TreeNode* tree = new TreeNode();
    tree->expr_data_type = INTEGER;
    tree->node_kind = ID_NODE;
    tree->type = INTNUM;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, INTNUM);

    if (ppi->next_token.type == ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);

    // check for assignment when declartion
    if (ppi->next_token.type == ASSIGN)
    {
        TreeNode* assignNode = new TreeNode();
        assignNode->node_kind=ASSIGN_NODE;
        assignNode->expr_data_type = INTEGER;
        assignNode->line_num=pci->in_file.cur_line_num;
        AllocateAndCopy(&assignNode->id, tree->id);
        Match(pci, ppi, ASSIGN);
        assignNode->child[0]=Expr(pci, ppi);
        tree->child[0]=assignNode;
    }

    pci->debug_file.Out("End Integer Var");
    return tree;
}
// Add Bool token for grammer and see if there assignment after the Bool token
TreeNode* Bool(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Bool Var");

    TreeNode* tree = new TreeNode();
    tree->expr_data_type = BOOLEAN;
    tree->node_kind = ID_NODE;
    tree->type = BOOL;
    tree->bl =
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, BOOL);

    if (ppi->next_token.type == ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);

    // check for assignment when declartion
    if (ppi->next_token.type == ASSIGN)
    {
        TreeNode* assignNode = new TreeNode();
        assignNode->node_kind=ASSIGN_NODE;
        assignNode->expr_data_type = BOOLEAN;
        assignNode->line_num=pci->in_file.cur_line_num;
        AllocateAndCopy(&assignNode->id, tree->id);
        Match(pci, ppi, ASSIGN);
        assignNode->child[0]=Expr(pci, ppi);
        tree->child[0]=assignNode;
    }

    pci->debug_file.Out("End Bool Var");
    return tree;
}

TreeNode* StmtSeq(CompilerInfo*, ParseInfo*);

// repeatstmt -> repeat stmtseq until expr
TreeNode* RepeatStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start RepeatStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=REPEAT_NODE;
    tree->expr_data_type=BOOLEAN;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, REPEAT); tree->child[0]=StmtSeq(pci, ppi);
    Match(pci, ppi, UNTIL); tree->child[1]=Expr(pci, ppi);

    pci->debug_file.Out("End RepeatStmt");
    return tree;
}

// ifstmt -> if exp then stmtseq [ else stmtseq ] end
TreeNode* IfStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start IfStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=IF_NODE;
    tree->expr_data_type = BOOLEAN;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, IF); tree->child[0]=Expr(pci, ppi);
    Match(pci, ppi, THEN); tree->child[1]=StmtSeq(pci, ppi);
    if(ppi->next_token.type==ELSE) {Match(pci, ppi, ELSE); tree->child[2]=StmtSeq(pci, ppi);}
    Match(pci, ppi, END);

    pci->debug_file.Out("End IfStmt");
    return tree;
}

// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
TreeNode* Stmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Stmt");

    // Compare the next token with the First() of possible statements
    TreeNode* tree=0;


    if(ppi->next_token.type==IF) tree=IfStmt(pci, ppi);
    else if(ppi->next_token.type==REPEAT) tree=RepeatStmt(pci, ppi);
    else if(ppi->next_token.type==ID) tree=AssignStmt(pci, ppi);
    else if(ppi->next_token.type==READ) tree=ReadStmt(pci, ppi);
    else if(ppi->next_token.type==WRITE) tree=WriteStmt(pci, ppi);
    // call the new tokens in stmt
    else if(ppi->next_token.type==INTNUM) tree= Integer(pci, ppi);
    else if(ppi->next_token.type==BOOL) tree= Bool(pci, ppi);
    else if(ppi->next_token.type==REALNUM) tree= Real(pci, ppi);
    else throw 0;

    pci->debug_file.Out("End Stmt");
    return tree;
}

// stmtseq -> stmt { ; stmt }
TreeNode* StmtSeq(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start StmtSeq");

    TreeNode* first_tree=Stmt(pci, ppi);
    TreeNode* last_tree=first_tree;

    // If we did not reach one of the Follow() of StmtSeq(), we are not done yet
    while(ppi->next_token.type!=ENDFILE && ppi->next_token.type!=END &&
          ppi->next_token.type!=ELSE && ppi->next_token.type!=UNTIL)
    {
        Match(pci, ppi, SEMI_COLON);
        TreeNode* next_tree=Stmt(pci, ppi);
        last_tree->sibling=next_tree;
        last_tree=next_tree;
    }

    pci->debug_file.Out("End StmtSeq");
    return first_tree;
}

// program -> stmtseq
TreeNode* Parse(CompilerInfo* pci)
{
    ParseInfo parse_info;
    GetNextToken(pci, &parse_info.next_token);

    TreeNode* syntax_tree=StmtSeq(pci, &parse_info);

    if(parse_info.next_token.type!=ENDFILE)
        pci->debug_file.Out("Error code ends before file ends");

    return syntax_tree;
}

void PrintTree(TreeNode* node, int sh=0)
{
    int i, NSH=3;
    for(i=0;i<sh;i++) printf(" ");

    printf("[%s]", NodeKindStr[node->node_kind]);

    if(node->node_kind==OPER_NODE) printf("[%s]", TokenTypeStr[node->oper]);
    else if(node->node_kind==NUM_NODE && (node->expr_data_type==INTEGER)){
        printf("[%d]", node->num);
    }
    else if(node->node_kind==NUM_NODE && ((node->expr_data_type==BOOLEAN))){
        printf("[%d]", node->bl);
    }
    else if(node->node_kind==NUM_NODE && (node->expr_data_type==REAL)){
        printf("[%f]", node->num1);
    }
    else if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE) printf("[%s]", node->id);

    if((node->expr_data_type!=VOID))printf("[%s]", ExprDataTypeStr[node->expr_data_type]);

    printf("\n");

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) PrintTree(node->child[i], sh+NSH);
    if(node->sibling) PrintTree(node->sibling, sh);
}

void DestroyTree(TreeNode* node)
{
    int i;

    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
        if(node->id) delete[] node->id;

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) DestroyTree(node->child[i]);
    if(node->sibling) DestroyTree(node->sibling);

    delete node;
}

////////////////////////////////////////////////////////////////////////////////////
// Analyzer ////////////////////////////////////////////////////////////////////////

const int SYMBOL_HASH_SIZE=10007;

struct LineLocation
{
    int line_num;
    LineLocation* next;
};

struct VariableInfo
{
    char* name;
    int memloc;
    // add exprestions type for variables
    ExprDataType expr_data_type;

    LineLocation* head_line; // the head of linked list of source line locations
    LineLocation* tail_line; // the tail of linked list of source line locations
    VariableInfo* next_var; // the next variable in the linked list in the same hash bucket of the symbol table
};

struct SymbolTable
{
    int num_vars;
    VariableInfo* var_info[SYMBOL_HASH_SIZE];

    SymbolTable() {num_vars=0; int i; for(i=0;i<SYMBOL_HASH_SIZE;i++) var_info[i]=0;}

    int Hash(const char* name)
    {
        int i, len=strlen(name);
        int hash_val=11;
        for(i=0;i<len;i++) hash_val=(hash_val*17+(int)name[i])%SYMBOL_HASH_SIZE;
        return hash_val;
    }

    VariableInfo* Find(const char* name)
    {
        int h=Hash(name);
        VariableInfo* cur=var_info[h];
        while(cur)
        {
            if(Equals(name, cur->name)) return cur;
            cur=cur->next_var;
        }
        return 0;
    }

    void Insert(const char* name, int line_num, ExprDataType exprDataType)
    {
        LineLocation* lineloc=new LineLocation;
        lineloc->line_num=line_num;
        lineloc->next=0;

        int h=Hash(name);
        VariableInfo* prev=0;
        VariableInfo* cur=var_info[h];


        while(cur)
        {
            if(Equals(name, cur->name))
            {
                // just add this line location to the list of line locations of the existing var
                cur->tail_line->next=lineloc;
                cur->tail_line=lineloc;
                return;
            }
            prev=cur;
            cur=cur->next_var;
        }

        VariableInfo* vi=new VariableInfo;
        //add
        vi->expr_data_type = exprDataType;
        vi->head_line=vi->tail_line=lineloc;
        vi->next_var=0;
        vi->memloc=num_vars++;
        AllocateAndCopy(&vi->name, name);

        if(!prev) var_info[h]=vi;
        else prev->next_var=vi;
    }

    void Print()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                printf("[Var=%s][Mem=%d][Type=%s]", curv->name, curv->memloc, ExprDataTypeStr[curv->expr_data_type]);
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    printf("[Line=%d]", curl->line_num);
                    curl=curl->next;
                }
                printf("\n");
                curv=curv->next_var;
            }
        }
    }

    void Destroy()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    LineLocation* pl=curl;
                    curl=curl->next;
                    delete pl;
                }
                VariableInfo* p=curv;
                curv=curv->next_var;
                delete p;
            }
            var_info[i]=0;
        }
    }
};

void Analyze(TreeNode* node, SymbolTable* symbol_table)
{
    int i;

    // check if the var already exist if it is take the expr data type and dont insert in symbol table again other wise insert the var
    if (node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
    {
        VariableInfo* var = symbol_table->Find(node->id);
        if(var)
        {
            //This condition to see if its another deceleration here if it's not take the expr data type and continue
            if((node->type == INTNUM)||(node->oper == REALNUM)||((node->type == BOOL)))throw runtime_error("Dub Declerations");
            node->expr_data_type = var->expr_data_type;
        }
        else
        {
            //check if the node is read node and the var is not in the symbol table
            if(node->node_kind != READ_NODE)symbol_table->Insert(node->id, node->line_num, node->expr_data_type);
            else
            {
                throw runtime_error("ERROR Undefined Var\n");
            }
        }
    }

    //check if the assignment is in different types then throw error
    if (node->node_kind==ASSIGN_NODE || node->node_kind==READ_NODE)
    {
        if(node->node_kind==ASSIGN_NODE)
        {
            if(node->child[0]->node_kind==ID_NODE)
            {
                VariableInfo* var = symbol_table->Find(node->child[0]->id);
                if(ExprDataTypeStr[var->expr_data_type]!=ExprDataTypeStr[node->expr_data_type])
                {
                    throw runtime_error("Don't assign different types");
                }
                node->expr_data_type = var->expr_data_type;
            }

        }
        // search in SymbolTable
        VariableInfo* var = symbol_table->Find(node->id);
        if (var)
        {
            symbol_table->Insert(node->id, node->line_num, var->expr_data_type);
        }
        else
        {
            printf("ERROR Undefined Var\n");
        }
    }

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) Analyze(node->child[i], symbol_table);


    // handle epr data types for variables and check if the oper is in different types then throw error
    if(node->node_kind==OPER_NODE)
    {
        if(node->oper==EQUAL || node->oper==LESS_THAN) node->expr_data_type=BOOLEAN;
        if(node->oper==PLUS || node->oper==MINUS || node->oper == DIVIDE || node->oper == TIMES || node->oper == ABST )
        {
            if((node->child[0]->expr_data_type != node->child[1]->expr_data_type) ||(node->child[0]->expr_data_type == BOOLEAN)||(node->child[1]->expr_data_type == BOOLEAN))
            {
                throw runtime_error("No operation for different types");
            }

            node->expr_data_type = node->child[0]->expr_data_type;


        }

    }
    //check if the assignment is in different types then throw error
    if(node->node_kind == ASSIGN_NODE)
    {
            if(node->child[0])
            {
                if(node->child[0]->expr_data_type!=node->expr_data_type)
                {
                    throw runtime_error("Don't assign different types");
                }
            }
    }

    if(node->node_kind==IF_NODE && node->child[0]->expr_data_type!=BOOLEAN) printf("ERROR If test must be BOOLEAN\n");
    if(node->node_kind==REPEAT_NODE && node->child[1]->expr_data_type!=BOOLEAN) printf("ERROR Repeat test must be BOOLEAN\n");
    if(node->sibling) Analyze(node->sibling, symbol_table);
}

////////////////////////////////////////////////////////////////////////////////////
// Code Generator //////////////////////////////////////////////////////////////////


//make the next to function templates so it can hold the new types
template <typename T>
T Power(T a, int b)
{
    if(a==0) return 0;
    if(b==0) return 1;
    if(b>=1) return a*Power(a, b-1);
    return 0;
}

template <typename T>
T Evaluate(TreeNode* node, SymbolTable* symbol_table, double* variables)
{
    // add
    if((node->node_kind==NUM_NODE) && (node->expr_data_type==REAL)) return node->num1;
    if((node->node_kind==NUM_NODE) && (node->expr_data_type==INTEGER)) return node->num;
    if((node->node_kind==NUM_NODE) && (node->expr_data_type==BOOLEAN)) return node->bl;

    if(node->node_kind==ID_NODE) return variables[symbol_table->Find(node->id)->memloc];

    T a=Evaluate<T>(node->child[0], symbol_table, variables);
    T b=Evaluate<T>(node->child[1], symbol_table, variables);


    if(node->oper==EQUAL) return a==b;
    if(node->oper==LESS_THAN) return a<b;
    if(node->oper==PLUS) return a+b;
    if(node->oper==ABST) return abs(a-b);
    if(node->oper==MINUS) return a-b;
    if(node->oper==TIMES) return a*b;
    if(node->oper==DIVIDE) return a/b;
    if(node->oper==POWER) return Power(a,b);
    throw 0;
    return 0;
}

void RunProgram(TreeNode* node, SymbolTable* symbol_table, double* variables)
{
    //check the expr data type before evaluation
    if(node->node_kind==IF_NODE)
    {
        if(node->expr_data_type == REAL)
        {
            double cond=Evaluate<double>(node->child[0], symbol_table, variables);
            if(cond) RunProgram(node->child[1], symbol_table, variables);
            else if(node->child[2]) RunProgram(node->child[2], symbol_table, variables);
        }
        if(node->expr_data_type == INTEGER)
        {
            int cond=Evaluate<int>(node->child[0], symbol_table, variables);
            if(cond) RunProgram(node->child[1], symbol_table, variables);
            else if(node->child[2]) RunProgram(node->child[2], symbol_table, variables);
        }
        if(node->expr_data_type == BOOLEAN)
        {
            bool cond=Evaluate<bool>(node->child[0], symbol_table, variables);
            if(cond) RunProgram(node->child[1], symbol_table, variables);
            else if(node->child[2]) RunProgram(node->child[2], symbol_table, variables);
        }

    }
    if(node->node_kind==ASSIGN_NODE)
    {
        if(node->expr_data_type == REAL )
        {
            double v=Evaluate<double>(node->child[0], symbol_table, variables);
            variables[symbol_table->Find(node->id)->memloc]=v;
        }
        if(node->expr_data_type == INTEGER || node->expr_data_type == BOOLEAN)
        {
            int v=Evaluate<int>(node->child[0], symbol_table, variables);
            variables[symbol_table->Find(node->id)->memloc]=v;
        }
        if(node->expr_data_type == BOOLEAN)
        {
            bool v=Evaluate<bool>(node->child[0], symbol_table, variables);
            variables[symbol_table->Find(node->id)->memloc]=v;
        }

    }
    //To evaluate after declerations which means if there is child call evaluate with the child
    if(node->node_kind==ID_NODE)
    {
        if (node->child[0]) RunProgram(node->child[0], symbol_table, variables);
    }

    if(node->node_kind==READ_NODE)
    {
        printf("Enter %s: ", node->id);
        scanf("%lf", &variables[symbol_table->Find(node->id)->memloc]);

    }
    if(node->node_kind==WRITE_NODE)
    {
        if(node->child[0]->expr_data_type == REAL)
        {
            double v=Evaluate<double>(node->child[0], symbol_table, variables);
            printf("Val: %f\n", v);
        }
        if(node->child[0]->expr_data_type == INTEGER)
        {
            int v=Evaluate<int>(node->child[0], symbol_table, variables);
            printf("Val: %d\n", v);
        }
        if(node->child[0]->expr_data_type == BOOLEAN)
        {
            bool v=Evaluate<bool>(node->child[0], symbol_table, variables);
            printf("Val: %d\n", v);
        }

    }

    if(node->node_kind==REPEAT_NODE)
    {
        do
        {
            RunProgram(node->child[0], symbol_table, variables);
        }
        while(!Evaluate<double>(node->child[1], symbol_table, variables));
    }
    if(node->sibling) RunProgram(node->sibling, symbol_table, variables);
}

void RunProgram(TreeNode* syntax_tree, SymbolTable* symbol_table)
{
    int i;
    double* variables=new double[symbol_table->num_vars];
    for(i=0;i<symbol_table->num_vars;i++) variables[i]=0.0;
    RunProgram(syntax_tree, symbol_table, variables);
    delete[] variables;
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner and Compiler ////////////////////////////////////////////////////////////

void StartCompiler(CompilerInfo* pci)
{
    TreeNode* syntax_tree=Parse(pci);

    SymbolTable symbol_table;
    Analyze(syntax_tree, &symbol_table);

    printf("Symbol Table:\n");
    symbol_table.Print();
    printf("---------------------------------\n"); fflush(NULL);

    printf("Syntax Tree:\n");
    PrintTree(syntax_tree);
    printf("---------------------------------\n"); fflush(NULL);

    printf("Run Program:\n");
    RunProgram(syntax_tree, &symbol_table);
    printf("---------------------------------\n"); fflush(NULL);

    symbol_table.Destroy();
    DestroyTree(syntax_tree);
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner only ////////////////////////////////////////////////////////////////////

void StartScanner(CompilerInfo* pci)
{
    Token token;

    while(true)
    {
        GetNextToken(pci, &token);
        printf("[%d] %s (%s)\n", pci->in_file.cur_line_num, token.str, TokenTypeStr[token.type]); fflush(NULL);
        if(token.type==ENDFILE || token.type==ERROR) break;
    }
}

////////////////////////////////////////////////////////////////////////////////////

int main()
{
    printf("Start main()\n"); fflush(NULL);

    CompilerInfo compiler_info("input.txt", "output.txt", "debug.txt");

    StartCompiler(&compiler_info);

    printf("End main()\n"); fflush(NULL);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
