CFLAGS = -Wall

.PHONY: all testar passos limpar

all: parser

# o cminus.tab.h precisa existir antes de compilar o lexico
codigo/cminus.tab.c codigo/cminus.tab.h: codigo/cminus.y
	bison -d -v -o codigo/cminus.tab.c codigo/cminus.y

codigo/lex.yy.c: codigo/cminus.l codigo/cminus.tab.h
	flex -o $@ codigo/cminus.l

parser: codigo/main.c codigo/util.c codigo/cminus.tab.c codigo/lex.yy.c codigo/globals.h codigo/util.h codigo/scan.h codigo/parse.h
	$(CC) $(CFLAGS) -o $@ codigo/main.c codigo/util.c codigo/cminus.tab.c codigo/lex.yy.c

# roda os testes, grava as saidas e confere o retorno (0 valido, 1 invalido)
testar: parser
	@falhas=0; \
	for arquivo in testes/validos/*.cm; do \
		nome=`basename $$arquivo .cm`; \
		./parser $$arquivo > saidas/$$nome.txt; \
		if [ $$? -ne 0 ]; then echo "FALHOU: $$nome devia ser valido"; falhas=1; else echo "valido    $$arquivo"; fi; \
	done; \
	for arquivo in testes/invalidos/*.cm; do \
		nome=`basename $$arquivo .cm`; \
		./parser $$arquivo > saidas/$$nome.txt; \
		if [ $$? -ne 1 ]; then echo "FALHOU: $$nome devia ser invalido"; falhas=1; else echo "invalido  $$arquivo"; fi; \
	done; \
	if [ $$falhas -eq 0 ]; then echo "todos os testes passaram"; else exit 1; fi

# roda os exemplos com as etapas no console
passos: parser
	@for arquivo in testes/validos/*.cm testes/invalidos/*.cm; do \
		./parser --passos $$arquivo; \
		echo; \
	done

limpar:
	rm -f parser
