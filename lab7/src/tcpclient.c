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
#define SIZE sizeof(struct sockaddr_in)

int main(int argc, char *argv[]) {
  // --- НАЧАЛО ИЗМЕНЕНИЙ ---
  int bufsize = 100; // Размер буфера по умолчанию
  
  // Парсинг опционального аргумента --bufsize
  struct option long_options[] = {{"bufsize", required_argument, 0, 'b'},
                                  {0, 0, 0, 0}};
  int opt;
  while ((opt = getopt_long(argc, argv, "b:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'b':
      bufsize = atoi(optarg);
      break;
    default:
        // optind сместится, так что выводим правильное использование
      fprintf(stderr, "Usage: %s <ip> <port> [--bufsize <size>]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  // Проверяем, что обязательные аргументы (ip и port) переданы
  if (argc - optind < 2) {
    fprintf(stderr, "Usage: %s <ip> <port> [--bufsize <size>]\n", argv[0]);
    exit(1);
  }
  
  char *ip_addr = argv[optind];
  int port = atoi(argv[optind + 1]);

  char *buf = malloc(bufsize);
  if (!buf) {
      perror("malloc for buffer failed");
      exit(1);
  }
  // --- КОНЕЦ ИЗМЕНЕНИЙ ---

  int fd, nread;
  struct sockaddr_in servaddr;
  
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, ip_addr, &servaddr.sin_addr) <= 0) {
    perror("bad address");
    exit(1);
  }
  servaddr.sin_port = htons(port);

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
    perror("connect");
    exit(1);
  }

  write(1, "Input message to send\n", 22);
  while ((nread = read(0, buf, bufsize)) > 0) {
    if (write(fd, buf, nread) < 0) {
      perror("write");
      exit(1);
    }
  }

  close(fd);
  free(buf);
  exit(0);
}