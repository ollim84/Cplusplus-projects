# Makefile 
#

all : ap mn ns NSFIS 

ap:	
	g++ -c -Iinclude src/fisAP.cc
mn:
	g++ -c -Iinclude src/fisMN.cc
ns:
	g++ -c -Iinclude src/fisNS.cc
NSFIS: 
	g++ src/main.cc fisAP.o fisMN.o fisNS.o -Iinclude -Llib -lfuzzylite -o  nsfis
	rm -f *.o
libs:
	g++ -fPIC -shared -c -Iinclude src/nsfis.cc -o lib/libnsfis.so -Llib -lfuzzylite -lpython2.5
debug:
	g++ -DFL_USE_LOG -DFL_USE_DEBUG -DFL_USE_ASSERT -c -Iinclude src/fisAP.cpp
	g++ -DFL_USE_LOG -DFL_USE_DEBUG -DFL_USE_ASSERT -c -Iinclude src/fisMN.cpp
	g++ -DFL_USE_LOG -DFL_USE_DEBUG -DFL_USE_ASSERT -c -Iinclude src/fisNS.cpp
	g++ -DFL_USE_LOG -DFL_USE_DEBUG -DFL_USE_ASSERT src/main.cpp fisAP.o fisMN.o fisNS.o -Iinclude -Llib -lfuzzylite -o  nsfis-dbg
	rm -f *.o
clean:
	rm -f nsfis nsfis-dbg lib/libnsfis.so
