
http: src/main.c src/cgi.c src/constant.h src/handle_request.h src/request_func.h src/response_func.h src/start_server.h
	gcc -lpthread -o app src/main.c

clean: 
	rm app

run: 
	./app