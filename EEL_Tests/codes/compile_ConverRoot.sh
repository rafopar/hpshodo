#g++ ConverRoot.cc -o ConverRoot.exe `root-config --cflags --libs` -I$CODA/Linux-x86_64/include -L$CODA/Linux-x86_64/lib -levioxx -levio -lexpat -lpthread
g++ -std=c++11 ConverRoot.cc -o ConverRoot.exe `root-config --cflags --libs` -I$evio5p1/src/libsrc -I$evio5p1/src/libsrc++ -L$evio5p1/src/libsrc++/.Linux-x86_64 -levioxx  -L$evio5p1/src/libsrc/.Linux-x86_64 -levio -lexpat -lpthread
