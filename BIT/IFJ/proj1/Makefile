# Projekt: Implementace interpretu imperativního jazyka IFJ14

NAME=projekt

OBJFILES=scanner.o globalvariables.o errors.o parserexpression.o main.o parser.o symboltable.o bst.o interpret.o substring.o rewritecode.o quicksort.o memoryallocation2.o

GENERATOR_NAME=generateruletable

RULE_TABLE=ruleTable.csv

OBJFILES2=generateruletable.o errors.o globalvariables.o memoryallocation2.o

OUTPUT_FILE=ruleTable.out

TEST_FILE=scannertest.txt

RUN_TEST_FILE=parsertest.pas

OBJFILES3=scanner.o globalvariables.o errors.o mainscannertest.o

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra -O2

# univerzální pravidlo pro generování všech objektových souborů
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# Startovací pravidlo
all: clear $(NAME)

# vytvori archiv, ktery bude odevzdan
archiv:
	zip xjanou15.zip *.c *.h Makefile dokumentace.pdf rozsireni rozdeleni

clean:
	rm -f *.o $(NAME) *.gch $(GENERATOR_NAME) $(OUTPUT_FILE)
clear:
	rm -f *.o $(NAME) *.gch $(GENERATOR_NAME) $(OUTPUT_FILE)

# Slinkování všech objektových souborů do jednoho spustitelného programu.
ifdef f
$(NAME):$(f)
# prelozi jenom jeden zdrojovy soubor do object file
	$(CC) $(CFLAGS) -O -c $(f)
else
$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES)
	chmod 777 $@
endif

generate: $(OBJFILES2) $(RULE_TABLE)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES2)
	./$@ $(RULE_TABLE) > $(OUTPUT_FILE)

scanner: $(OBJFILES3) $(TEST_FILE)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES3)
	./$@ $(TEST_FILE)

valgrind: $(OBJFILES) $(RUN_TEST_FILE)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES)
	chmod 777 $@
	valgrind ./$@ $(RUN_TEST_FILE)

test: clear $(OBJFILES) $(RUN_TEST_FILE)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES)
	chmod 777 $@
	./$@ $(RUN_TEST_FILE)


$(RUN_TEST_FILE):


