
http: src/main.c src/cgi.c src/constant.h src/handle_request.h src/request_func.h src/response_func.h src/start_server.h
	gcc -o app src/main.c -lpthread 

clean: 
	rm app

run: 
	./app