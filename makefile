# To "make"       run in terminal: "make client" or "make server"
# To "make clean" run in terminal: "make client_clean" or "make server_clean"
client:
	make -f makefile.client
client_clean:
	make clean -f makefile.client
server:
	make -f makefile.server
server_clean:
	make clean -f makefile.server