
output: emulator.o
	gcc ./emulator.o -shared -o ./emulator.so
	cp ./emulator.so ../lib/

emulator.o: emulator.c ../common/include/emulator.h
	gcc -c -fPIC ./emulator.c -o ./emulator.o

clean: 
	rm -rf *.o *.so
