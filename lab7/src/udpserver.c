#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

int main(int argc, char *argv[]) {
  // --- НАЧАЛО ИЗМЕНЕНИЙ ---
  int port = 20001;
  int bufsize = 1024;

  struct option long_options[] = {{"port", required_argument, 0, 'p'},
                                  {"bufsize", required_argument, 0, 'b'},
                                  {0, 0, 0, 0}};
  int opt;
  while ((opt = getopt_long(argc, argv, "p:b:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'p':
      port = atoi(optarg);
      break;
    case 'b':
      bufsize = atoi(optarg);
      break;
    default:
      fprintf(stderr, "Usage: %s --port <port> --bufsize <size>\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  char *mesg = malloc(bufsize);
  if (!mesg) {
      perror("malloc for buffer failed");
      exit(1);
  }
  // --- КОНЕЦ ИЗМЕНЕНИЙ ---

  int sockfd, n;
  char ipadr[16];
  struct sockaddr_in servaddr, cliaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }

  memset(&servaddr, 0, SLEN);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port); // Используем переменную

  if (bind(sockfd, (SADDR *)&servaddr, SLEN) < 0) {
    perror("bind problem");
    exit(1);
  }
  printf("UDP Server started on port %d with buffer size %d\n", port, bufsize);

  while (1) {
    unsigned int len = SLEN;
    if ((n = recvfrom(sockfd, mesg, bufsize, 0, (SADDR *)&cliaddr, &len)) < 0) {
      perror("recvfrom");
      exit(1);
    }
    if (n < bufsize) mesg[n] = 0;

    printf("REQUEST %s      FROM %s : %d\n", mesg,
           inet_ntop(AF_INET, (void *)&cliaddr.sin_addr.s_addr, ipadr, 16),
           ntohs(cliaddr.sin_port));

    if (sendto(sockfd, mesg, n, 0, (SADDR *)&cliaddr, len) < 0) {
      perror("sendto");
      exit(1);
    }
  }
  
  free(mesg);
  return 0;
}