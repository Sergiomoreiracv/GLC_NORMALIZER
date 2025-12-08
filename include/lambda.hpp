#ifndef LAMBDA_HPP
#define LAMBDA_HPP

#include <set>
#include <string>
#include "grammar.hpp"

// Encontra todas as variáveis anuláveis
std::set<Symbol> findNullable(const Grammar &G);

// Remove produções λ (lambda)
void removeLambda(Grammar &G, const std::string &logFile);

#endif
