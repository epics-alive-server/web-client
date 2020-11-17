

prefix         = /usr/local
cgi_bin_dir        = $(prefix)/cgi-bin


#### Set any of these if library or header not in default search path
#AliveClient_Library_Dir = /usr/local/lib
#AliveClient_Include_Dir = /usr/local/include
#DCGI_Library_Dir = /usr/local/lib
#DCGI_Include_Dir = /usr/local/include


#### Set either of these if you don't want to use either of the 
#### default values for the alive database address and port that
#### are built into the alive-client library
#Server = localhost
#DB_Port = 5679


# You can put local overrides in a separate file if you want
-include LocalOptions

#####################################################

export AliveClient_Library_Dir
export AliveClient_Include_Dir

export DCGI_Library_Dir
export DCGI_Include_Dir

export Server
export DB_Port


.PHONY : all clean install uninstall

all:
	make -C src all

clean:
	make -C src clean


INSTALL_MKDIR = mkdir -p
INSTALL_BIN   = install -c -m 0755
UNINSTALL_RM = -rm

install : all
	$(INSTALL_MKDIR) $(DESTDIR)$(cgi_bin_dir)
	$(INSTALL_BIN)   src/ioc_alive.cgi $(DESTDIR)$(cgi_bin_dir)/

uninstall :
	$(UNINSTALL_RM) $(DESTDIR)$(cgi_bin_dir)/ioc_alive.cgi

