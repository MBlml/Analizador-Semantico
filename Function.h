//  Function.h

#include "Defines.h"
#ifndef Function_h
#define Function_h

class Function{
public:
    vector<Parameter> parameters;
    int return_type;
    Function(){}
    Function(const vector<Parameter>& parameters, int return_type):parameters(parameters),return_type(return_type) {}
};

class FunctionManager{
public:
    map<string, Function> functions;
    
    FunctionManager(){}
    
    bool exists_function(string function_name){
        return functions.find(function_name) != functions.end();
    }
    
    void add_function(string function_name, const vector<Parameter>& parameters, int return_type){
        if(!exists_function(function_name)){
            functions[function_name] = Function(parameters, return_type);
        }
        else{
            printf("[ERROR] La funcion '%s' ya ha sido definida\n", function_name.c_str());
        }
        
    }
    
    void print_functions(){
        for(auto function : functions){
            printf("-----------------------------------------\n");
            printf("Nombre de la funcion : %s\t|\n", function.first.c_str());
            printf("-----------------------------------------\n");
            printf("Parametros : ");
            for(auto parameter : function.second.parameters){
                printf("%s(%s)\t",  parameter.second.c_str(), TYPE[parameter.first].c_str());
            }
            printf("\n");
            printf("Tipo de retorno : %s\n", TYPE[function.second.return_type].c_str());
            printf("-----------------------------------------\n\n\n");
        }
    }
};

#endif /* Function_h */