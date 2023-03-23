main:main.cpp
	g++ main.cpp -o main -lpthread


.PHONY: clean
clean:
	rm -f *.o $(TARGET)