FROM debian:bullseye-slim

RUN apt-get update && apt-get install -y \
	make \
	clang \
	valgrind

WORKDIR /webserv/

COPY . /webserv/

RUN make -re -j

CMD ["valgrind", "--track-fds=yes", "--leak-check=full", "--show-leak-kinds=all", "./bin/webserv"]