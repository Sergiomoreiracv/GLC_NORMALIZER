#  GLC_NORMALIZER  
### Ferramenta para Normalização de Gramáticas Livres de Contexto (GLC)  
**Suporte completo para:**
✔ Remoção de regras-λ (lambda) 
✔ Remoção de produções unitárias 
✔ Remoção de símbolos inúteis 
✔ Conversão para Forma Normal de Chomsky (FNC / CNF) 
✔ Conversão para Forma Normal de Greibach (FNG / GNF) 
✔ Geração de log com todas as gramáticas intermediárias 

---

## 📘 Sobre o Projeto

Este projeto implementa uma ferramenta completa em **C++** para normalização de Gramáticas Livres de Contexto (GLC).  
Ele foi desenvolvido seguindo os requisitos acadêmicos da disciplina de Linguagens Formais e Autômatos, permitindo que o usuário:

- Forneça uma gramática a partir de um arquivo `.txt`
- Escolha o tipo de normalização desejada
- Receba como saída um arquivo de log detalhado contendo **cada etapa da normalização**

O programa está modularizado em arquivos independentes:

- `lambda.cpp` → remoção de regras-λ 
- `unit.cpp` → remoção de produções unitárias 
- `useless.cpp` → remoção de símbolos inúteis 
- `cnf.cpp` → conversão para Forma Normal de Chomsky 
- `greibach.cpp` → conversão para Forma Normal de Greibach 
- `parser.cpp` → interpretação da gramática de entrada 
- `logger.cpp` → geração do log passo a passo 
- `main.cpp` → interface principal de interação com o usuário 

---

## 📄 Formato do arquivo de entrada

A gramática pode ser inserida em **forma completa ou reduzida**
O símbolo `&` representa a palavra vazia (lambda).

---

## 📤 Saída (arquivo de log)

O programa gera automaticamente um arquivo:

Nele constam:

- A gramática inicial
- A identificação e remoção de regras-λ
- A nova gramática gerada após cada passo
- Remoção das unitárias
- Remoção dos símbolos inúteis
- Conversão final (FNC ou FNG)

Cada etapa é cuidadosamente documentada de forma legível.

---

## ▶️ Como compilar

No diretório `src`, execute:

g++ -std=c++17 main.cpp parser.cpp logger.cpp lambda.cpp unit.cpp useless.cpp cnf.cpp greibach.cpp -I../include -o glc_normalizer

Estrutura do Projeto
/GLC_NORMALIZER
│
├── include/
│   ├── grammar.hpp
│   ├── logger.hpp
│   ├── parser.hpp
│   ├── lambda.hpp
│   ├── unit.hpp
│   ├── useless.hpp
│   ├── cnf.hpp
│   ├── greibach.hpp
│
├── src/
│   ├── main.cpp
│   ├── parser.cpp
│   ├── logger.cpp
│   ├── lambda.cpp
│   ├── unit.cpp
│   ├── useless.cpp
│   ├── cnf.cpp
│   ├── greibach.cpp
│
└── README.md

Autores: 
Sérgio Moreira 
Iure 

