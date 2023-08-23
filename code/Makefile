all: hide unhide

hide: hide.cpp
	g++ -std=c++17 -Wall -o hide hide.cpp -lfreeimage
unhide: unhide.cpp
	g++ -std=c++17 -Wall -o unhide unhide.cpp -lfreeimage

clean:
	rm hide
	rm unhide