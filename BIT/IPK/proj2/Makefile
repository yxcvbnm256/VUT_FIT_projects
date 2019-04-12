all:
	@g++ server.cpp -std=c++11 -Wall -Wextra -o ipkhttpserver
	@g++ client.cpp -std=c++11 -Wall -Wextra -o ipkhttpclient
clean:
	@rm -f *.log ipkhttpclient ipkhttpserver *.payload *.header *.gz *.err *.!!!
pack:
	@tar -czf xpawlu00.tgz server.cpp client.cpp test.sh Makefile README www
test:
	@make -s
	@sh test.sh

