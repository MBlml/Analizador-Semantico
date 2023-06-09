//  GrammarAnalyzer.h

#include "Defines.h"
#include "Node.h"

#ifndef GrammarAnalyzer_h
#define GrammarAnalyzer_h

/*
PROGRAM -> HEADER MAIN
MAIN -> int main() Block
Block -> { Stmts }
Stmts -> Stmt Stmts | Empty
Stmt -> Asig_E ; | Asig_C ; | Asig_S ; |
        Decl ; | break ; | continue; |
        DO_WHILE ; | IF | WHILE | DO_WHILE ; |
        RETURN ; | SWITCH | FunCall ;

HEADER -> H_Stmts
H_Stmts -> H_Stmt H_Stmts | Empty
H_Stmt -> FunDef | Decl ;

IF -> if ( Expr ) Block |
      if ( Expr ) Block ELSE |
      if ( Expr ) Block ELSE_IF ELSE
ELSE_IF -> else if ( Expr ) Block | else if ( Expr ) Block ELSE_IF
ELSE -> else Block

WHILE -> while ( Expr ) Block
DO_WHILE -> do Block while ( Expr )

SWITCH -> switch ( Expr ) { CASES DEFAULT }
CASES -> CASE | CASE CASES
CASE -> case Const : Stmts
DEFAULT -> default : Stmts
Const -> Number | true | false

RETURN -> return Expr

Asig_C -> Id Asig_C_Op Expr
Asig_C_Op -> += | -= | /= | *= |
             ^= | &= | |= | %= |
             ~= | !=
Asig_S -> Id ++ | Id --
Asig_E -> Id = Expr

FunDef -> Type FunName ( ParasDef ) Block
ParasDef -> ParaDef | ParaDef , ParasDef | Empty
ParaDef -> Type Id

FunCall -> FunName ( Paras )
Paras -> Para | Para , Paras | Empty
Para -> Expr

Decl -> Type Descs
Descs -> Desc | Desc , Descs
Desc -> Id | Asig_E
Type -> bool | char | int | double | float | string

Expr -> Factor | Expr BinOp Expr
Factor -> Number | ( Expr ) | Id | FunCall | SingOp Factor
BinOp -> || ｜ && ｜ | ｜ ^ ｜ & ｜ != ｜
         == ｜ > ｜ >= ｜ <= ｜ < ｜ << ｜
         >> ｜ + ｜ - ｜ * ｜ / ｜ %
SingOp -> ! | ~ | - | ++ | --
Number -> Decimal_Number | Octal_Number | Hexademical_Number
*/

class GrammarAnalyzer {
    
public:
    
    GrammarAnalyzer(vector<pair<vector<string>, int> > &lex_result):PROGRAM("PROGRAM"){
        this->lex_result = lex_result;
        grammer_analysis();
    }
    
    void grammer_analysis(){
        PROGRAM.add_son(proc_HEADER());
        PROGRAM.add_son(proc_MAIN());
    }
    
    void draw_AST(string AST_path) const{
        ofstream AST(AST_path);
        draw_Node(PROGRAM, 0, true, AST);
        AST.close();
    }
    
    void draw_Node(const Node& node, int cols_off, bool is_first, ofstream &AST) const{
        if(!is_first){
            for(int i=0; i<cols_off; i++){
                for(int j=0; j<Node::format_length; j++){
                    // 4 space(" ")
                    AST << "    ";
                }
            }
        }
        AST << node.format_Component();
        if(node.is_terminal()){
            AST << "\n";
        }
        else{
            for(int i=0; i<node.sons.size(); i++){
                draw_Node(node.sons[i], cols_off+1, (i==0), AST);
            }
        }
    }
    
    const Node& get_root() const{
        return PROGRAM;
    }
    

private:
    
    int cur = 0;
    Node PROGRAM;
    
    vector<pair<vector<string>, int> > lex_result;

    
    string get_word(){
        return lex_result[cur].first[0];
    }
    
    string get_next_word(){
        return lex_result[cur+1].first[0];
    }
    
    void check_add(Node &node, string symbol){
        if(get_word() == symbol){
            node.add_son(Node(symbol));
            cur++;
        }
        else{
            
            node.add_son(Node("Error("+symbol+")"));
            cur--;
            printf("[ERROR] Falta %s despues de '%s' [%s] - [%s]\n", symbol.c_str(), get_word().c_str(), lex_result[cur].first[1].c_str(), lex_result[cur].first[2].c_str());
            cur++;
        }
    }
    
    bool is_Type(){
        string word = get_word();
        return (word=="int" || word=="char" || word=="bool" || word=="void" ||
                word=="string" || word=="double" || word=="float");
    }
    
    bool is_SingOp(){
        string word = get_word();
        return (word=="-" || word=="!" || word=="~" || word=="++" || word=="--");
    }
    
    
    bool is_Id(){
        int word_id = lex_result[cur].second;
        return KEY_WORDS[word_id-1] == _Id_;
    }
    
    bool is_Number(){
        int word_id = lex_result[cur].second;
        string word_type = KEY_WORDS[word_id-1];
        return (word_type=="Octal_Number" || word_type==_Decimal_Number_ || word_type=="Hexademical_Number" || word_type==_Float_Number_);
    }
    
    bool is_TF(){
        string word = get_word();
        return (word=="true" || word=="false");
    }
    
    bool is_Const(){
        return is_Number() || is_TF();
    }

    
    bool is_Asig_C_Op(string word){
        return (word=="+=" || word=="-=" || word== "/=" ||
                word=="*=" || word=="^=" || word== "&=" ||
                word=="|=" || word=="%=" || word== "~=" || word=="!=");
    }
    
    bool is_Asig_C_Op(){
        return is_Asig_C_Op(get_word());
    }
    
    bool is_Stmts_End(){
        string word = get_word();
        return (word == "}" || word=="case" || word=="default");
    }
    
    bool is_HEADER_End(){
        return get_word()=="int" && get_next_word()=="main";
    }
    
    bool is_FunDef(){
        bool cond1 = is_Type();
        cur++;
        bool cond2 = is_Id();
        bool cond3 = get_next_word()=="(";
        cur--;
        return cond1 && cond2 && cond3;
    }
    
    
    // Program -> int main Block
    Node proc_Program(){
        Node Program("Program");
        
        Program.add_son(proc_HEADER());
        Program.add_son(proc_MAIN());

        return Program;
    }
    
    // HEADER -> H_Stmts
    Node proc_HEADER(){
        Node HEADER(_HEADER_);
        HEADER.add_son(proc_H_Stmts());
        return HEADER;
    }
    
    // H_Stmts -> H_Stmt H_Stmts | Empty
    Node proc_H_Stmts(){
        Node H_Stmts("H_Stmts");
        
        while(!is_HEADER_End()){
            H_Stmts.add_son(proc_H_Stmt());
        }
        
        return H_Stmts;
    }
    
    // H_Stmt -> FunDef | Decl
    Node proc_H_Stmt(){
        Node H_Stmt("H_Stmt");
        
        if(is_FunDef()){
            H_Stmt.add_son(proc_FunDef());
        }
        else{
            H_Stmt.add_son(proc_Decl());
            check_add(H_Stmt, ";");
        }
        
        return H_Stmt;
    }
    
    
    // FunDef -> Type FunName ( ParasDef ) Block
    Node proc_FunDef(){
        Node FunDef("FunDef");
        FunDef.add_son(proc_Type());
        FunDef.add_son(proc_FunName());
        check_add(FunDef, "(");
        FunDef.add_son(proc_ParasDef());
        check_add(FunDef, ")");
        FunDef.add_son(proc_Block());
        return FunDef;
    }
    
    // ParasDef -> ParaDef | ParaDef , ParasDef | Empty
    Node proc_ParasDef(){
        Node ParasDef("ParasDef");
        
        while(get_word()!=")"){
            ParasDef.add_son(proc_ParaDef());
            if(get_word()==","){
                check_add(ParasDef, ",");
            }
        }
        
        return ParasDef;
    }
    
    // ParaDef -> Type Id
    Node proc_ParaDef(){
        Node ParaDef("ParaDef");
        
        ParaDef.add_son(proc_Type());
        ParaDef.add_son(proc_Id());
        
        return ParaDef;
    }
    
    // MAIN -> int main ( ) Block
    Node proc_MAIN(){
        Node MAIN("MAIN");
        check_add(MAIN, "int");
        check_add(MAIN, "main");
        check_add(MAIN, "(");
        check_add(MAIN, ")");

        MAIN.add_son(proc_Block());
        return MAIN;
    }
    
    // Block -> { Stmts }
    Node proc_Block(){
        Node Block("Block");

        check_add(Block, "{");

        if(!is_Stmts_End()){
            Block.add_son(proc_Stmts());
        }
        
        check_add(Block, "}");
        return Block;
    }
    
    // Stmts -> Stmt Stmts | Empty
    Node proc_Stmts(){
        Node Stmts("Stmts");
        
        Stmts.add_son(proc_Stmt());

        while(!is_Stmts_End()){
            Stmts.add_son(proc_Stmt());
        }
            
        return Stmts;
    }
    
    
    //Stmt -> Asig_E ; | Decl ; | break ; | continue; | DO_WHILE | IF |
    //          WHILE | DO_WHILE ; | GET ; | PUT ; | Asig_C ; | Asig_S ; | RETURN ;
    Node proc_Stmt(){
        Node Stmt("Stmt");
//        cout << get_word() << endl;
        if(is_Id()){
            // Asig_C
            if(is_Asig_C_Op(get_next_word())){
                Stmt.add_son(proc_Asig_C());
                check_add(Stmt, ";");
            }
            
            // Asig_S
            else if(get_next_word()=="++" || get_next_word()=="--"){
                Stmt.add_son(proc_Asig_S());
                check_add(Stmt, ";");
            }
            
            //Asig_E
            else if(get_next_word()=="="){
                Stmt.add_son(proc_Asig_E());
                check_add(Stmt, ";");
            }
            
            //Funion Call
            else if(get_next_word()=="("){
                Stmt.add_son(proc_Id());
                check_add(Stmt, ";");
            }
            
            //error : no assignment operator
            else{
                Stmt.add_son(proc_Id());
                check_add(Stmt, "Assignment Operator");
            }
        }
        
        //Decl
        else if(is_Type()){
            Stmt.add_son(proc_Decl());
            check_add(Stmt, ";");
        }
        
        // IF
        else if(get_word() == "if"){
            Stmt.add_son(proc_IF_Block());
        }
        
        // WHILE
        else if(get_word() == "while"){
            Stmt.add_son(proc_WHILE());
        }
        
        // DO_WHILE
        else if(get_word() == "do"){
            Stmt.add_son(proc_DO_WHILE());
            check_add(Stmt, ";");
        }
        
        // break ; | continue ;
        else if(get_word() == "break" || get_word() == "continue"){
            check_add(Stmt, get_word());
            check_add(Stmt, ";");
        }
        
        //SWITCH
        else if(get_word()=="switch"){
            Stmt.add_son(proc_SWITCH());
        }
        
        // RETURN
        else if(get_word() == "return"){
            Stmt.add_son(proc_RETURN());
            check_add(Stmt, ";");
        }
        
        // error : illegal statement
        else{
            check_add(Stmt, "Palabra de declaración");
            cur++;
        }
        return Stmt;
    }

    
    // IF -> if ( Expr ) Block |
    //       if ( Expr ) Block ELSE |
    //       if ( Expr ) Block ELSE_IF ELSE
    
    Node proc_IF(){
        Node IF(_IF_);
        check_add(IF, "if");
        check_add(IF, "(");
        
        IF.add_son(proc_Expr());
        
        check_add(IF, ")");
        
        IF.add_son(proc_Block());
        return IF;
    }
    
    Node proc_IF_Block(){
        
        Node IF_Block(_IF_Block_);
        
        IF_Block.add_son(proc_IF());
        
        
        while(get_word() == "else if"){
            IF_Block.add_son(proc_ELSE_IF());
        }
        
        
        if(get_word() == "else"){
            IF_Block.add_son(proc_ELSE());
        }
        return IF_Block;
    }
    
    // ELSE -> else Block
    Node proc_ELSE(){
        Node ELSE(_ELSE_);
        
        check_add(ELSE, "else");

        ELSE.add_son(proc_Block());
        
        return ELSE;
    }
    
    // ELSE_IF -> else if ( Expr ) Block | else if ( Expr ) Block ELSE_IF
    Node proc_ELSE_IF(){
        Node ELSE_IF(_ELSE_IF_);
        
        check_add(ELSE_IF, "else if");
        check_add(ELSE_IF, "(");
        
        ELSE_IF.add_son(proc_Expr());
        
        check_add(ELSE_IF, ")");
        
        ELSE_IF.add_son(proc_Block());
        return ELSE_IF;
    }
    
    // WHILE -> while ( Expr ) Block
    Node proc_WHILE(){
        Node WHILE("WHILE");
        
        check_add(WHILE, "while");
        check_add(WHILE, "(");
        
        WHILE.add_son(proc_Expr());
        
        check_add(WHILE, ")");
        
        WHILE.add_son(proc_Block());
        
        return WHILE;
    }
    
    // DO_WHILE -> do Block while ( Expr )
    Node proc_DO_WHILE(){
        Node DO_WHILE("DO_WHILE");
        check_add(DO_WHILE, "do");
        DO_WHILE.add_son(proc_Block());
        check_add(DO_WHILE, "while");
        check_add(DO_WHILE, "(");
        DO_WHILE.add_son(proc_Expr());
        check_add(DO_WHILE, ")");
        return DO_WHILE;
    }
    
    
    
    Node proc_Type(){
        Node Type("Type");
        Type.add_son(Node(get_word()));
        cur++;
        return Type;
    }
    
    // Decl -> Type Descs
    Node proc_Decl(){
        Node Decl(_Decl_);
        
        Decl.add_son(proc_Type());
        Decl.add_son(proc_Descs());
        
        return Decl;
        
    }
    
    // Descs -> Desc | Desc , Descs
    Node proc_Descs(){
        Node Descs(_Descs_);
        Descs.add_son(proc_Desc());
        while(get_word()==","){
            check_add(Descs, ",");
            Descs.add_son(proc_Desc());
        }
        return Descs;
    }
    
    // Desc -> Id | Asig_E
    Node proc_Desc(){
        Node Desc(_Desc_);
        if(is_Id() && get_next_word()=="="){
            Desc.add_son(proc_Asig_E());
        }
        else if(is_Id()){
            Desc.add_son(proc_Id());
        }
        return Desc;
    }
    
    // Paras -> Para | Para , Paras | Empty
    Node proc_Paras(){
        Node Paras("Paras");
        while(get_word()!=")"){
            Paras.add_son(proc_Para());
            if(get_word()==","){
                check_add(Paras, ",");
            }
        }
        return Paras;
    }
    
    //Para -> Expr
    Node proc_Para(){
        Node Para("Para");
        Para.add_son(proc_Expr());
        return Para;
    }
    
    Node proc_FunName(){
        Node FunName("FunName");
        check_add(FunName, get_word());
        return FunName;
    }
    
    
    //Id or FunCall or Id++ or Id--
    Node proc_Id(){
        if(get_next_word()=="("){
            Node FunCall("FunCall");
            
            FunCall.add_son(proc_FunName());
            check_add(FunCall, "(");
            
            FunCall.add_son(proc_Paras());
            check_add(FunCall, ")");
            return FunCall;
        }
//        else if(get_next_word()=="++" || get_next_word()=="--"){
//            Node Factor("Factor");
////            check_add(Factor, get_word());
////            Factor.add_son(proc_Id());
//            Node Id(_Id_);
//            check_add(Id, get_word());
//            Factor.add_son(Id);
//            check_add(Factor, get_word());
//            return Factor;
//        }
        else{
            Node Id(_Id_);
            check_add(Id, get_word());
            return Id;
        }
    }
    
    Node proc_Number(){
        int word_id = lex_result[cur].second;
        string number_type = KEY_WORDS[word_id-1];
        Node Number(number_type);
        Number.add_son(get_word());
        cur++;
        return Number;
    }
    
    Node proc_TF(){
        Node TF(_TF_);
        TF.add_son(Node(get_word()));
        cur++;
        return TF;
    }
    
    Node proc_Const(){
        Node Const("Const");
        if(is_Number()){
            Const.add_son(proc_Number());
        }
        else if(is_TF()){
            Const.add_son(proc_TF());
        }
        return Const;
    }
    
    // Asig_E -> Id = Expr
    Node proc_Asig_E(){
        Node Asig_E(_Asig_E_);
        
        Asig_E.add_son(proc_Id());
        
        check_add(Asig_E, "=");

        Asig_E.add_son(proc_Expr());
        return Asig_E;
    }
    

    // Factor -> Number | ( Expr ) | Id | FunCall | SingOp Factor
    Node proc_Factor(){
        Node Factor("Factor");
        
        if(is_Id() && (get_next_word()=="++" || get_next_word()=="--")){
            Factor.add_son(proc_Id());
            check_add(Factor, get_word());
        }
        else if(is_Id()){
            Factor.add_son(proc_Id());
        }
        else if(is_Number()){
            Factor.add_son(proc_Number());
        }
        
        else if(is_TF()){
            Factor.add_son(proc_TF());
        }
        
        else if(get_word()=="("){
            check_add(Factor, "(");
            
            Factor.add_son(proc_Expr());
            
            check_add(Factor, ")");
        }
        else if(is_SingOp()){
            check_add(Factor, get_word());
            
            Factor.add_son(proc_Id());
        }
        else{
            //error
            check_add(Factor, "Expresión");
        }
        return Factor;
        
    }

    
    int get_Prec(string op){
        return Op_Precedence[op];
    }
    
    Node merge(Node &LHS, Node &BinOp, Node &RHS){
        Node c("Expr");
        c.add_son(LHS);
        c.add_son(BinOp);
        c.add_son(RHS);
        return c;
    }
    
    
    // Expr -> Factor | Expr BinOp Expr
    Node proc_Expr(){
        Node LHS = proc_Factor();
        // no operator ahead, so pass Prev_Prec=0 to RHS
        return proc_RHS(LHS, 0);
    }
    
    Node proc_RHS(Node &LHS, int Prev_Prec){
        while(true){
            // get the current operator and its precedence
            string op = get_word();
            int cur_Prec = Op_Precedence[op];
            
            // if the previous operator is prior to or equal to the current one
            // then then LHS must be viewed as a whole calculation unit
            if(cur_Prec <= Prev_Prec){
                return LHS;
            }
            
            // get the next Factor
            cur++;
            Node RHS = proc_Factor();
            
            // get the next operator and its precedence
            string next_op = get_word();
            
            int next_Prec = Op_Precedence[next_op];
            
            // if the next operator is prior to the current one
            // then use recursive method to get the corrent RHS
            // pass the current operator's precedence to the recursion function
            if(next_Prec > cur_Prec){
                RHS = proc_RHS(RHS, cur_Prec);
            }
            
            // merge LHS, op and RHS
            Node BinOp = Node(op);
            LHS = merge(LHS, BinOp, RHS);
        }
    }
    

    
    // SWITCH -> switch ( Expr ) { CASES DEFAULT }
    Node proc_SWITCH(){
        Node SWITCH("SWITCH");
        
        check_add(SWITCH, "switch");
        check_add(SWITCH, "(");
        SWITCH.add_son(proc_Expr());
        check_add(SWITCH, ")");
        
        check_add(SWITCH, "{");
        SWITCH.add_son(proc_CASES());
        SWITCH.add_son(proc_DEFAULT());
        
        check_add(SWITCH, "}");
        
        return SWITCH;
    }
    
    // CASES -> CASE | CASE CASES
    Node proc_CASES(){
        Node CASES("CASES");
        
        while(get_word()=="case"){
            CASES.add_son(proc_CASE());
        }
        
        return CASES;
    }
    
    
    // CASE -> case Const : Stmts
    Node proc_CASE(){
        Node CASE("CASE");
        
        check_add(CASE, "case");
        if(is_Const()){
            CASE.add_son(proc_Const());
        }
        check_add(CASE, ":");
        CASE.add_son(proc_Stmts());
        
        
        return CASE;
    }
    
    
    // DEFAULT -> default : Stmts
    Node proc_DEFAULT(){
        Node DEFAULT("DEFAULT");
        check_add(DEFAULT, "default");
        check_add(DEFAULT, ":");
        DEFAULT.add_son(proc_Stmts());

        return DEFAULT;
    }
    
    // RETURN -> return Expr
    Node proc_RETURN(){
        Node RETURN("RETURN");
        
        check_add(RETURN, "return");
        
        RETURN.add_son(proc_Expr());
        
        return RETURN;
    }
    
    // Asig_C -> Id Asig_Op Expr
    Node proc_Asig_C(){
        Node Asig_C("Asig_C");
        
        if(is_Id()){
            Asig_C.add_son(proc_Id());
        }
        if(is_Asig_C_Op()){
            check_add(Asig_C, get_word());
        }
        Asig_C.add_son(proc_Expr());
        
        return Asig_C;
    }
    
    // Asig_S -> Id ++ | Id --
    Node proc_Asig_S(){
        Node Asig_S("Asig_S");
        

        Asig_S.add_son(proc_Id());


        return Asig_S;
    }
};


#endif /* GrammarAnalyzer_h */
