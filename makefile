all:
	g++ -fpermissive -std=c++11 -O3 main.cpp ./DB/MurmurHash.cpp -o main.out -lpthread
	./main.out