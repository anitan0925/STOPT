COMPILER=clang++
INCLUDE=-I../../include
CFLAGS=-O -std=c++11

compile:
	$(COMPILER) $(CFLAGS) $(INCLUDE) amsvrg.cpp -o amsvrg
	$(COMPILER) $(CFLAGS) $(INCLUDE) sgd.cpp -o sgd
	$(COMPILER) $(CFLAGS) $(INCLUDE) test.cpp -o test

all: compile get_mnist test_sgd test_amsvrg

clean:
	- rm progress_amsvrg.txt
	- rm weight_amsvrg.txt
	- rm progress_sgd.txt
	- rm weight_sgd.txt
	- rm mnist.scale.t
	- rm mnist.scale
	- rm mnist.scale.t.bz2
	- rm mnist.scale.bz2
	- rm test
	- rm sgd
	- rm amsvrg

get_mnist:
	python get_mnist.py

test_sgd:
	./sgd -f ./configures/sgd_opt.json
	./test -f ./configures/test.json -w weight_sgd.txt

test_amsvrg:
	./amsvrg -f ./configures/amsvrg_opt.json
	./test -f ./configures/test.json -w weight_amsvrg.txt
