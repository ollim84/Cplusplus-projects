rm nsfis.py nsfis.pyc *.o *.so
swig -c++ -python src/nsfis.i
g++ -fPIC -c -Iinclude src/nsfis.cpp -Llib -lfuzzylite
g++ -fPIC -c -Iinclude src/fisAP.cpp -Llib -lfuzzylite
g++ -fPIC -c -Iinclude src/fisMN.cpp -Llib -lfuzzylite
g++ -fPIC -c -Iinclude src/fisNS.cpp -Llib -lfuzzylite
g++ -fPIC -c -Iinclude -I/usr/include/python2.6 src/nsfis_wrap.cxx
g++ -shared -Wl,-no-undefined -Wl,--warn-once -lpython2.6 nsfis.o fisAP.o fisMN.o fisNS.o nsfis_wrap.o -Llib -lfuzzylite -o _nsfis.so
mv src/nsfis.py nsfis.py
rm *.o
