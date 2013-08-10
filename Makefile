LIBS=main.o dict.o get_time.o get_man.o query_ip.o smbot.o torrent.o \
youku.o bt.o yb.o weather.o stack.o id.o checkid.o url.o \
joke.o dream.o song.o bing.o google.o image.o mysock.o

smbot:$(LIBS)
	gcc -o smbot $(LIBS) -lssl

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

stack.o:stack.h stack.c
	gcc -c stack.c

id.o:id.h id.c
	gcc -c id.c

check.o:checkid.h checkid.c
	gcc -c checkid.c

url.o:url.h url.c mysock/mysock.h 
	gcc -c url.c

joke.o:joke.h joke.c
	gcc -c joke.c

dream.o:dream.h dream.c
	gcc -c dream.c

song.o:song.h song.c
	gcc -c song.c

bing.o:bing.h bing.c
	gcc -c bing.c

google:google.h google.c
	gcc -c google.c

image.o:image.h image.c
	gcc -c image.c

mysock.o:mysock/mysock.c mysock/mysock.h 
	gcc -c mysock/mysock.c

clean:
	rm smbot *.o
