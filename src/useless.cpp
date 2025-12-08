#include "useless.hpp"
#include "logger.hpp"

#include <set>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <algorithm>

using namespace std;

// ------------------------------------------------------------
// 1. Encontrar variáveis geradoras (que conseguem gerar terminais)
// ------------------------------------------------------------
static set<Symbol> findGenerating(const Grammar &G) {
    set<Symbol> generating;
    bool changed = true;

    // Inicial: variáveis que possuem uma produção composta apenas por terminais
    for (const auto &kv : G.P) {
        const Symbol &A = kv.first;
        for (const auto &rhs : kv.second) {
            bool allTerms = true;
            // uma produção vazia (&) não conta aqui (assumimos que lambda já foi tratado)
            if (rhs.empty()) { allTerms = false; continue; }
            for (const auto &s : rhs) {
                // símbolo terminal (em G.T) -> ok
                // símbolo variável -> não ok para a iteração inicial
                if (G.V.count(s)) { allTerms = false; break; }
                // se não é variável e nem terminal (improvável), trate como não-terminal
                if (!G.T.count(s) && !G.V.count(s)) { allTerms = false; break; }
            }
            if (allTerms) { generating.insert(A); break; }
        }
    }

    // Fecho: A é geradora se existe A -> X1 X2 ... Xn com todos Xi sendo
    // ou terminais, ou variáveis que já estão em 'generating'
    while (changed) {
        changed = false;
        for (const auto &kv : G.P) {
            const Symbol &A = kv.first;
            if (generating.count(A)) continue;
            for (const auto &rhs : kv.second) {
                if (rhs.empty()) continue; // & já foi tratado antes
                bool ok = true;
                for (const auto &s : rhs) {
                    if (G.T.count(s)) continue;                 // terminal é ok
                    if (G.V.count(s) && generating.count(s)) continue; // variável geradora ok
                    ok = false; break;
                }
                if (ok) {
                    generating.insert(A);
                    changed = true;
                    break;
                }
            }
        }
    }

    return generating;
}

// ------------------------------------------------------------
// 2. Encontrar símbolos alcançáveis a partir de S (BFS)
// ------------------------------------------------------------
static set<Symbol> findReachable(const Grammar &G) {
    set<Symbol> reachableV;
    set<Symbol> reachableT;
    queue<Symbol> q;

    // se S não estiver definido, retorna vazio
    if (G.S.empty()) return {};

    reachableV.insert(G.S);
    q.push(G.S);

    while (!q.empty()) {
        Symbol A = q.front(); q.pop();
        auto it = G.P.find(A);
        if (it == G.P.end()) continue;
        for (const auto &rhs : it->second) {
            for (const auto &s : rhs) {
                // se for variável e nova → add e enfileira
                if (G.V.count(s) && !reachableV.count(s)) {
                    reachableV.insert(s);
                    q.push(s);
                }
                // se for terminal, marque-o (somente para info)
                if (G.T.count(s)) reachableT.insert(s);
            }
        }
    }

    // só retornamos variáveis (a função chamadora usa isso)
    return reachableV;
}

// ------------------------------------------------------------
// 3. Remoção de símbolos inúteis (seguro e em etapas)
// ------------------------------------------------------------
void removeUselessSymbols(Grammar &G, const std::string &logFile) {
    logTitle(logFile, "REMOCAO DE SIMBOLOS INUTEIS");
    writeLog(logFile, "=> Calculando variaveis geradoras...");

    // 1) calcular geradores (fecho)
    set<Symbol> gen = findGenerating(G);
    {
        stringstream ss; ss << "Variaveis geradoras: ";
        if (gen.empty()) ss << "(nenhuma)";
        else for (const auto &x : gen) ss << x << " ";
        writeLog(logFile, ss.str());
    }

    // 2) construir P' contendo apenas produções cujo LHS é gerador,
    //    e cujos símbolos no RHS sejam terminais ou variáveis geradoras.
    map<Symbol, vector<R_side>> Pprime;
    for (const auto &kv : G.P) {
        const Symbol &A = kv.first;
        if (!gen.count(A)) continue; // LHS não é gerador → ignorar
        vector<R_side> list;
        for (const auto &rhs : kv.second) {
            bool ok = true;
            for (const auto &s : rhs) {
                // s pode ser terminal (aceito) ou variável geradora (aceito)
                if (G.T.count(s)) continue;
                if (G.V.count(s) && gen.count(s)) continue;
                ok = false; break;
            }
            if (ok) list.push_back(rhs);
        }
        if (!list.empty()) Pprime[A] = std::move(list);
    }

    writeLog(logFile, "=> Producoes apos filtrar nao-geradores:");
    {
        Grammar tmp = G;
        tmp.P = Pprime;
        logGrammar(logFile, tmp);
    }

    // 3) calcular símbolos alcançáveis a partir de S sobre P'
    Grammar Gprime = G;
    Gprime.P = Pprime;

    // Se símbolo inicial não for gerador (ou foi removido), não há nada gerável.
    if (!gen.count(G.S)) {
        writeLog(logFile, "Simbolo inicial NAO eh gerador -> gramática vazia de producoes (só epsilon possivelmente).");
        // Se S gera & (lambda) isso já foi tratado em passo anterior; mantemos a produção S -> & somente se existir.
        // Montamos G com apenas S->& se existir essa produção, caso contrário G fica sem producoes.
        map<Symbol, vector<R_side>> newP;
        auto itS = G.P.find(G.S);
        if (itS != G.P.end()) {
            for (const auto &rhs : itS->second) {
                if (rhs.empty()) {
                    newP[G.S].push_back(rhs);
                }
            }
        }
        G.P = newP;
        // atualizar V e T
        set<Symbol> newV;
        if (G.P.count(G.S)) newV.insert(G.S);
        G.V = newV;
        // recomputar T
        set<Symbol> newT;
        for (const auto &kv : G.P)
            for (const auto &rhs : kv.second)
                for (const auto &s : rhs)
                    if (!G.V.count(s)) newT.insert(s);
        G.T = newT;

        writeLog(logFile, "=> Após remoção (caso especial S não gerador):");
        logGrammar(logFile, G);
        return;
    }

    set<Symbol> reachable = findReachable(Gprime);
    {
        stringstream ss; ss << "Variaveis alcancaveis: ";
        for (const auto &x : reachable) ss << x << " ";
        writeLog(logFile, ss.str());
    }

    // 4) construir P'' contendo apenas produções cujo LHS esteja em reachable
    map<Symbol, vector<R_side>> Psec;
    for (const auto &A : reachable) {
        auto it = Pprime.find(A);
        if (it != Pprime.end()) Psec[A] = it->second;
    }

    // 5) atualizar G: V = reachable ∩ gen ; P = Psec ; ajustar T
    set<Symbol> newV;
    for (const auto &A : reachable) if (gen.count(A)) newV.insert(A);

    set<Symbol> newT;
    for (const auto &kv : Psec) {
        for (const auto &rhs : kv.second) {
            for (const auto &s : rhs) {
                if (!newV.count(s)) newT.insert(s); // terminal
            }
        }
    }

    G.V = newV;
    G.P = Psec;
    G.T = newT;

    writeLog(logFile, "=> Resultado apos remocao de simbolos inuteis:");
    logGrammar(logFile, G);
}
