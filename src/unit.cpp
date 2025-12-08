#include "unit.hpp"
#include "logger.hpp"

#include <set>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

// Serializa um R_side para chave para evitar duplicatas
static string rsideToKey(const R_side &r) {
    if (r.empty()) return "&";
    stringstream ss;
    bool first = true;
    for (const auto &s : r) {
        if (!first) ss << " ";
        first = false;
        ss << s;
    }
    return ss.str();
}

void removeUnitProductions(Grammar &G, const std::string &logFile) {

    logTitle(logFile, "REMOCAO DE PRODUCOES UNITARIAS");
    writeLog(logFile, "=> Calculando fechamento de producoes unitarias");

    // 🔥 DECLARAÇÃO CORRETA DO MAPA unit
    map<Symbol, set<Symbol>> unit;

    // inicial: cada A inclui a si mesmo
    for (const auto &A : G.V)
        unit[A].insert(A);

    // adicionar relações diretas A -> B quando houver produção unitária
    for (const auto &kv : G.P) {
        const Symbol &A = kv.first;
        for (const auto &rhs : kv.second) {
            if (rhs.size() == 1 && G.V.count(rhs[0])) {
                unit[A].insert(rhs[0]);
            }
        }
    }

    // Fecho transitivo
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto &A : G.V) {

            vector<Symbol> copy(unit[A].begin(), unit[A].end());

            for (const auto &B : copy) {
                for (const auto &C : unit[B]) {
                    if (!unit[A].count(C)) {
                        unit[A].insert(C);
                        changed = true;
                    }
                }
            }
        }
    }

    // Log do fechamento unitário
    {
        stringstream ss;
        ss << "Fechamento unitario (Unit(A)):\n";
        for (const auto &A : G.V) {
            ss << "  " << A << " : ";
            for (const auto &b : unit[A]) ss << b << " ";
            ss << "\n";
        }
        writeLog(logFile, ss.str());
    }

    // Criar novas produções
    map<Symbol, vector<R_side>> newP;

    for (const auto &A : G.V) {
        set<string> seenKeys;

        for (const auto &B : unit[A]) {

            auto it = G.P.find(B);
            if (it == G.P.end()) continue;

            for (const auto &rhs : it->second) {
                if (rhs.size() == 1 && G.V.count(rhs[0])) continue; // unitária

                string key = rsideToKey(rhs);

                if (!seenKeys.count(key)) {
                    newP[A].push_back(rhs);
                    seenKeys.insert(key);
                }
            }
        }
    }

    // Substituir produções por newP
    G.P = newP;

    writeLog(logFile, "=> Gramática após remoção de produções unitárias:");
    logGrammar(logFile, G);
    writeLog(logFile, "Remoção de produções unitárias concluída.");
}
