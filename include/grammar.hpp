#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <vector>
#include <map>
#include <set>


using Symbol = std::string;

using R_side = std::vector<Symbol>;

struct Grammar{
    std::set<Symbol> V;                                //Variaveis
    std::set<Symbol> T;                                //Terminais
    Symbol S;                                          //Simbolo inicial
    std::map<Symbol, std::vector<R_side>> P;           //Producoes
};

#endif