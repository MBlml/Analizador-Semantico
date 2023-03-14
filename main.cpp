//  main.cpp
//  Analizador Semantico

#include "LexicalAnalyzer.h"
#include "GrammarAnalyzer.h"
#include "SemanticAnalyzer.h"

int main(int argc, const char * argv[]) {
    LexicalAnalyzer lex("codigo_fuente.txt");
    GrammarAnalyzer grammar(lex.lex_result);
    grammar.draw_AST("AST.txt");

    SemanticAnalyzer se(grammar.get_root());
    se.semantic_analysis();
}