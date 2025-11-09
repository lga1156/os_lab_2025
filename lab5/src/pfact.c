#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Глобальные переменные для хранения общего результата и мьютекса
long long global_result = 1;
long long mod_value = 0;
pthread_mutex_t result_mutex;

// Структура для передачи аргументов в поток
typedef struct {
  int start;
  int end;
  int thread_id;
} thread_args;

// Функция, которую будет выполнять каждый поток
void *factorial_worker(void *args) {
  thread_args *t_args = (thread_args *)args;
  long long partial_result = 1;

  printf("Thread %d: calculating product from %d to %d\n", t_args->thread_id,
         t_args->start, t_args->end);

  // Вычисляем частичное произведение в своем диапазоне
  for (int i = t_args->start; i <= t_args->end; i++) {
    partial_result = (partial_result * i) % mod_value;
  }

  // Захватываем мьютекс для безопасного обновления общего результата
  pthread_mutex_lock(&result_mutex);

  // --- Критическая секция ---
  global_result = (global_result * partial_result) % mod_value;
  // --- Конец критической секции ---

  // Освобождаем мьютекс
  pthread_mutex_unlock(&result_mutex);

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int k = 0, pnum = 0;

  // --- Парсинг аргументов командной строки ---
  // Используем getopt_long для удобного парсинга --pnum, --mod
  int opt;
  static struct option long_options[] = {{"pnum", required_argument, 0, 'p'},
                                         {"mod", required_argument, 0, 'm'},
                                         {0, 0, 0, 0}};
  int option_index = 0;

  while ((opt = getopt_long(argc, argv, "k:p:m:", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'k':
      k = atoi(optarg);
      break;
    case 'p':
    case 0: // Для --pnum
      pnum = atoi(optarg);
      break;
    case 'm':
    case 1: // для --mod
      mod_value = atoll(optarg);
      break;
    default:
      fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<modulus>\n",
              argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (k <= 0 || pnum <= 0 || mod_value <= 0) {
    fprintf(stderr, "All arguments (-k, --pnum, --mod) are required and must be positive.\n");
    exit(EXIT_FAILURE);
  }

  // --- Инициализация ---
  pthread_t threads[pnum];
  thread_args t_args[pnum];

  // Инициализируем мьютекс
  if (pthread_mutex_init(&result_mutex, NULL) != 0) {
    perror("Mutex init failed");
    exit(EXIT_FAILURE);
  }

  // --- Распределение работы и создание потоков ---
  int chunk_size = k / pnum;
  for (int i = 0; i < pnum; i++) {
    t_args[i].thread_id = i + 1;
    t_args[i].start = i * chunk_size + 1;

    if (i == pnum - 1) {
      // Последний поток берет все оставшиеся числа
      t_args[i].end = k;
    } else {
      t_args[i].end = (i + 1) * chunk_size;
    }
    
    // Если k < pnum, некоторые потоки могут ничего не делать
    if (t_args[i].start > k) {
        t_args[i].start = t_args[i].end + 1; // Делаем диапазон пустым
    }

    printf("Main: Creating thread %d for range [%d, %d]\n", t_args[i].thread_id, t_args[i].start, t_args[i].end);
    if (pthread_create(&threads[i], NULL, factorial_worker, &t_args[i]) != 0) {
      perror("Failed to create thread");
      exit(EXIT_FAILURE);
    }
  }

  // --- Ожидание завершения всех потоков ---
  for (int i = 0; i < pnum; i++) {
    if (pthread_join(threads[i], NULL) != 0) {
      perror("Failed to join thread");
      exit(EXIT_FAILURE);
    }
  }

  // --- Вывод результата и очистка ---
  printf("\nFactorial of %d mod %lld is: %lld\n", k, mod_value, global_result);

  // Уничтожаем мьютекс
  pthread_mutex_destroy(&result_mutex);

  return 0;
}