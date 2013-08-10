smbot:main.o smbot.o config.o taskfactory.o mysock.o
	gcc -o smbot main.o smbot.o config.o taskfactory.o mysock.o -lssl -lpthread

main.o:config.h main.c
	gcc -c main.c

smbot.o:smbot.h smbot.c
	gcc -c smbot.c

config.o:config.h config.c
	gcc -c config.c

taskfactory.o:TaskFactory/taskfactory.h TaskFactory/taskfactory.c 
	gcc -c TaskFactory/taskfactory.c 

mysock.o:mysock/mysock.h mysock/mysock.c 
	gcc -c mysock/mysock.c 

clean:
	rm *.o smbot
