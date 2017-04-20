nothing :

windows : 
	mkdir -p build
	cd build && cmake -G "NMake Makefiles" .. && nmake

unix :  
	mkdir -p build
	cd build && CC=clang cmake .. && make
