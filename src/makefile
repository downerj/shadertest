.PHONY: clean

BINDIR = ../bin
OBJDIR = ../obj
BINARY = shadertest
WARNS = -Wall -Wextra -Werror -Wpedantic -pedantic-errors -std=c++20
DEBUGS =
LIBS = -lGL -lGLEW -lglfw

release: DEBUGS = -O3
release: $(BINDIR)/$(BINARY)

debug: DEBUGS = -DDEBUG -Og -g
debug: $(BINDIR)/$(BINARY)

$(BINDIR)/$(BINARY): $(OBJDIR)/main.o $(OBJDIR)/core.o $(OBJDIR)/runtime.o $(OBJDIR)/window.o
	mkdir -p $(BINDIR)
	$(CXX) -o $@ $^ $(LIBS)

$(OBJDIR)/main.o: main.cc compatibility.hh configurations.hh core.hh runtime.hh window.hh
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS)

$(OBJDIR)/core.o: core.cc core.hh configurations.hh gl-includes.hh
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS)

$(OBJDIR)/runtime.o: runtime.cc runtime.hh configurations.hh core.hh geometry.hh gl-includes.hh
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS)

$(OBJDIR)/window.o: window.cc window.hh compatibility.hh configurations.hh gl-includes.hh
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS)

clean:
	rm -f $(BINDIR)/$(BINARY)
	rm -f $(OBJDIR)/*.o
