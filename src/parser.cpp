#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

std::string trim(const std::string &s){
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')) i++;
    if (i == s.size()) return ""; // string era toda em branco


    size_t j = s.size() - 1;
    while (j > i && (s[j] == ' ' || s[j] == '\t' || s[j] == '\n' || s[j] == '\r')) j--;


    return s.substr(i, j - i + 1);
}

bool isVariable(const std::string &x) {
    if (x.empty()) return false;
    for (char c : x) {
    if (!std::isupper((unsigned char)c)) return false;
    }
    return true;
}

std::vector<std::string> tokenizeR_side(const std::string &right) {
    std::vector<std::string> result;

    for (size_t i = 0; i < right.size(); i++) {
        char c = right[i];

        if (std::isupper((unsigned char)c)) {
            // Variável = 1 caractere maiúsculo
            result.push_back(std::string(1, c));
        } else {
            // Terminal = 1 caractere não maiúsculo
            result.push_back(std::string(1, c));
        }
    }

    return result;
}


Grammar readGrammar(const std::string &filename){
    std::ifstream in(filename);
    if(!in){
        throw std::runtime_error("Erro: não foi possível abrir o arquivo de entrada.");
    }

    Grammar G;
    std::string line;
    bool primeiraProducao = true;

    while (std::getline(in, line)){
        line = trim(line);
        if(line.empty()) continue;
        if(line[0] == '#') continue;

        size_t pos = line.find("->");
        if (pos == std::string::npos) continue;

        std::string left = trim(line.substr(0, pos));
        std::string right = trim(line.substr(pos + 2));

        if(primeiraProducao){
            G.S = left;
            primeiraProducao = false;
        }
        G.V.insert(left);

        std::stringstream ss(right);
        std::string part;
        while (std::getline(ss, part, '|')){
            part = trim(part);

            if(part == "&"){
                G.P[left].push_back({});
                continue;
            }

            //tokeniza parte
            auto rightTokens = tokenizeR_side(part);
            G.P[left].push_back(rightTokens);

            // registra terminais e variaveis
            for(auto &s: rightTokens){
                if (isVariable(s)) G.V.insert(s);
                else G.T.insert(s);
            }

        }
    }
    return G;
}