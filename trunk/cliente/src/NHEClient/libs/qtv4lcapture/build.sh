export LD_LIBRARY_PATH=.
make distclean
cd videolibi
make distclean
qmake
make
cd ..
qmake
make
./qtv4lCapture
