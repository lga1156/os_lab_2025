#include "utils.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <limits.h>
#include <inttypes.h> // <-- ВКЛЮЧИТЬ ЭТОТ ФАЙЛ

// Структура для хранения информации о сервере
struct Server {
  char ip[255];
  int port;
};

// Структура для передачи аргументов в поток клиента
struct ClientThreadArgs {
  struct Server server_info;
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
};

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }
  if (errno != 0 || (end != NULL && *end != '\0')) return false;
  *val = i;
  return true;
}

// Функция, выполняемая каждым потоком для связи с одним сервером
void *ClientWorker(void *args) {
  struct ClientThreadArgs *ct_args = (struct ClientThreadArgs *)args;
  
  struct hostent *hostname = gethostbyname(ct_args->server_info.ip);
  if (hostname == NULL) {
    fprintf(stderr, "gethostbyname failed with %s\n", ct_args->server_info.ip);
    pthread_exit((void*)1); 
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(ct_args->server_info.port);
  server_addr.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    perror("Socket creation failed!");
    pthread_exit((void*)1);
  }

  if (connect(sck, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Connection failed");
    close(sck);
    pthread_exit((void*)1);
  }

  // ИСПРАВЛЕНО: Замена %llu на макрос PRIu64
  printf("Thread for %s:%d connected. Sending task: range [%" PRIu64 ", %" PRIu64 "]\n", 
         ct_args->server_info.ip, ct_args->server_info.port, ct_args->begin, ct_args->end);

  char task[sizeof(uint64_t) * 3];
  memcpy(task, &ct_args->begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &ct_args->end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &ct_args->mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0) {
    perror("Send failed");
    close(sck);
    pthread_exit((void*)1);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    perror("Recieve failed");
    close(sck);
    pthread_exit((void*)1);
  }

  uint64_t answer = 0;
  memcpy(&answer, response, sizeof(uint64_t));
  close(sck);

  return (void *)answer;
}

int main(int argc, char **argv) {
  uint64_t k = 0;
  uint64_t mod = 0;
  char servers_path[PATH_MAX] = {'\0'};

  while (true) {
    // int current_optind = optind ? optind : 1; // <-- УДАЛЕНО: неиспользуемая переменная
    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1) break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        break;
      case 2:
        strncpy(servers_path, optarg, PATH_MAX - 1);
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;
    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == 0 || mod == 0 || !strlen(servers_path)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/servers.conf\n", argv[0]);
    return 1;
  }

  FILE *fp = fopen(servers_path, "r");
  if (!fp) {
    perror("Could not open servers file");
    return 1;
  }

  struct Server *servers_list = NULL;
  int servers_num = 0;
  char line[256];
  while (fgets(line, sizeof(line), fp)) {
      char *newline = strchr(line, '\n');
      if (newline) *newline = '\0'; // Убираем символ новой строки

      servers_num++;
      servers_list = realloc(servers_list, servers_num * sizeof(struct Server));
      char *ip = strtok(line, ":");
      char *port_str = strtok(NULL, "");
      if (ip && port_str) {
          strncpy(servers_list[servers_num-1].ip, ip, 254);
          servers_list[servers_num-1].port = atoi(port_str);
      } else {
          fprintf(stderr, "Invalid server format in file: %s\n", line);
          servers_num--; 
      }
  }
  fclose(fp);

  if (servers_num == 0) {
      fprintf(stderr, "No valid servers found in file.\n");
      free(servers_list);
      return 1;
  }
  printf("Loaded %d servers.\n", servers_num);

  pthread_t threads[servers_num];
  struct ClientThreadArgs args[servers_num];
  uint64_t chunk_size = k / servers_num;

  for (int i = 0; i < servers_num; i++) {
    args[i].server_info = servers_list[i];
    args[i].begin = i * chunk_size + 1;
    if (i == servers_num - 1) {
        args[i].end = k;
    } else {
        args[i].end = (i + 1) * chunk_size;
    }
    args[i].mod = mod;
    
    if (pthread_create(&threads[i], NULL, ClientWorker, &args[i])) {
        perror("Failed to create a client thread");
        return 1;
    }
  }
  
  uint64_t total_factorial = 1;
  for (int i = 0; i < servers_num; i++) {
      void* result_ptr;
      pthread_join(threads[i], &result_ptr);
      uint64_t result = (uint64_t)result_ptr;
      
      // Проверяем, что поток не завершился с ошибкой
      if (result_ptr == (void*)1) { 
          fprintf(stderr, "Thread for server %s:%d failed.\n", servers_list[i].ip, servers_list[i].port);
      } else {
          total_factorial = MultModulo(total_factorial, result, mod);
      }
  }

  free(servers_list);
  
  // ИСПРАВЛЕНО: Замена %llu на макрос PRIu64
  printf("\nFinal answer for %" PRIu64 "! mod %" PRIu64 " is: %" PRIu64 "\n", k, mod, total_factorial);

  return 0;
}