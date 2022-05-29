#include "mytalk.h"

/* establishes socket as a client, connects to server, communicates with 
   server */
void make_client(int opts[3], char *hostname, int port) {
  int sockfd, len;
  struct sockaddr_in sa;
  struct hostent *hostent;
  char buff[MAXLEN + 1];
  uid_t uid;
  struct passwd *pw;
  /* identity hostname */
  if ((hostent = gethostbyname(hostname)) == NULL) {
    herror("gethostbyname");
    exit(EXIT_FAILURE);
  }
  /* create socket */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  sa.sin_family = AF_INET;
  /*port passed into function from main */
  sa.sin_port = htons(port);
  /*set address to first host address */
  sa.sin_addr.s_addr = *(uint32_t*)hostent->h_addr_list[0];
  sa.sin_family = AF_INET;
  /* connect */
  if (connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  if (opts[1] == 0) {
  /* send username */
    uid = getuid();
    if ((pw = getpwuid(uid)) == NULL) {
      perror("invalid uid");
      exit(EXIT_FAILURE);
    }
    if (pw) {
      if (send(sockfd, pw->pw_name, sizeof(pw->pw_name), 0) == -1) {
	perror("send");
	exit(EXIT_FAILURE);
      }
    } else {
      fprintf(stderr, "get user failed");
    }
    /* initial message */
    printf("Waiting for response from %s.\n", hostname);
    /* get response from server */
    len = recv(sockfd, buff, sizeof(buff), 0);
    if (len == -1) {
      perror("receive");
      exit(EXIT_FAILURE);
    }
    /* null terminate */
    buff[len] = '\0';
    if (strcmp(buff, "ok") != 0) {
      printf("%s declined connection\n", hostname);
      close(sockfd);
      exit(EXIT_FAILURE);
    }
    /* verbosity */
    if (opts[0] > 1) {
      printf("Message from server: ok\n");
    }
  }
  /* create window unless N option is enabled */
  if (opts[1] == 0 ) {
    start_windowing();
  }
  /* set verbosity */
  set_verbosity(opts[0]);
  /* chat */
  talk(sockfd);
  /* close window unless N option is enabled */
  if (opts[1] == 0) {
    stop_windowing();
  }
  close(sockfd);
}
