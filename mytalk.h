/* header for mytalk.c */

#ifndef _MYTALK_
#define _MYTALK_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pwd.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <talk.h>
#include <poll.h>

#define DEFAULT_BACKLOG 1
#define DECIMAL 10
#define MAXLEN 1000
#define LOCAL 0
#define REMOTE (LOCAL + 1)

void make_client(int opts[3], char *hostname, int port);
void make_server(int opts[3], int port);
void talk(int sockfd);

#endif
