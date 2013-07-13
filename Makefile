LIBS=main.o dict.o get_time.o get_man.o query_ip.o smbot.o torrent.o youku.o bt.o yb.o weather.o mysock.o

smbot:$(LIBS)
	gcc -o smbot $(LIBS)

main.o:main.c smbot.h
	gcc -c main.c

dict.o:dict.c dict.h mysock/mysock.h 
	gcc -c dict.c

get_time.o:get_time.c get_time.h
	gcc -c get_time.c

get_man.o:get_man.c get_man.h mysock/mysock.h 
	gcc -c get_man.c

query_ip.o:query_ip.c query_ip.h
	gcc -c query_ip.c

smbot.o:smbot.c smbot.h mysock/mysock.h 
	gcc -c smbot.c

torrent.o:torrent.c torrent.h mysock/mysock.h 
	gcc -c torrent.c

youku.o:youku.c youku.h mysock/mysock.h 
	gcc -c youku.c

bt.o:bt.c bt.h
	gcc -c bt.c

yb.o:yb.c yb.h
	gcc -c yb.c

weather.o:weather.c weather.h mysock/mysock.h 
	gcc -c weather.c

mysock.o:mysock/mysock.c mysock/mysock.h 
	gcc -c mysock/mysock.c 

clean:
	rm smbot *.o
