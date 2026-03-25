CC:=gcc
#CFLAGS:=-I./src -std=gnu99
CFLAGS:=-I./src -std=gnu99 -O0 -ggdb3 -Wall

#LEX:=lex
#LEXLIB:=-ll
LEX:=flex
LEXLIB:=-lfl

OBJDIR:=obj

YACC:=yacc
YFLAGS:=-v -d -b $(OBJDIR)/y
GRAMMAR:=src/parser.y

GRAMMAROBJS:=$(OBJDIR)/lex.yy.o $(OBJDIR)/y.tab.o
OBJS:=$(GRAMMAROBJS) $(OBJDIR)/tree.o $(OBJDIR)/driver.o $(OBJDIR)/strtab.o

BIN:=$(OBJDIR)/mcc

all: $(BIN)

.PHONY: clean test

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LEXLIB)

$(OBJDIR)/y.tab.o: $(OBJDIR)/y.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/lex.yy.o: $(OBJDIR)/lex.yy.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/y.tab.h $(OBJDIR)/y.tab.c: $(GRAMMAR) src/tree.h
	mkdir -p $(OBJDIR)
	$(YACC) $(YFLAGS) $<

$(OBJDIR)/lex.yy.c: src/scanner.l $(OBJDIR)/y.tab.h
	$(LEX) -o $@ $<

$(OBJDIR)/tree.o: src/tree.c src/tree.h
	$(CC)  $(CFLAGS) -c $< -o $@

$(OBJDIR)/driver.o: src/driver.c src/tree.h $(OBJDIR)/y.tab.h
	$(CC)  $(CFLAGS) -c $< -o $@

$(OBJDIR)/strtab.o: src/strtab.c src/strtab.h $(OBJDIR)/y.tab.h
	$(CC)  $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)

test: test/testOutput.py $(BIN)
	python3 $< \
		--testpath test/cases --testext .mC \
		--exppath test/exp --expext .exp \
		--program $(BIN)
