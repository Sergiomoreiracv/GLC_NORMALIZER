#include "lambda.hpp"
#include "logger.hpp"

#include <set>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>

using namespace std;

// Encontra todas as variáveis anuláveis
std::set<Symbol> findNullable(const Grammar &G) {
    std::set<Symbol> N;
    bool changed = true;

    // Passo 1: variáveis com produção vazia direta
    for (const auto &kv : G.P) {
        const Symbol &A = kv.first;
        for (const auto &rhs : kv.second) {
            if (rhs.empty()) {
                N.insert(A);
            }
        }
    }

    // Passo 2: fechamento
    while (changed) {
        changed = false;
        for (const auto &kv : G.P) {
            const Symbol &A = kv.first;

            if (N.count(A)) continue;

            for (const auto &rhs : kv.second) {
                if (rhs.empty()) continue;

                bool allNullable = true;
                for (const auto &s : rhs) {
                    if (G.V.count(s) == 0 || N.count(s) == 0) {
                        allNullable = false;
                        break;
                    }
                }

                if (allNullable) {
                    N.insert(A);
                    changed = true;
                    break;
                }
            }
        }
    }

    return N;
}

// Serialização para evitar duplicatas
static string rightToKey(const R_side &r) {
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

// Remove produções lambda
void removeLambda(Grammar &G, const std::string &logFile) {
    logTitle(logFile, "REMOCAO DE REGRAS-LAMBDA");
    writeLog(logFile, "Calculando variáveis anuláveis...");

    set<Symbol> N = findNullable(G);

    // Log das anuláveis
    {
        stringstream ss;
        ss << "Variáveis anuláveis: ";
        if (N.empty()) ss << "(nenhuma)";
        else for (auto &x : N) ss << x << " ";
        writeLog(logFile, ss.str());
    }

    // Novo conjunto de produções
    map<Symbol, vector<R_side>> newP;

    for (const auto &kv : G.P) {
        const Symbol &A = kv.first;
        set<string> seen;

        for (const auto &rhs : kv.second) {

            // Descobre quais símbolos são anuláveis dentro da produção
            vector<int> idx;
            for (int i = 0; i < (int)rhs.size(); i++) {
                const Symbol &s = rhs[i];
                if (G.V.count(s) && N.count(s))
                    idx.push_back(i);
            }

            int m = idx.size();
            int subsets = 1 << m;

            for (int mask = 0; mask < subsets; mask++) {
                R_side cur;

                for (int i = 0; i < (int)rhs.size(); i++) {
                    bool remove = false;
                    for (int j = 0; j < m; j++)
                        if (idx[j] == i && ((mask >> j) & 1))
                            remove = true;

                    if (!remove)
                        cur.push_back(rhs[i]);
                }

                // não permitir cur vazio exceto S anulável
                if (cur.empty() && !(A == G.S && N.count(G.S)))
                    continue;

                string key = rightToKey(cur);
                if (!seen.count(key)) {
                    newP[A].push_back(cur);
                    seen.insert(key);
                }
            }
        }
    }

    G.P = newP;

    writeLog(logFile, "Gramática após remoção de λ-produções:");
    logGrammar(logFile, G);
}
