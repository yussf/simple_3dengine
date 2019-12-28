all : clean main run
main:
	g++ main.cpp -w -lSDL2 -o main
run:
	./main 0
clean:
	rm -f main