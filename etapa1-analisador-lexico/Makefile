CFLAGS = -Wall

.PHONY: all comparar completo passos limpar

all: scanner_manual scanner_flex

scanner_manual: codigo/main.c codigo/util.c codigo/scan.c codigo/globals.h codigo/util.h codigo/scan.h
	$(CC) $(CFLAGS) -o $@ codigo/main.c codigo/util.c codigo/scan.c

codigo/lex.yy.c: codigo/cminus.l
	flex -o $@ $<

scanner_flex: codigo/main.c codigo/util.c codigo/lex.yy.c codigo/globals.h codigo/util.h codigo/scan.h
	$(CC) $(CFLAGS) -o $@ codigo/main.c codigo/util.c codigo/lex.yy.c

comparar: all
	@set -e; for arquivo in testes/subconjunto/*.cm; do \
		nome=`basename $$arquivo .cm`; \
		./scanner_manual $$arquivo > saidas/$$nome.manual.txt; \
		./scanner_flex $$arquivo > saidas/$$nome.flex.txt; \
		echo "$$nome:"; \
		diff saidas/$$nome.manual.txt saidas/$$nome.flex.txt; \
	done

completo: scanner_flex
	@set -e; for arquivo in testes/completo/*.cm; do \
		nome=`basename $$arquivo .cm`; \
		./scanner_flex $$arquivo > saidas/$$nome.flex.txt; \
	done

# roda os exemplos com as etapas no console
passos: all
	@for arquivo in testes/subconjunto/*.cm; do \
		echo "Scanner manual"; \
		./scanner_manual --passos $$arquivo; \
		echo; \
	done
	@for arquivo in testes/completo/*.cm; do \
		echo "Scanner do Flex"; \
		./scanner_flex --passos $$arquivo; \
		echo; \
	done

limpar:
	rm -f scanner_manual scanner_flex
