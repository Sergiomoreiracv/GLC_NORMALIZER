#include <iostream>
#include <fstream>
#include <sstream>
#include "grammar.hpp"
#include "parser.hpp"

using namespace std;

int main() {
    cout << "\n=== LENDO ARQUIVO entrada.txt ===\n\n";

    ifstream in("entrada.txt");
    if (!in) {
        cerr << "Erro: não foi possível abrir entrada.txt\n";
        return 1;
    }

    Grammar G;
    string line;
    bool primeira = true;

    while (getline(in, line)) {
        cout << "Linha lida: \"" << line << "\"\n";

        line = trim(line);
        if (line.empty()) {
            cout << " -> Linha vazia, ignorada.\n\n";
            continue;
        }

        // Encontrar ->
        size_t pos = line.find("->");
        if (pos == string::npos) {
            cout << " -> Não contém '->', ignorada.\n\n";
            continue;
        }

        string left = trim(line.substr(0, pos));
        string right = trim(line.substr(pos + 2));

        cout << "  Lado esquerdo (variável): " << left << "\n";
        cout << "  Lado direito bruto: " << right << "\n";

        if (primeira) {
            G.S = left;
            primeira = false;
            cout << "  -> Definido como símbolo inicial: " << G.S << "\n";
        }

        G.V.insert(left);
        cout << "  -> Variável adicionada ao conjunto V\n";

        // Quebrar por |
        stringstream ss(right);
        string alt;

        while (getline(ss, alt, '|')) {
            alt = trim(alt);
            cout << "  Alternativa encontrada: \"" << alt << "\"\n";

            if (alt == "&") {
                cout << "    -> Esta alternativa é λ\n";
                G.P[left].push_back({});
                continue;
            }

            auto tokens = tokenizeR_side(alt);

            cout << "    Tokens gerados:\n";
            for (auto &t : tokens) {
                cout << "      [" << t << "] -> ";

                if (isVariable(t)) {
                    cout << "Variável\n";
                    G.V.insert(t);
                } else {
                    cout << "Terminal\n";
                    G.T.insert(t);
                }
            }

            // Guardar produção
            G.P[left].push_back(tokens);
        }

        cout << "\n";
    }

    // Exibir estrutura final ---------------------------
    cout << "\n=========== RESUMO FINAL ===========\n";

    cout << "\nVariáveis (G.V): ";
    for (auto &v : G.V) cout << v << " ";
    cout << "\n";

    cout << "\nTerminais (G.T): ";
    for (auto &t : G.T) cout << t << " ";
    cout << "\n";

    cout << "\nSímbolo inicial (G.S): " << G.S << "\n";

    cout << "\nProduções (G.P):\n";
    for (auto &kv : G.P) {
        cout << "  " << kv.first << " -> ";
        bool first = true;
        for (auto &right : kv.second) {
            if (!first) cout << " | ";
            first = false;

            if (right.empty()) {
                cout << "&";
            } else {
                for (auto &s : right) cout << s;
            }
        }
        cout << "\n";
    }

    cout << "\n=====================================\n";

    return 0;
}
