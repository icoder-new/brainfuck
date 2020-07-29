all:
	g++ -Wall interpreter.cpp -o bf
	gcc -Wall bf2c.c -o bf2c

clean:
	rm -rf bf bf2c