# Компилятор C
CC = gcc

# Флаги компилятора: -Wall (все предупреждения), -Werror (предупреждения как ошибки), -g (отладочная информация)
CFLAGS = -Wall -Werror -g

# Флаги линковщика: -lpthread для подключения библиотеки POSIX Threads
LDFLAGS = -lpthread

# Имена исполняемых файлов
CLIENT_EXEC = client
SERVER_EXEC = server

# Исходные файлы
CLIENT_SRC = client.c
SERVER_SRC = server.c
UTILS_SRC = utils.c

# Объектные файлы (автоматически получаем из .c)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
UTILS_OBJ = $(UTILS_SRC:.c=.o)

# Цель по умолчанию, собирает всё
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Правило для сборки клиента
# Клиент зависит от своего объектного файла и от объектного файла утилит
$(CLIENT_EXEC): $(CLIENT_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Правило для сборки сервера
# Сервер зависит от своего объектного файла и от объектного файла утилит
$(SERVER_EXEC): $(SERVER_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Общее правило для компиляции .c файлов в .o
# -c говорит компилятору только скомпилировать, но не линковать
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Правило для очистки
# Теперь нужно удалять и объектные файлы (*.o)
clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC) $(CLIENT_OBJ) $(SERVER_OBJ) $(UTILS_OBJ)

# Объявляем цели, которые не являются файлами
.PHONY: all clean