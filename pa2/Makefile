all: dfs dfc

dfc: dfc.c
	gcc -Wall dfc.c -o dfc -lcrypto -lssl

dfs: dfs.c
	gcc -Wall dfs.c -o dfs -lcrypto -lssl

.PHONY: clean

clean:
	rm -f dfs
	rm -f dfc