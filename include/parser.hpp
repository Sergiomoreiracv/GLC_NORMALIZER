#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "grammar.hpp"

// Remove espacos no inicio e fim de uma string
std::string trim(const std::string &s);

bool isVariable(const std::string &x);

//Quebra o lado direito de uma producoa em simbolos
std::vector<std::string> tokenizeR_side(const std::string &right);

Grammar readGrammar(const std::string &filename);

#endif
