
#initial Makefile for this example.   much packaging remains to be done.

all:
	(cd KernelServer; make)
	(cd User; make)


clean:
	(cd KernelServer; make clean)
	(cd User; make clean)
