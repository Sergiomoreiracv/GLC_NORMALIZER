#ifndef USELESS_HPP
#define USELESS_HPP

#include <string>
#include <set>
#include "grammar.hpp"

// Remove variáveis não-geradoras e símbolos inalcançáveis
void removeUselessSymbols(Grammar &G, const std::string &logFile);

#endif
