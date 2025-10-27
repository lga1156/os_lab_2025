#!/bin/bash


if [ $# -eq 0 ]; then
  echo "Ошибка: Не передано ни одного числового аргумента."
  echo "Использование: ./average.sh число1 число2 ..."
  exit 1 
fi

sum=0

for number in "$@"; do
	((sum += number))
done

average=$(python3 -c "print(round($sum / $#, 2))")

echo "Количество аргументов: $#"
echo "Среднее арифметическое: $average"


