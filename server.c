#include "mytalk.h"

/* establishes socket as a server, accpets connection from client, 
   communicates with client */
void make_server(int opts[3], int port) {
  int mlen, sockfd, newsock, i;
  struct sockaddr_in sa, newsockinfo, peerinfo;
  socklen_t len;
  char userin[4];
  char buff[MAXLEN + 1];
  struct hostent *hostent;
  struct in_addr adr;
  char localaddr[INET_ADDRSTRLEN], peeraddr[INET_ADDRSTRLEN];
  /*create socket */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  /* bind */
  if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  /* listen */
  if (listen(sockfd, DEFAULT_BACKLOG) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  /* accept */
  len = sizeof(peerinfo);
  if ((newsock = accept(sockfd, (struct sockaddr *)&peerinfo, &len)) == -1) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  len = sizeof(newsockinfo);
  if ((getsockname(newsock, (struct sockaddr *)&newsockinfo, &len)) == -1) {
    perror("getsockname");
    exit(EXIT_FAILURE);
  }
  /* get info on sockets */
  if (inet_ntop(AF_INET,&newsockinfo.sin_addr.s_addr,localaddr,
		sizeof(localaddr)) == NULL) {
    perror("inet_ntop");
    exit(EXIT_FAILURE);
  }
  if (inet_ntop(AF_INET, &peerinfo.sin_addr.s_addr,peeraddr,
		sizeof(peeraddr)) == NULL) {
    perror("inet_ntop");
    exit(EXIT_FAILURE);
  }
  /* verbosity */
  if (opts[0] > 0) {
    printf("newsock:  %d\n", newsock);
    if (opts[0] > 1) {
      printf("sa address: %d\n", sa.sin_addr.s_addr);
      printf("localaddr: %s\n", localaddr);
      printf("peeraddr: %s\n", peeraddr);
    }
  }
  /* prompt user to accept Mytalk request */
  if (opts[1] == 0) {
    /* get user name */
    if ((mlen = recv(newsock, buff, sizeof(buff), 0)) == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    /* null terminate */
    buff[mlen] = '\0';
    if (inet_aton(peeraddr, &adr) == 0) {
      perror("inet_aton");
      exit(EXIT_FAILURE);
    }
    if ((hostent = gethostbyaddr(&adr, sizeof(adr), AF_INET)) == NULL) {
      perror("gethostbyaddr");
      exit(EXIT_FAILURE);
    }
    printf("Mytalk request from %s@%s.  Accept (y/n)? ",buff,hostent->h_name);
    fflush(stdout);
    scanf("%3s", userin);
    for (i = 0; i < strlen(userin); i++) {
      userin[i] = tolower((unsigned char)userin[i]);
    }
    if (strcmp(userin, "y") != 0 && strcmp(userin, "yes") != 0) {
      exit(EXIT_FAILURE);
    }
    /* send ok */
    strcpy(buff, "ok");
    if ((mlen = send(newsock, buff, sizeof(buff), 0)) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
    }
    /* verbosity */
    if (opts[0] > 1) {
      printf("Message send to client: ok");
    }
  }
  /* create window unless option N is enabled*/
  if (opts[1] == 0) {
    start_windowing();
  }
  /* set verbosity */
  set_verbosity(opts[0]);
  /* chat */
  talk(newsock);
  /* close window unless N option is enabled */
  if (opts[1] == 0) {
    stop_windowing();
  }
  
  close(sockfd);
  close(newsock);
}
