CC = gcc
CFLAGS = -I./src -std=gnu99 -g

LEX ?= flex
YACC ?= bison
YFLAGS_BISON = -v -d -o obj/y.tab.c
YFLAGS_YACC = -v -d -b obj/y
GRAMMAR = parser.y

obj/mcc: obj/lex.yy.o obj/y.tab.o obj/tree.o obj/driver.o obj/strtab.o obj/codegen.o | obj
	$(CC) $(CFLAGS) -o $@ $^ -lfl

obj/y.tab.o: obj/y.tab.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/lex.yy.o: obj/lex.yy.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/y.tab.h obj/y.tab.c: src/$(GRAMMAR) src/tree.h src/strtab.h | obj
	@if command -v $(YACC) >/dev/null 2>&1; then \
		if [ "$(YACC)" = "bison" ] || $(YACC) --version 2>/dev/null | grep -qi bison; then \
			$(YACC) $(YFLAGS_BISON) $<; \
		else \
			$(YACC) $(YFLAGS_YACC) $<; \
		fi; \
	else \
		echo "error: parser generator not found (set YACC=bison or YACC=yacc)"; \
		exit 1; \
	fi

obj/lex.yy.c: src/scanner.l obj/y.tab.h | obj
	$(LEX) -o $@ $<

obj/tree.o: src/tree.c src/tree.h | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/driver.o: src/driver.c src/tree.h src/strtab.h obj/y.tab.h | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/strtab.o: src/strtab.c src/strtab.h obj/y.tab.h | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/codegen.o: src/codegen.c src/tree.h src/strtab.h | obj
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean obj

obj:
	@mkdir -p obj

clean:
	@rm -rf obj
	@rm -f lex.yy.* *.o *~ scanner
