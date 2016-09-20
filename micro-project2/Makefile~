cc=gcc
cflags= -Wall -std=c99
ftmarco= -fno-stack-protector
debug = -g
part1:
	$(cc) -o gdb_example1 $(cflags) gdb_example1.c $(debug)

part2:
	$(cc) -o gdb_example2 $(cflags) $(ftmarco) gdb_example2.c $(debug)

part2-mem-protection:
	$(cc) -o gdb_example2 $(cflags) gdb_example2.c $(debug)
.PHONY: clean
clean:
	rm -f gdb_example1 gdb_example2

submit:
	tar cvf micro_project2.tar.gz gdb_example1.c gdb_example2.c buggyBST.c micro-project2.txt Makefile

