CFLAGS = -Wall

.PHONY: all testar passos limpar

all: ll1

ll1: codigo/main.c codigo/scan.c codigo/parse.c codigo/util.c codigo/globals.h codigo/scan.h codigo/parse.h codigo/util.h
	$(CC) $(CFLAGS) -o $@ codigo/main.c codigo/scan.c codigo/parse.c codigo/util.c

# roda os testes, grava as saidas e confere o retorno (0 aceita, 1 rejeita)
testar: ll1
	@falhas=0; \
	for arquivo in testes/aceitas/*.txt; do \
		nome=`basename $$arquivo .txt`; \
		./ll1 $$arquivo > saidas/$$nome.txt; \
		if [ $$? -ne 0 ]; then echo "FALHOU: $$nome devia ser aceita"; falhas=1; else echo "aceita     $$arquivo"; fi; \
	done; \
	for arquivo in testes/rejeitadas/*.txt; do \
		nome=`basename $$arquivo .txt`; \
		./ll1 $$arquivo > saidas/$$nome.txt; \
		if [ $$? -ne 1 ]; then echo "FALHOU: $$nome devia ser rejeitada"; falhas=1; else echo "rejeitada  $$arquivo"; fi; \
	done; \
	if [ $$falhas -eq 0 ]; then echo "todos os testes passaram"; else exit 1; fi

# roda os exemplos com as etapas no console
passos: ll1
	@for arquivo in testes/aceitas/*.txt testes/rejeitadas/*.txt; do \
		echo "Arquivo: $$arquivo"; \
		echo; \
		./ll1 $$arquivo; \
		echo; \
	done

limpar:
	rm -f ll1
