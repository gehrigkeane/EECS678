STUDENT_ID=2727430

all: clean lab8 test

lab8: lab8.c
	gcc -g -o lab8 lab8.c

test: lab8
	./lab8

tar: clean
	mkdir $(STUDENT_ID)-qsort-lab
	cp lab8.c Makefile $(STUDENT_ID)-qsort-lab/
	tar cvzf $(STUDENT_ID)-qsort-lab.tar.gz $(STUDENT_ID)-qsort-lab
	rm -rf $(STUDENT_ID)-qsort-lab

clean:
	rm -rf lab8