#include "greibach.hpp"
#include "logger.hpp"

#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>

using namespace std;

static string rsideToString(const R_side &r) {
    if (r.empty()) return "&";
    string s;
    for (const auto &sym : r) s += sym;
    return s;
}

static string freshVar(set<Symbol> &V, const string &prefix, int &counter) {
    while (true) {
        string cand = prefix + to_string(counter++);
        if (V.count(cand) == 0) {
            V.insert(cand);
            return cand;
        }
    }
}

static void substituteFirstVar(map<Symbol, vector<R_side>> &P,
                               const Symbol &Ai,
                               const Symbol &Aj,
                               vector<string> &logDetails) {
    vector<R_side> newList;
    for (const auto &rhs : P[Ai]) {
        if (!rhs.empty() && rhs[0] == Aj) {
            
            for (const auto &rhsAj : P[Aj]) {
                R_side combined;
               
                combined.reserve(rhsAj.size() + rhs.size() - 1);
                for (const auto &s : rhsAj) combined.push_back(s);
                for (size_t k = 1; k < rhs.size(); ++k) combined.push_back(rhs[k]);
                newList.push_back(combined);
                stringstream ss;
                ss << "Substituicao: " << Ai << " -> " << rsideToString(rhs) << "  =>  " << Ai << " -> " << rsideToString(combined);
                logDetails.push_back(ss.str());
            }
        } else {
            newList.push_back(rhs);
        }
    }
    P[Ai] = std::move(newList);
}
static void eliminateImmediateLeftRecursion(const Symbol &A,
                                            map<Symbol, vector<R_side>> &P,
                                            set<Symbol> &V,
                                            int &auxCounter,
                                            vector<string> &logDetails) {
    vector<R_side> prods = P[A];
    vector<R_side> left;   
    vector<R_side> nonleft; 

    for (const auto &rhs : prods) {
        if (!rhs.empty() && rhs[0] == A) {
            R_side alpha(rhs.begin() + 1, rhs.end());
            left.push_back(alpha);
        } else {
            nonleft.push_back(rhs);
        }
    }

    if (left.empty()) return; 

    
    string Aprime = freshVar(V, A + "_p", auxCounter);
    logDetails.push_back("Eliminando recursao esquerda em " + A + ", criado " + Aprime);

    vector<R_side> newA;
    for (auto beta : nonleft) {
        R_side nb = beta;
        nb.push_back(Aprime);
        newA.push_back(nb);
        stringstream ss; ss << A << " -> " << rsideToString(nb);
        logDetails.push_back("  Nova: " + ss.str());
    }
    P[A] = newA;

   
    vector<R_side> newAprime;
    for (auto alpha : left) {
        R_side na = alpha;
        na.push_back(Aprime);
        newAprime.push_back(na);
        stringstream ss; ss << Aprime << " -> " << rsideToString(na);
        logDetails.push_back("  Nova: " + ss.str());
    }
   
    newAprime.push_back(R_side{}); 
    logDetails.push_back("  Nova: " + Aprime + " -> &");

    P[Aprime] = newAprime;
}


void toGreibach(Grammar &G, const std::string &logFile) {
    logTitle(logFile, "CONVERSAO PARA FORMA NORMAL DE GREIBACH (FNG)");

  
    map<Symbol, vector<R_side>> P = G.P;
    vector<Symbol> order;   
    order.reserve(G.V.size());
    for (const auto &v : G.V) order.push_back(v);
    sort(order.begin(), order.end()); 
    set<Symbol> Vset = G.V;

    writeLog(logFile, "Ordem das variaveis (fixa):");
    {
        string s;
        for (auto &x : order) s += x + " ";
        writeLog(logFile, s);
    }

    int auxCounter = 1;
   
    vector<string> stepDetails;

  
    for (size_t i = 0; i < order.size(); ++i) {
        Symbol Ai = order[i];
        stepDetails.clear();
        writeLog(logFile, string("Processando variavel: ") + Ai);

        
        for (size_t j = 0; j < i; ++j) {
            Symbol Aj = order[j];
          
            bool found = false;
            for (const auto &rhs : P[Ai]) {
                if (!rhs.empty() && rhs[0] == Aj) { found = true; break; }
            }
            if (found) {
                substituteFirstVar(P, Ai, Aj, stepDetails);
            }
        }

      
        eliminateImmediateLeftRecursion(Ai, P, Vset, auxCounter, stepDetails);

     
        if (!stepDetails.empty()) {
            writeLog(logFile, "Detalhes de substituicoes / recursao para " + Ai + ":");
            for (auto &d : stepDetails) writeLog(logFile, "  " + d);
        } else {
            writeLog(logFile, " Nenhuma substituicao/recursao imediata para " + Ai);
        }
    }

    writeLog(logFile, "Tentativa final de substituir producoes que comecam por variavel.");
    bool changed = true;
    int iter = 0, maxIter = 1000;
    while (changed && iter < maxIter) {
        changed = false;
        ++iter;
        for (auto it = order.begin(); it != order.end(); ++it) {
            Symbol A = *it;
            vector<R_side> newList;
            for (const auto &rhs : P[A]) {
                if (!rhs.empty() && Vset.count(rhs[0])) {
                    Symbol B = rhs[0];
                    for (const auto &brhs : P[B]) {
                        R_side combined;
                        combined.reserve(brhs.size() + rhs.size() - 1);
                        for (auto &s : brhs) combined.push_back(s);
                        for (size_t k = 1; k < rhs.size(); ++k) combined.push_back(rhs[k]);
                        newList.push_back(combined);
                        changed = true;
                    }
                } else {
                    newList.push_back(rhs);
                }
            }
            P[A] = std::move(newList);
        }
    }
    if (iter >= maxIter) writeLog(logFile, "Aviso: limite de iteracoes atingido na substituicao final.");

    map<Symbol, vector<R_side>> finalP;
    for (const auto &kv : P) {
        const Symbol &A = kv.first;
        for (const auto &rhs : kv.second) {
            if (rhs.empty()) {
                if (A == G.S) finalP[A].push_back(rhs);
            } else {
                if (!G.V.count(rhs[0])) {
                    finalP[A].push_back(rhs);
                } else {
                    stringstream ss;
                    ss << "Dropping production (não começa com terminal): " << A << " -> " << rsideToString(rhs);
                    writeLog(logFile, ss.str());
                }
            }
        }
    }

    set<Symbol> newV;
    set<Symbol> newT;
    for (const auto &kv : finalP) {
        newV.insert(kv.first);
        for (const auto &rhs : kv.second) {
            for (const auto &s : rhs) {
                if (G.V.count(s)) newV.insert(s);
                else newT.insert(s);
            }
        }
    }
    for (const auto &v : Vset) {
        if (finalP.count(v)) newV.insert(v);
    }

    G.P = finalP;
    G.V = newV;
    G.T = newT;

    writeLog(logFile, "=> Gramática (tentativa) em Forma Normal de Greibach:");
    logGrammar(logFile, G);

    writeLog(logFile, "Conversão para GNF (tentativa) concluída.");
}
