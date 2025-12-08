#ifndef UNIT_HPP
#define UNIT_HPP

#include <string>
#include "grammar.hpp"

// Remove produções unitárias (A -> B) da gramática G.
// Escreve passos no arquivo de log indicado.
void removeUnitProductions(Grammar &G, const std::string &logFile);

#endif // UNIT_HPP
