#ifndef INCLUDES_HPP
#define INCLUDES _HPP

// Library includes goes here
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <cmath>
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <list>
#include <stack>
#include <iterator>
#include <map>
#include <stack>
#include <deque>
#include <utility>
#include <ctime>
#include <climits>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <linux/limits.h>
#include <fcntl.h>

// Other common includes
#define BOLD "\033[1m"
#define ITALIC "\033[3m"
#define UNDERLINE "\033[4m"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

extern int max_fd;

extern fd_set currentfds;
extern fd_set writefds;
extern fd_set readfds;

extern std::map<int, bool> sock_fd;

#endif