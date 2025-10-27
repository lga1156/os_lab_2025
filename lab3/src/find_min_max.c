#include "find_min_max.h"

#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  // Проходим по массиву в заданном диапазоне [begin, end)
  for (unsigned int i = begin; i < end; i++) {
    // Если текущий элемент меньше текущего минимума, обновляем минимум
    if (array[i] < min_max.min) {
      min_max.min = array[i];
    }
    
    // Если текущий элемент больше текущего максимума, обновляем максимум
    if (array[i] > min_max.max) {
      min_max.max = array[i];
    }
  }

  return min_max;
}