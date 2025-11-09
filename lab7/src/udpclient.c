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

int main(int argc, char **argv) {
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
      fprintf(stderr, "Usage: %s <ip> --port <port> --bufsize <size>\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (argc - optind != 1) {
    fprintf(stderr, "Usage: %s <ip> [--port <port>] [--bufsize <size>]\n", argv[0]);
    exit(1);
  }

  char *ip_addr = argv[optind];
  char *sendline = malloc(bufsize);
  char *recvline = malloc(bufsize + 1);
  if (!sendline || !recvline) {
      perror("malloc failed");
      exit(1);
  }
  // --- КОНЕЦ ИЗМЕНЕНИЙ ---

  int sockfd, n;
  struct sockaddr_in servaddr;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port); // Используем переменную

  if (inet_pton(AF_INET, ip_addr, &servaddr.sin_addr) < 0) {
    perror("inet_pton problem");
    exit(1);
  }
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }

  write(1, "Enter string\n", 13);

  while ((n = read(0, sendline, bufsize)) > 0) {
    if (sendto(sockfd, sendline, n, 0, (SADDR *)&servaddr, SLEN) == -1) {
      perror("sendto problem");
      exit(1);
    }
    if (recvfrom(sockfd, recvline, bufsize, 0, NULL, NULL) == -1) {
      perror("recvfrom problem");
      exit(1);
    }
    recvline[n] = 0; // Добавляем терминатор строки
    printf("REPLY FROM SERVER= %s", recvline);
  }
  
  close(sockfd);
  free(sendline);
  free(recvline);
  return 0;
}