#include "cnf.hpp"
#include "logger.hpp"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Serializa um R_side para chave
static string rsideKey(const R_side &r){
    if(r.empty()) return "&";
    stringstream ss;
    bool first = true;
    for(const auto &s : r){
        if(!first) ss << " ";
        first = false;
        ss << s;
    }
    return ss.str();
}

static string freshVarName(set<Symbol> &existing, const string &prefix, int &counter){
    while (true) {
        string cand = prefix + to_string(counter++);
        if (existing.count(cand) == 0) {
            existing.insert(cand);
            return cand;
        }
    }
}

// Converte a gramática G para FNC e registra passos no logFile
void toCNF(Grammar &G, const std::string &logFile) {
    logTitle(logFile, "CONVERSAO PARA FORMA NORMAL DE CHOMSKY (FNC)");

    map<Symbol, vector<R_side>> P = G.P; // cópia das produções
    set<Symbol> V = G.V;
    set<Symbol> T = G.T;
    Symbol S = G.S;

    writeLog(logFile, "=> Estado inicial (antes CNF):");
    logGrammar(logFile, G);

    writeLog(logFile, "=> Substituindo terminais em RHS com comprimento >= 2 por variáveis auxiliares");

    map<Symbol, Symbol> termVar;
    int termVarCounter = 1;

    map<Symbol, vector<R_side>> newP;

    for (const auto &kv : P) {
        const Symbol &A = kv.first;
        for (const auto &rhs : kv.second) {
            // se rhs tem comprimento >= 2, substituir terminais internos
            if (rhs.size() >= 2) {
                R_side newRhs;
                for (const auto &sym : rhs) {
                    if (V.count(sym) == 0) {
                        if (termVar.count(sym) == 0) {
                            string base = "T";
                            string helper = freshVarName(V, base, termVarCounter);
                            termVar[sym] = helper;
                            newP[ helper ].push_back(R_side{ sym });
                            writeLog(logFile, string("Criada variavel auxiliar: ") + helper + " -> " + sym);
                        }
                        newRhs.push_back(termVar[sym]);
                    } else {
                        newRhs.push_back(sym);
                    }
                }
                newP[A].push_back(newRhs);
            } else {
                newP[A].push_back(rhs);
            }
        }
    }

    writeLog(logFile, "Gramática após substituição de terminais em RHS longos:");
    {
        Grammar tmp = G;
        tmp.P = newP;
        tmp.V = V;
        tmp.T = T;
        logGrammar(logFile, tmp);
    }

    //BINARIZAR PRODUÇÕES COM MAIS DE 2 SÍMBOLOS
    writeLog(logFile, "=> Binarizando producoes: transformar RHS com len>2 em cadeias binarias");

    map<Symbol, vector<R_side>> binP;
    int auxCounter = 1; // para nomes de variáveis auxiliares de binarização

    for (const auto &kv : newP) {
        const Symbol &A = kv.first;
        for (const auto &rhs : kv.second) {
            if (rhs.size() <= 2) {
                // ja esta ok
                binP[A].push_back(rhs);
                continue;
            }
            // copia dos símbolos
            vector<Symbol> symbols(rhs.begin(), rhs.end());
            Symbol left = A;

            for (size_t i = 0; i + 2 < symbols.size(); ++i) {
                Symbol X = symbols[i];
                // criar novo var auxiliar para o par restante
                Symbol Y = freshVarName(V, "X", auxCounter);
                R_side r1;
                r1.push_back(X);
                r1.push_back(Y);
                binP[left].push_back(r1);
                writeLog(logFile, string("Binarizada: ") + left + " -> " + X + " " + Y);
                left = Y;
            }
            R_side last;
            last.push_back(symbols[symbols.size() - 2]);
            last.push_back(symbols[symbols.size() - 1]);
            binP[left].push_back(last);
        }
    }

    writeLog(logFile, "Gramática após binarizacao:");
    {
        Grammar tmp = G;
        tmp.P = binP;
        tmp.V = V;
        tmp.T = T;
        logGrammar(logFile, tmp);
    }

    //GARANTIR PRODUCOES DO TIPO A -> a E A -> B C
    writeLog(logFile, "=> Limpando producoes inválidas (garantir apenas A->a ou A->B C).");

    map<Symbol, vector<R_side>> finalP;
    set<string> seenPairs;

    for (const auto &kv : binP) {
        const Symbol &A = kv.first;
        for (const auto &rhs : kv.second) {
            if (rhs.empty()) {
                if (A == S) {
                    finalP[A].push_back(rhs);
                } else {
                    // descartar
                }
            } else if (rhs.size() == 1) {
                Symbol s = rhs[0];
                finalP[A].push_back(rhs);
            } else if (rhs.size() == 2) {
                if (V.count(rhs[0]) && V.count(rhs[1])) {
                    finalP[A].push_back(rhs);
                } else {
                    R_side fixed;
                    for (const auto &s : rhs) {
                        if (V.count(s)) fixed.push_back(s);
                        else {
                            if (termVar.count(s) == 0) {
                                Symbol helper = freshVarName(V, "T", termVarCounter);
                                termVar[s] = helper;
                                finalP[ helper ].push_back(R_side{ s });
                                writeLog(logFile, string("Criada variavel auxiliar (ajuste final): ") + helper + " -> " + s);
                            }
                            fixed.push_back(termVar[s]);
                        }
                    }
                    finalP[A].push_back(fixed);
                }
            } else {
            }
        }
    }

    // Atualizar G com finalP e atualizar V e T corretamente
    // recomputar conjunto de terminais reais
    set<Symbol> newV = V;
    set<Symbol> newT;
    for (const auto &kv : finalP) {
        for (const auto &rhs : kv.second) {
            if (rhs.size() == 1) {
                Symbol s = rhs[0];
                if (newV.count(s) == 0) newT.insert(s);
            } else {
                for (const auto &s : rhs) {
                }
            }
        }
    }

    G.P = finalP;
    G.V = newV;
    G.T = newT;

    writeLog(logFile, "=> Gramática final em FNC:");
    logGrammar(logFile, G);

    writeLog(logFile, "Conversão para FNC concluída.");
}