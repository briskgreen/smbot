LIBS=main.o config.o command.o smbot.o taskfactory.o mysock.o

smbot:$(LIBS)
	gcc -o smbot $(LIBS) -lssl -lpthread

main.o:config.h main.c
	gcc -c main.c

config.o:command.h config.c
	gcc -c config.c

command.o:smbot.h command.c
	gcc -c command.c

smbot.o:mysock/mysock.h smbot.c
	gcc -c smbot.c

taskfactory.o:TaskFactory/taskfactory.h TaskFactory/taskfactory.c 
	gcc -c TaskFactory/taskfactory.c

mysock.o:mysock/mysock.h mysock/mysock.c 
	gcc -c mysock/mysock.c

clean:
	rm smbot *.o
