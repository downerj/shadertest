.PHONY: clean

INCDIR = include
HEADERS = $(wildcard $(INCDIR)/*.hh)
BINDIR = bin
OBJDIR = obj
PREDIR = out/pre
ASMDIR = out/asm
BIN = shadertest
WARNS = -Wall -Wextra -Werror -Wpedantic -pedantic-errors
DEBUGS =
LIBS = -lGL -lGLEW -lglfw

release: DEBUGS = -O3
release: $(BINDIR)/$(BIN)

debug: DEBUGS = -DDEBUG -Og -g
debug: $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN): $(OBJDIR)/main.oo
	mkdir -p $(BINDIR)
	$(CXX) -o $@ $^ $(LIBS)

$(OBJDIR)/main.oo: main.cc $(HEADERS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS)

pre: $(PREDIR)/main.ii

$(PREDIR)/main.ii: main.cc $(HEADERS)
	mkdir -p $(PREDIR)
	$(CXX) -E -o $@ $< $(WARNS) $(DEBUGS)

asm: $(ASMDIR)/main.s

$(ASMDIR)/main.s: main.cc $(HEADERS)
	mkdir -p $(ASMDIR)
	$(CXX) -S -o $@ $< $(WARNS) $(DEBUGS)

clean:
	rm -f $(BINDIR)/$(BIN)
	rm -f $(OBJDIR)/*.oo
	rm -f $(PREDIR)/*.ii
	rm -f $(ASMDIR)/**.s
