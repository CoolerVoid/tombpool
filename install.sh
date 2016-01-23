#!/usr/bin
echo "THis is another thread pool C library by Cooler_"
echo "---------------------------------------------------------"
echo "Tomb Pool - https://github.com/CoolerVoid/tombpool "
mkdir pack; cd pack
cmake ../; make; cd .. ; cp tombpool.h pack; cp test.c pack; cd pack
cp libtombpool.so /lib
cp libtombpool.so /usr/local/lib
cp libtombpool.so /usr/lib64/
cp libtombpool.so /usr/lib32/
cp libtombpool.so /usr/lib

echo "================================================"
echo "Test programm at pack/test\n "
echo " $ gcc -o test test.c -lpthread -ltombpool"
echo "------------------------------------------------"
gcc -o test test.c -lpthread -ltombpool
./test
echo "look the file \"test.c\" to you know how to use this library..."

