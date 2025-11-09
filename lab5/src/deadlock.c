#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // для sleep()

// Два глобальных мьютекса
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// Функция для первого потока
void *thread1_routine(void *arg) {
  printf("Thread 1: Trying to lock mutex1...\n");
  pthread_mutex_lock(&mutex1);
  printf("Thread 1: Locked mutex1.\n");

  // Вставляем небольшую задержку, чтобы увеличить вероятность
  // того, что второй поток успеет захватить mutex2.
  printf("Thread 1: Sleeping for 1 second...\n");
  sleep(1);

  printf("Thread 1: Trying to lock mutex2...\n");
  // Эта строка приведет к блокировке, если mutex2 уже захвачен потоком 2
  pthread_mutex_lock(&mutex2);
  printf("Thread 1: Locked mutex2.\n");

  // Этот код никогда не выполнится в случае deadlock
  printf("Thread 1: Doing some work...\n");

  pthread_mutex_unlock(&mutex2);
  printf("Thread 1: Unlocked mutex2.\n");
  pthread_mutex_unlock(&mutex1);
  printf("Thread 1: Unlocked mutex1.\n");

  return NULL;
}

// Функция для второго потока
void *thread2_routine(void *arg) {
  printf("Thread 2: Trying to lock mutex2...\n");
  pthread_mutex_lock(&mutex2);
  printf("Thread 2: Locked mutex2.\n");

  printf("Thread 2: Sleeping for 1 second...\n");
  sleep(1);

  printf("Thread 2: Trying to lock mutex1...\n");
  // Эта строка приведет к блокировке, если mutex1 уже захвачен потоком 1
  pthread_mutex_lock(&mutex1);
  printf("Thread 2: Locked mutex1.\n");

  // Этот код никогда не выполнится в случае deadlock
  printf("Thread 2: Doing some work...\n");

  pthread_mutex_unlock(&mutex1);
  printf("Thread 2: Unlocked mutex1.\n");
  pthread_mutex_unlock(&mutex2);
  printf("Thread 2: Unlocked mutex2.\n");

  return NULL;
}

int main() {
  pthread_t thread1, thread2;

  printf("Main: Creating threads to demonstrate deadlock.\n");

  if (pthread_create(&thread1, NULL, thread1_routine, NULL) != 0) {
    perror("Failed to create thread 1");
    exit(1);
  }

  if (pthread_create(&thread2, NULL, thread2_routine, NULL) != 0) {
    perror("Failed to create thread 2");
    exit(1);
  }

  // Главный поток будет ждать завершения дочерних потоков.
  // В случае deadlock они никогда не завершатся, и программа "зависнет".
  printf("Main: Waiting for threads to finish...\n");
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  // Это сообщение никогда не будет выведено
  printf("Main: All threads finished successfully. (You should not see this!)\n");

  return 0;
}