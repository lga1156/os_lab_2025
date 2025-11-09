# Компилятор C
CC = gcc

# Флаги компилятора: -Wall (все предупреждения), -Werror (считать предупреждения ошибками), -g (отладочная информация)
CFLAGS = -Wall -Werror -g

# Имена всех исполняемых файлов
TARGETS = tcpclient tcpserver udpclient udpserver

# Цель по умолчанию, которая вызывается при запуске `make` без аргументов
# Она зависит от всех наших программ
all: $(TARGETS)

# Правило для сборки tcpclient
tcpclient: tcpclient.c
	$(CC) $(CFLAGS) -o tcpclient tcpclient.c

# Правило для сборки tcpserver
tcpserver: tcpserver.c
	$(CC) $(CFLAGS) -o tcpserver tcpserver.c

# Правило для сборки udpclient
udpclient: udpclient.c
	$(CC) $(CFLAGS) -o udpclient udpclient.c

# Правило для сборки udpserver
udpserver: udpserver.c
	$(CC) $(CFLAGS) -o udpserver udpserver.c

# Цель для очистки скомпилированных файлов
# `rm -f` удаляет файлы без запроса подтверждения и не ругается, если файлов нет
clean:
	rm -f $(TARGETS)

# Объявляем цели, которые не являются файлами, чтобы make не искал файлы с именами 'all' и 'clean'
.PHONY: all clean