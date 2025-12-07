#include "logger.hpp"
#include <iostream>
#include <fstream>

void startLog(const std::string &filename) {
std::ofstream out(filename, std::ios::trunc);
if (!out) {
std::cerr << "Erro ao criar arquivo de log: " << filename << "\n";
return;
}
out << "===== LOG DO PROCESSO DE NORMALIZACAO =====\n\n";
}

void writeLog(const std::string &filename, const std::string &text) {
std::ofstream out(filename, std::ios::app);
if (!out) return;
out << text << "\n";
}


void logTitle(const std::string &filename, const std::string &title) {
std::ofstream out(filename, std::ios::app);
if (!out) return;
out << "\n=======================================\n";
out << " " << title << "\n";
out << "=======================================\n\n";
}

void logGrammar(const std::string &filename, const Grammar &G) {
std::ofstream out(filename, std::ios::app);
if (!out) return;


out << "Variaveis: ";
for (const auto &v : G.V) out << v << " ";
out << "\n";


out << "Terminais: ";
for (const auto &t : G.T) out << t << " ";
out << "\n";


out << "Simbolo inicial: " << G.S << "\n";


out << "Producoes:\n";
for (const auto &kv : G.P) {
out << " " << kv.first << " -> ";
bool first = true;
for (const auto &rhs : kv.second) {
if (!first) out << " | ";
first = false;
if (rhs.empty()) {
out << "&";
} else {
for (const auto &s : rhs) out << s;
}
}
out << "\n";
}


out << "\n";
}