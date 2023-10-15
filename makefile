.PHONY: clean

SRCDIR = src
HEADERS = $(wildcard $(SRCDIR)/**/*.hh)
BINDIR = bin
OBJDIR = out/obj
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

$(OBJDIR)/main.oo: $(SRCDIR)/main.cc $(HEADERS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< -I$(SRCDIR) $(WARNS) $(DEBUGS)

clean:
	rm -f $(BINDIR)/$(BIN)
	rm -f $(OBJDIR)/*.oo
