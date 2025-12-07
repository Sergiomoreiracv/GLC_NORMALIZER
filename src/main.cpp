#include <iostream>
#include "grammar.hpp"
#include "parser.hpp"
#include "logger.hpp"

using namespace std;

int main() {
    string entrada = "entrada.txt";
    string logFile = "log.txt";

    try {
        // Iniciar log
        startLog(logFile);
        logTitle(logFile, "LEITURA DA GRAMATICA A PARTIR DO ARQUIVO");

        // Ler gramática
        Grammar G = readGrammar(entrada);

        // Registrar gramática lida
        writeLog(logFile, "Gramática carregada com sucesso.");
        logGrammar(logFile, G);

        // Mostrar no terminal também
        cout << "\nGramática carregada:\n";
        cout << "Variáveis: ";
        for (auto &v : G.V) cout << v << " ";
        cout << "\n";

        cout << "Terminais: ";
        for (auto &t : G.T) cout << t << " ";
        cout << "\n";

        cout << "Símbolo inicial: " << G.S << "\n";

        cout << "Produções:\n";
        for (auto &kv : G.P) {
            cout << "  " << kv.first << " -> ";
            bool first = true;
            for (auto &rhs : kv.second) {
                if (!first) cout << " | ";
                first = false;
                if (rhs.empty())
                    cout << "&";
                else
                    for (auto &s : rhs) cout << s;
            }
            cout << "\n";
        }

        // Aviso final
        cout << "\n=== LOG GERADO EM: " << logFile << " ===\n";

    } catch (exception &e) {
        cerr << "Erro: " << e.what() << endl;
        writeLog(logFile, string("Erro: ") + e.what());
    }

    return 0;
}

