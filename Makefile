7cc: 7cc.c


test: 7cc
		./7cc -test
		./test.sh


clean:
		rm -f 7cc *.o *~ tmp*