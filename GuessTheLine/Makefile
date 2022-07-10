LIBS=-lsfml-graphics -lsfml-window -lsfml-system
CXX := g++

all: sfml-app

%.o: %.cpp
	$(CXX) -c $< -o $@

%.o: %.h
	$(CXX) -c $< -o $@

sfml-app: main.o
	$(CXX) -o $@ $? $(LIBS)

clean:
	rm -rf sfml-app *.o
