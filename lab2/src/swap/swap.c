#include "swap.h"

void Swap(char *left, char *right)
{
	char temp = *left;  // 1. Сохраняем значение из 'left'
	*left = *right;     // 2. Копируем значение из 'right' в 'left'
	*right = temp;      // 3. Копируем сохраненное значение в 'right'
}