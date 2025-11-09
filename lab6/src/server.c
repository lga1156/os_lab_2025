#include "utils.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"
#include <inttypes.h> // <-- ВКЛЮЧИТЬ ЭТОТ ФАЙЛ

struct FactorialArgs {
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
};

uint64_t Factorial(const struct FactorialArgs *args) {
  uint64_t ans = 1;
  if (args->begin > args->end) return 1; // Если диапазон пуст
  for (uint64_t i = args->begin; i <= args->end; i++) {
    ans = MultModulo(ans, i, args->mod);
  }
  return ans;
}

void *ThreadFactorial(void *args) {
  struct FactorialArgs *fargs = (struct FactorialArgs *)args;
  return (void *)Factorial(fargs);
}

int main(int argc, char **argv) {
  int tnum = -1;
  int port = -1;

  while (true) {
    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1) break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        port = atoi(optarg);
        break;
      case 1:
        tnum = atoi(optarg);
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;
    case '?':
      printf("Unknown argument\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (port == -1 || tnum == -1) {
    fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Can not create server socket!");
    return 1;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons((uint16_t)port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

  if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("Can not bind to socket!");
    return 1;
  }
  if (listen(server_fd, 128) < 0) {
    perror("Could not listen on socket\n");
    return 1;
  }
  printf("Server listening at %d with %d threads\n", port, tnum);

  while (true) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0) {
      perror("Could not establish new connection\n");
      continue;
    }

    while (true) {
      unsigned int buffer_size = sizeof(uint64_t) * 3;
      char from_client[buffer_size];
      int read_bytes = recv(client_fd, from_client, buffer_size, 0);

      if (!read_bytes) break;
      if (read_bytes < 0) {
        perror("Client read failed");
        break;
      }
      if (read_bytes < buffer_size) {
        fprintf(stderr, "Client send wrong data format\n");
        break;
      }

      pthread_t threads[tnum];
      uint64_t begin = 0, end = 0, mod = 0;
      memcpy(&begin, from_client, sizeof(uint64_t));
      memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
      memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

      // ИСПРАВЛЕНО: Замена %llu на макрос PRIu64
      fprintf(stdout, "Receive task: range [%" PRIu64 ", %" PRIu64 "], mod %" PRIu64 "\n", begin, end, mod);

      struct FactorialArgs args[tnum];
      uint64_t num_elements = end - begin + 1;
      uint64_t chunk_size = num_elements / tnum;

      for (int i = 0; i < tnum; i++) {
        args[i].begin = begin + i * chunk_size;
        args[i].mod = mod;
        if (i == tnum - 1) {
          args[i].end = end;
        } else {
          args[i].end = begin + (i + 1) * chunk_size - 1;
        }

        if (pthread_create(&threads[i], NULL, ThreadFactorial, (void *)&args[i])) {
          printf("Error: pthread_create failed!\n");
          return 1;
        }
      }

      uint64_t total = 1;
      for (int i = 0; i < tnum; i++) {
        void *result_ptr;
        pthread_join(threads[i], &result_ptr);
        uint64_t result = (uint64_t)result_ptr;
        total = MultModulo(total, result, mod);
      }
      
      // ИСПРАВЛЕНО: Замена %llu на макрос PRIu64
      printf("Calculated partial result: %" PRIu64 "\n", total);

      char buffer[sizeof(total)];
      memcpy(buffer, &total, sizeof(total));
      if (send(client_fd, buffer, sizeof(total), 0) < 0) {
        perror("Can't send data to client\n");
        break;
      }
    }
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
  }
  return 0;
}