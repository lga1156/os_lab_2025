# --- Переменные ---
CC = gcc
CFLAGS = -Wall -g
# Флаги для линковщика. -pthread - ОБЯЗАТЕЛЬНЫЙ флаг для сборки с pthreads.
LDFLAGS = -pthread
# Команда для удаления
RM = rm -f

# --- Основные цели ---
all: psum

# --- Правила сборки ---

# Правило для сборки исполняемого файла 'psum'
psum: parallel_sum.o sum_lib.o utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Правила для компиляции каждого .c файла в объектный .o файл
parallel_sum.o: parallel_sum.c sum_lib.h utils.h
	$(CC) $(CFLAGS) -c -o $@ $<

sum_lib.o: sum_lib.c sum_lib.h
	$(CC) $(CFLAGS) -c -o $@ $<

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c -o $@ $<

# --- Служебные цели ---
clean:
	$(RM) psum *.o

.PHONY: all clean