# Etapa 1

# Analisador Léxico C-Minus

Integrantes: Guilherme Rozenblat e Leonardo Valladares

Scanner manual para o subconjunto e scanner Flex para a linguagem completa.
Gramática de referência em [`bnf.txt`](etapa1-analisador-lexico/bnf.txt).

## Comandos rápidos

```sh
make
./scanner_manual testes/subconjunto/valido1.cm
./scanner_flex testes/completo/gcd.cm
make comparar
make completo
make limpar
```
