#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include "grammar.hpp"

// Limpa o arquivo de log
void startLog(const std::string &filename);

// Escreve uma linha no log
void writeLog(const std::string &filename, const std::string &text);

// Escreve um título no log
void logTitle(const std::string &filename, const std::string &title);

// Escreve a gramática no log
void logGrammar(const std::string &filename, const Grammar &G);

#endif
