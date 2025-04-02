test:Main.cc
	g++ -std=c++11 -o $@ $^ -lgtest -lpthread
.PHONEY:clean
clean:
	rm -rf test