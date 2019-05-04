all:
	g++ main.cpp src/*.cpp -I include/ -o rasm
	./rasm -n test.s -o test
	hexdump -C test
