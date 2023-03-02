.PHONY: clean

BINDIR = bin
OBJDIR = obj
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
	$(CXX) -c -g -o $@ $< $(WARNS) $(DEBUGS)

clean:
	rm -f $(BINDIR)/$(BIN)
	rm -f $(OBJDIR)/*.oo
