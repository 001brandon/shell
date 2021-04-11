CC=gcc
# CC=gcc -Wall

mysh: get_path.o which.o shell-with-builtin.o cd.o list.o linked.o watchuser.o linked_char.o
	$(CC) -g -pthread shell-with-builtin.c get_path.o which.o cd.o list.o linked.o watchuser.o linked_char.o -o mysh

shell-with-builtin.o: shell-with-builtin.c  sh.h 
	$(CC) -g -c shell-with-builtin.c

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

which.o: which.c get_path.h
	$(CC) -g -c which.c

cd.o: cd.c sh.h
	$(CC) -g -c cd.c

list.o: list.c sh.h
	$(CC) -g -c list.c

linked.o: linked.c linked.h
	$(CC) -g -c linked.c

watchuser.o: watchuser.c watchuser.h
	$(CC) -g -c watchuser.c

linked_char.o: linked_char.c linked_char.h
	$(CC) -g -c linked_char.c

clean:
	rm -rf shell-with-builtin.o get_path.o which.o cd.o list.o watchuser.o linked.o linked_char.o mysh
