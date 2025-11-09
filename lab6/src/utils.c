// utils.c

#include "utils.h"

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t res = 0;
    a %= mod;
    while (b > 0) {
        // Если b нечетное, добавляем a к результату
        if (b & 1) res = (res + a) % mod;
        // Удваиваем a и делим b пополам
        a = (a * 2) % mod;
        b >>= 1; // b = b / 2
    }
    return res;
}