#include "mytalk.h"

/* parse command line and call make_server or make_client depending on
   whether a hostname was provided  */
int main(int argc, char *argv[]) {
  /* 0: v, 1: a, 2: N */
  int opts[3] = {0};
  int opt;
  int port;
  char *hostname;
  extern int optind;
  /* parse options */
  while ((opt = getopt(argc, argv, "vaN")) != -1) {
    switch(opt) {
    case 'v':
      opts[0]++;
    case 'a':
      opts[1] = 1;
    case 'N':
      opts[2] = 1;
    }
  }
  /* choose server or client mode */
  port = strtol(argv[optind], NULL, DECIMAL);
  /* hostname found -> client mode */ 
  if (port == 0) {
    hostname = argv[optind];
    port = strtol(argv[optind + 1], NULL, DECIMAL);
    if (port < 1024) {
      fprintf(stderr, "Invalid port");
      exit(EXIT_FAILURE);
    }
    /*client function*/
    /* verbosity */
    if (opts[0] > 1) {
      printf("Calling make_client...");
    }
    make_client(opts, hostname, port);
  }
  /* port found -> server mode */
  else {
    /* server function */
    if (port < 1024) {
      fprintf(stderr, "Invalid port");
      exit(EXIT_FAILURE);
    }
    if (opts[0] > 1) {
      printf("Calling make_server");
    }
    make_server(opts, port);
  }
  return 0;
}

/* handles talking between server and client */ 
void talk(int sockfd) {
  int mlen;
  struct pollfd fds[REMOTE + 1];
  char buff[MAXLEN + 1];
  /* define poll struct */
  fds[LOCAL].fd = STDIN_FILENO;
  fds[LOCAL].events = POLLIN;
  fds[REMOTE].fd = sockfd;
  fds[REMOTE].events = POLLIN;
  /* using len and buff, send() and recv() from sockfd */
  while (1) {
    /* poll stdin and socket with no timeout */
    if (poll(fds, sizeof(fds)/sizeof(struct pollfd), -1) == -1) {
      perror("poll");
      exit(EXIT_FAILURE);
    }
    if (fds[LOCAL].revents & POLLIN) {
      /* handle user input */
      update_input_buffer();
      while (has_whole_line()) {
	mlen = read_from_input(buff, MAXLEN);
	if (mlen == -1) {
	  perror("read_from_input");
	  exit(EXIT_FAILURE);
	}
	if ((mlen = send(sockfd, buff, mlen, 0)) == -1) {
	  perror("send");
	  exit(EXIT_FAILURE);
	}
	if (has_hit_eof()) {
	  return;
	}
      }
    }
    if (fds[REMOTE].revents & POLLIN) {
      /* receive message from peer */
      mlen = recv(sockfd, buff, MAXLEN, 0);
      if (mlen == -1) {
	perror("recv");
	exit(EXIT_FAILURE);
      }
      /* null terminate received message */
      buff[mlen] = '\0';
      /* exit case */
      if (mlen == 0) {
	printf("\nConnection closed.  ^C to terminate.\n");
	pause();
      }
      /* write received message to window */
      fprint_to_output(buff);
    }
  }
}
