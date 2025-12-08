# GLC Normalizer --- Normalização de Gramáticas Livres de Contexto

Este projeto implementa, em C++, uma ferramenta capaz de **ler uma
gramática livre de contexto (GLC)** a partir de um arquivo `.txt` e
**normalize-la** conforme a opção escolhida pelo usuário:

-   **Remoção de regras-λ (lambda)**
-   **Remoção de produções unitárias**
-   **Remoção de símbolos inúteis**
-   **Conversão para Forma Normal de Chomsky (FNC/CNF)**
-   **Conversão para Forma Normal de Greibach (FNG/GNF)**

Como saída, o programa gera um arquivo `.txt` contendo **um log
completo**, detalhando passo a passo todas as transformações realizadas
na gramática.

------------------------------------------------------------------------

##  Características principais

✔️ Aceita gramática **completa ou reduzida**\
✔️ Interpreta **&** como palavra vazia\
✔️ Gera **log detalhado** de cada etapa\
✔️ Suporta normalização para **FNC (Chomsky)**\
✔️ Suporta normalização para **FNG (Greibach)**\
✔️ Implementado com módulos claros e independentes\
✔️ Fácil de compilar e executar em qualquer sistema Linux ou Windows
(WSL)

------------------------------------------------------------------------

## 📁 Estrutura do Projeto

    GLC_NORMALIZER/
    │
    ├── include/
    │   ├── grammar.hpp
    │   ├── parser.hpp
    │   ├── lambda.hpp
    │   ├── unit.hpp
    │   ├── useless.hpp
    │   ├── cnf.hpp
    │   ├── greibach.hpp
    │   ├── logger.hpp
    │
    ├── src/
    │   ├── main.cpp
    │   ├── parser.cpp
    │   ├── lambda.cpp
    │   ├── unit.cpp
    │   ├── useless.cpp
    │   ├── cnf.cpp
    │   ├── greibach.cpp
    │   ├── logger.cpp
    │   ├── entrada.txt
    │   └── teste
    │
    └── README.md

------------------------------------------------------------------------

## 📄 Como compilar

``` bash
cd src
g++ -std=c++17 main.cpp parser.cpp logger.cpp lambda.cpp unit.cpp useless.cpp cnf.cpp greibach.cpp -I../include -o teste
```

------------------------------------------------------------------------

## ▶️ Como executar

``` bash
./teste
```

------------------------------------------------------------------------

## ✏️ Formato da entrada (entrada.txt)

    S -> AB | aA | &
    A -> aA | B | a
    B -> bB | C | b
    C -> aC | D
    D -> &

------------------------------------------------------------------------

## 📘 Exemplo de saída (trecho do log)

    =======================================
     REMOCAO DE REGRAS-LAMBDA
    =======================================

    Variáveis anuláveis: A B C D S
    Produções após remoção:
    A -> aA | a | B
    B -> bB | b | C
    S -> AB | B | A | aA | a | &

------------------------------------------------------------------------

## 👨‍💻 Autor

**Sergio Moreira**
**Iure Morais**
------------------------------------------------------------------------

