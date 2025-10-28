#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    // Создаем дочерний процесс
    pid_t child_pid = fork();

    // fork() вернул ошибку
    if (child_pid < 0) {
        perror("fork failed");
        return 1;
    }

    // Этот код выполняется в ДОЧЕРНЕМ процессе
    if (child_pid == 0) {
        printf("CHILD (PID: %d): Я родился и сейчас умру, чтобы стать зомби.\n", getpid());
        exit(0); // Дочерний процесс немедленно завершается
    } 
    // Этот код выполняется в РОДИТЕЛЬСКОМ процессе
    else {
        printf("PARENT (PID: %d): Я создал дочерний процесс с PID %d.\n", getpid(), child_pid);
        printf("PARENT: Сейчас я засну на 30 секунд. В это время можно проверить список процессов.\n");
        
        // Родитель засыпает, НЕ вызывая wait()
        sleep(30);

        printf("PARENT: Я проснулся. Программа завершается, и зомби будет убран системой.\n");
        // Родительский процесс завершается, и init "уберет" зомби
    }

    return 0;
}