LIBS=main.o config.o command.o smbot.o taskfactory.o mysock.o

smbot:$(LIBS)
	gcc -o smbot $(LIBS) -lssl -lpthread -g

main.o:config.h main.c
	gcc -c main.c -g

config.o:command.h config.c
	gcc -c config.c -g

command.o:smbot.h command.c
	gcc -c command.c -g

smbot.o:mysock/mysock.h smbot.c
	gcc -c smbot.c -g

taskfactory.o:TaskFactory/taskfactory.h TaskFactory/taskfactory.c 
	gcc -c TaskFactory/taskfactory.c -g

mysock.o:mysock/mysock.h mysock/mysock.c 
	gcc -c mysock/mysock.c -g

clean:
	rm smbot *.o
