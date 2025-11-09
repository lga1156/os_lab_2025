// utils.h

#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h> // Для типа uint64_t

/**
 * @brief Выполняет умножение (a * b) % mod для 64-битных чисел, избегая переполнения.
 * 
 * @param a Первый множитель.
 * @param b Второй множитель.
 * @param mod Модуль.
 * @return (uint64_t) Результат (a * b) % mod.
 */
uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

#endif // UTILS_H