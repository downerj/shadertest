.PHONY: clean pre

BINDIR = bin
OBJDIR = obj
PREDIR = pre
BIN = shadertest
WARNS = -Wall -Wextra
DEBUGS = 
LIBS = -lGL -lGLEW -lglfw

release: DEBUGS = -O3
release: $(BINDIR)/$(BIN)

debug: DEBUGS = -DDEBUG -Og -g
debug: $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN): $(OBJDIR)/main.oo
	mkdir -p $(BINDIR)
	$(CXX) -o $@ $^ $(LIBS)

$(OBJDIR)/main.oo: main.cc
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS)

pre: $(PREDIR)/main.ii

$(PREDIR)/main.ii: main.cc
	mkdir -p $(PREDIR)
	$(CXX) -E -o $@ $< $(WARNS) $(DEBUGS)

clean:
	rm -f $(BINDIR)/$(BIN)
	rm -f $(OBJDIR)/*.oo
