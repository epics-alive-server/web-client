

LOCAL_CFLAGS = 
LOCAL_LDFLAGS = -laliveclient -ldcgi
ifdef AliveClient_Library_Dir
  LOCAL_LDFLAGS += -L$(AliveClient_Library_Dir)
endif
ifdef AliveClient_Include_Dir
  LOCAL_CFLAGS += -I$(AliveClient_Include_Dir)
endif
ifdef DCGI_Library_Dir
  LOCAL_LDFLAGS += -L$(DCGI_Library_Dir)
endif
ifdef DCGI_Include_Dir
  LOCAL_CFLAGS += -I$(DCGI_Include_Dir)
endif

DEFINITIONS =
ifdef Server
  DEFINITIONS += -DSERVER=\"$(Server)\"
endif
ifdef DB_Port
  DEFINITIONS += -DDB_PORT=$(DB_Port)
endif


.PHONY: all clean

all: ioc_alive.cgi

ioc_alive.cgi: ioc_alive.o
	gcc -Wall -o ioc_alive.cgi ioc_alive.o $(LOCAL_LDFLAGS)

ioc_alive.o: ioc_alive.c ioc_alive.h
	gcc -O2 -Wall -c ioc_alive.c $(DEFINITIONS) $(LOCAL_CFLAGS)

clean:
	-rm ioc_alive.cgi *.o
