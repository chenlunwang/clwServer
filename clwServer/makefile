target1=server
target2=client
basesrc=$(wildcard base/*.cc)
netsrc=$(wildcard net/*.cc)
server_src=server.cc 
client_src=client.cc
baseobj=$(patsubst base/%.cc,./obj/%.o,$(basesrc))
netobj=$(patsubst net/%.cc,./obj/%.o,$(netsrc))
sobj=$(patsubst %.cc,./obj/%.o,$(server_src))
cobj=$(patsubst %.cc,./obj/%.o,$(client_src))

all:$(target1) $(target2)

$(target2):$(cobj)
	g++ $^ -o $@ -Wall 

$(target1):$(baseobj) $(netobj) $(sobj)
	g++ $^ -o $@ -Wall -lpthread

$(baseobj):./obj/%.o:base/%.cc
	g++ -c $< -o $@ -Wall -lpthread

$(netobj):./obj/%.o:net/%.cc
	g++ -c $< -o $@ -Wall -lpthread

$(sobj):./obj/%.o:%.cc
	g++ -c $< -o $@ -Wall -lpthread 

$(cobj):./obj/%.o:%.cc   
	g++ -c $< -o $@ -Wall -lpthread 

.PHONY:clean
clean:
	-rm -rf $(target1) $(target2) $(baseobj) $(netobj) $(sobj) $(cobj)

.PHONY:logclean
logclean:
	-rm -rf log/*.log
