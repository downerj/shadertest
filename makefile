.PHONY: clean

BIN = shadertest
WARNS = -Wall -Wextra
LIBS = -lGL -lGLEW -lglfw

$(BIN): main.oo
	$(CXX) -o $@ $^ $(LIBS)

main.oo: main.cc
	$(CXX) -c -g -o $@ $< $(WARNS)

clean:
	rm -f $(BIN)
	rm -f *.oo

