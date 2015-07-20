all:
	cd lib;\
	make;\
	cd ../src;\
	make
dbg:
	cd lib;\
	make;\
	cd ../src;\
	make dbg
clean:
	cd lib;\
	rm *.o;\
	cd ../src;\
	rm *.o;\
	cd ../bin;\
	rm *;

cleanlib:
	cd src/cudd;\
	make distclean
