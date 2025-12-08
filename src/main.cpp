#include <iostream>
#include <string>
#include "parser.hpp"
#include "lambda.hpp"
#include "unit.hpp"
#include "useless.hpp"
#include "cnf.hpp"
#include "greibach.hpp"
#include "logger.hpp"

using namespace std;

int main() {
    string inputFile;
    string optionStr;
    int option = 0;

    cout << "Digite o caminho do arquivo de entrada (.txt) que contem a GLC:" << endl;
    getline(cin, inputFile);

    cout << "Escolha a normalizacao desejada:" << endl
         << " 1 - Forma Normal de Chomsky (FNC / CNF)" << endl
         << " 2 - Forma Normal de Greibach (FNG / GNF)" << endl
         << " 3 - Apenas remover lambda, unitarias e simbolos inuteis" << endl;
    getline(cin, optionStr);

    if (optionStr.empty()) {
        cout << "Opcao invalida. Saindo." << endl;
        return 0;
    }

    option = stoi(optionStr);

    if (option < 1 || option > 3) {
        cout << "Opcao invalida. Saindo." << endl;
        return 0;
    }

    string logFile = inputFile.substr(0, inputFile.find_last_of('.')) + "_log.txt";
    startLog(logFile);

    writeLog(logFile, "===== LOG DO PROCESSO DE NORMALIZACAO =====\n");

    Grammar G = readGrammar(inputFile);

    // Pipeline padrao
    removeLambda(G, logFile);
    removeUnitProductions(G, logFile);
    removeUselessSymbols(G, logFile);

    if (option == 1)
        toCNF(G, logFile);
    else if (option == 2)
        toGreibach(G, logFile);

    writeLog(logFile, "\nNormalizacao concluida.\n");

    cout << "\nProcesso concluido. Log salvo em: " << logFile << endl;
    cout << "Confira o arquivo de log para ver as gramaticas intermediarias e detalhes." << endl;

    return 0;
}
