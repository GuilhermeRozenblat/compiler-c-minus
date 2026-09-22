CFLAGS = -Wall

.PHONY: all comparar completo limpar

all: scanner_manual scanner_flex

scanner_manual: codigo/principal.c codigo/scanner_manual.c codigo/scanner.h
	$(CC) $(CFLAGS) -o $@ codigo/principal.c codigo/scanner_manual.c

codigo/lex.yy.c: codigo/cminus.l
	flex -o $@ $<

scanner_flex: codigo/principal.c codigo/lex.yy.c codigo/scanner.h
	$(CC) $(CFLAGS) -o $@ codigo/principal.c codigo/lex.yy.c

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

limpar:
	rm -f scanner_manual scanner_flex
