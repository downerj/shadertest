.PHONY: clean

BIN = shadertest
WARNS = -Wall -Wextra
DEBUGS = 
LIBS = -lGL -lGLEW -lglfw

release: DEBUGS = -O3
release: $(BIN)

debug: DEBUGS = -DDEBUG -Og -g
debug: $(BIN)

$(BIN): main.oo
	$(CXX) -o $@ $^ $(LIBS)

main.oo: main.cc
	$(CXX) -c -g -o $@ $< $(WARNS) $(DEBUGS)

clean:
	rm -f $(BIN)
	rm -f *.oo
