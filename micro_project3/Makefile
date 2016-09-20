CC     = gcc
CFLAGS = -g -Wall -pedantic
LIBS   = -lpthread
ALL    = m3
SUBMIT = student.c Makefile shortlist longlist

m3:	student.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

submit:	clean
	tar zcvf m3-submit.tar.gz $(SUBMIT)

clean:
	rm -f m3 *.o
