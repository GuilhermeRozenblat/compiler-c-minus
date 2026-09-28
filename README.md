# Etapa 1

# Analisador Léxico C-Minus

Integrantes: Guilherme Rozenblat e Leonardo Valladares

Scanner manual para o subconjunto e scanner Flex para a linguagem completa.
Gramática de referência em [`bnf.txt`](etapa1-analisador-lexico/bnf.txt). 
Relatório: [Analisador Léxico C-Minus](Analisador%20L%C3%A9xico%20C-Minus.pdf).

## Comandos rápidos

```sh
cd etapa1-analisador-lexico
make
./scanner_manual testes/subconjunto/valido1.cm
./scanner_flex testes/completo/gcd.cm
make comparar
make completo
make limpar
```
