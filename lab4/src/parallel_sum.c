#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>   // Главный заголовочный файл для pthreads
#include <getopt.h>
#include <sys/time.h>

#include "utils.h"
#include "sum_lib.h"

int main(int argc, char **argv) {
    int threads_num = -1;
    int seed = -1;
    int array_size = -1;

    // --- 1. Парсинг аргументов командной строки ---
    static struct option options[] = {{"threads_num", required_argument, 0, 0},
                                      {"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    while (1) {
        int c = getopt_long(argc, argv, "", options, &option_index);
        if (c == -1) break;

        if (c == 0) {
            switch (option_index) {
                case 0: threads_num = atoi(optarg); break;
                case 1: seed = atoi(optarg); break;
                case 2: array_size = atoi(optarg); break;
                default: printf("Unknown option index\n"); return 1;
            }
        }
    }

    if (threads_num <= 0 || seed <= 0 || array_size <= 0) {
        printf("Usage: %s --threads_num \"num\" --seed \"num\" --array_size \"num\"\n", argv[0]);
        return 1;
    }

    // --- 2. Генерация массива ---
    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);

    // --- 3. Подготовка к созданию потоков ---
    pthread_t threads[threads_num];
    struct SumArgs args[threads_num];

    struct timeval start_time;
    gettimeofday(&start_time, NULL); // Начинаем замер времени

    // --- 4. Создание потоков ---
    int chunk_size = array_size / threads_num;
    for (int i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * chunk_size;
        args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * chunk_size;

        if (pthread_create(&threads[i], NULL, sum_part, &args[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // --- 5. Ожидание завершения потоков и сбор результатов ---
    long long total_sum = 0;
    for (int i = 0; i < threads_num; i++) {
        void *thread_result;
        // pthread_join блокирует выполнение до тех пор, пока поток threads[i] не завершится.
        // Второй аргумент - указатель, куда будет сохранен результат, возвращенный потоком.
        if (pthread_join(threads[i], &thread_result) != 0) {
            perror("Failed to join thread");
            return 1;
        }

        total_sum += *(long long *)thread_result;
        free(thread_result); // Освобождаем память, которую выделил поток
    }
    
    struct timeval finish_time;
    gettimeofday(&finish_time, NULL); // Заканчиваем замер времени

    // --- 6. Вывод результатов ---
    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    
    printf("Total sum: %lld\n", total_sum);
    printf("Elapsed time: %fms\n", elapsed_time);

    // --- 7. Очистка ---
    free(array);

    return 0;
}