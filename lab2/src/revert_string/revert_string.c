#include <string.h>
#include "revert_string.h"

void RevertString(char *str)
{
	// 1. Находим длину строки
	int len = strlen(str);
	
	// 2. Инициализируем два индекса:
	int left = 0;          // 'left' указывает на начало строки
	int right = len - 1;   // 'right' указывает на последний символ (перед '\0')

	// 3. Идем навстречу друг другу, пока левый индекс меньше правого
	while (left < right)
	{
		// Меняем местами символы
		char temp = str[left];
		str[left] = str[right];
		str[right] = temp;

		// Сдвигаем индексы к центру
		left++;
		right--;
	}
}