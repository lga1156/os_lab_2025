#include "sum_lib.h"

#include <stdlib.h>

void *sum_part(void *args) {
    // 1. Распаковываем аргументы.
    // Приводим тип void* обратно к нашему типу SumArgs*.
    struct SumArgs *sum_args = (struct SumArgs *)args;

    // 2. Считаем сумму. Используем long long, чтобы избежать переполнения.
    long long local_sum = 0;
    for (int i = sum_args->begin; i < sum_args->end; i++) {
        local_sum += sum_args->array[i];
    }

    // 3. Возвращаем результат.
    // Потоковая функция должна вернуть void*. Мы не можем вернуть просто long long.
    // Правильный способ: выделить память в куче, записать туда результат
    // и вернуть указатель на эту память.
    long long *result = malloc(sizeof(long long));
    *result = local_sum;

    return (void *)result;
}