/* ---- DEFINE ---- */
//#define WINDOWS
#define LINUX
//#define ARDUINO
//#define STM32
/* ---- DEFINE ---- */



int nsedt(int argc, char* argv[]);

#include <stdio.h>
#include <string.h>

#define APP_NAME nsedt

#if defined(WINDOWS)
#define _CRT_SECURE_NO_WARNINGS
#else
char* strlwr(char* s) {
	char* str;
	str = s;
	while(*str != '\0') {
		if(*str >= 'A' && *str <= 'Z') {
			*str += 'a'-'A';
		}
		str++;
	}
	return s; 
}
#endif

#if defined(WINDOWS)
#include <stdlib.h>
#include <vadefs.h>
#define clearScreen() system("cls")
int print(const char* format, ...) {
	va_list args = 0;
	__crt_va_start(args, format);
	int result = vprintf(format, args);
	__crt_va_end(args);
	return result;
}
int scan(char* buffer) { return gets(buffer); }
int fscan(char* buffer, const char* format, ...) {
	gets(buffer);
	va_list args = 0;
	__crt_va_start(args, format);
	int result = vsscanf(buffer, format, args);
	__crt_va_end(args);
	return result;
}

int main(int argc, char* argv[]) {
	int result;
	result = APP_NAME(argc, argv);
	system("pause");
	return result;
}

#elif defined(LINUX)
#include <stdlib.h>
#include <stdarg.h>
#define IOBUF 128
#define clearScreen() system("clear")
int print(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int result = vprintf(format, args);
	va_end(args);
	return result;
}
char* scan(char* buffer) {
	char count = 0, tmp = 0;
	while (1) {
		tmp = getchar();
		if (tmp == '\n') break;
		buffer[count] = tmp;
		if (buffer[count] == 0x08 && count > 0) {
			count -= 1;
			print("%c", 0x08);
			continue;
		}
		else if (buffer[count] != 0x08) {
			print("%c", buffer[count]);
			count += 1;
		}
	}
	buffer[count] = '\0';
	print("\n");
}
int fscan(char* buffer, const char* format, ...) {
	scan(buffer);
	va_list args;
	va_start(args, format);
	int result = vsscanf(buffer, format, args);
	va_end(args);
	return result;
}

int main(int argc, char* argv[]) {
	int result;
	result = APP_NAME(argc, argv);
	return result;
}

#elif defined(ARDUINO)
#include <stdarg.h>
#define BACKCOLOR 0x0000
#define __print(buf) tft.print(buf)
#define clearScreen() { tft.setCursor(0, 0); tft.fillScreen(BACKCOLOR); }
#define IOBUF 128
int print(const char* format, ...) {
	char* iobuf = malloc(sizeof(char) * IOBUF);
	va_list args;
	va_start(args, format);
	int result = vsprintf(iobuf, format, args);
	va_end(args);
	__print(iobuf);
	free(iobuf);
	return result;
}
int scan(char* buffer) {
	char count = 0;
	while (true) {
		if (Serial.available() > 0) {
			if (Serial.peek() == '\n') break;
			buffer[count] = Serial.read();
			if (buffer[count] == 0x08 && count > 0) {
				count -= 1;
				print("%c", 0x08);
				continue;
			}
			else if (buffer[count] != 0x08) {
				print("%c", buffer[count]);
				count += 1;
			}
		}
		else if (Serial1.available() > 0) {
			if (Serial1.peek() == '\n') break;
			buffer[count] = Serial1.read();
			if (buffer[count] == 0x08 && count > 0) {
				count -= 1;
				print("%c", 0x08);
				continue;
			}
			else if (buffer[count] != 0x08) {
				print("%c", buffer[count]);
				count += 1;
			}
		}
	}
	buffer[count] = '\0';
	if (Serial.available() > 0) Serial.read();
	else if (Serial1.available() > 0) Serial1.read();
	print("\n");
}
int fscan(char* buffer, const char* format, ...) {
	scan(buffer);
	va_list args = 0;
	va_start(args, format);
	int result = vsscanf(buffer, format, args);
	va_end(args);
	return result;
}

#elif defined(STM32)
#include <stdarg.h>
#define HUART huart2
#define __print(buf) lcd->printfa(lcd->p, buf)
#define clearScreen() lcd->clear(lcd->p)
#define IOBUF 128
int print(const char* format, ...) {
	char* iobuf = malloc(sizeof(char) * IOBUF);
	va_list args;
	va_start(args, format);
	int result = vsprintf(iobuf, format, args);
	va_end(args);
	__print(iobuf);
	free(iobuf);
	return result;
}
int scan(char* buffer) {
	char count = 0, tmp = '\0';
	while (1) {
		if (HAL_UART_Receive(&HUART, &tmp, 1, 1) == HAL_OK) {
			if (tmp == '\n') break;
			buffer[count] = tmp;
			if (buffer[count] == 0x08 && count > 0) {
				count -= 1;
				print("%c", 0x08);
				continue;
			} else if (buffer[count] != 0x08) {
				print("%c", buffer[count]);
				count += 1;
			}
		}
	}
	buffer[count] = '\0';
	print("\n");
}
int fscan(char* buffer, const char* format, ...) {
	scan(buffer);
	va_list args = 0;
	va_start(args, format);
	int result = vsscanf(buffer, format, args);
	va_end(args);
	return result;
}

#endif

/* -------------------------------- */

#define VERSION 0.01

#define OK 0
#define ERR 1
#define ETC -1

#ifndef IOBUF
#define IOBUF 128
#endif

/* -------------------------------- */

char* read(char* path);
int lines(char* src);
char* line(char* src, int index);

/* -------------------------------- */

int nsedt(int argc, char* argv[]) {
	print("NyaSama Edit Data Terminal\n");
	print("Version: %1.2f\n\n", VERSION);
	if (argc < 2) {
		print("Usage: nsedt [FILE]\n\n");
		return OK;
	} else {
		
		return OK;
	}
}

/* -------------------------------- */

char* read(char* path) {
	FILE* f = fopen(path, "r");
	if (f == 0) {
		print("File open failed.\n");
		print("At file: %s\n\n", path);
		return OK;
	}
	int length = 0; char tmp;
	while (feof(f) == 0) {
		tmp = fgetc(f);
		if (tmp != '\r')
			length += 1;
	}
	fclose(f);
	f = fopen(path, "r");
	if (f == 0) {
		print("File open failed.\n");
		print("At file: %s\n\n", path);
		return OK;
	}
	char* data = malloc(sizeof(char) * (length + 1));
	length = 0;
	while (feof(f) == 0) {
		tmp = fgetc(f);
		if (tmp != '\r') {
			data[length] = tmp;
			length += 1;
		}
	}
	data[length] = '\0';
	return data;
}

int lines(char* src) {
	if(src == 0) return OK;
	int cnt = 0, length = strlen(src);
	for (int i = 0; i < length; i++)
		if (src[i] == '\n') cnt += 1;
	return cnt;
}

char* line(char* src, int index) {
	if (index >= lines(src)) return OK;
	int srcLen = strlen(src), cnt = 0, pos = 0;
	char* buf = malloc(sizeof(char) * srcLen);
	char* result = 0;
	for (int i = 0; i < srcLen; i++) {
		if (index == 0) {
			for (i = 0; src[i] != '\n'; i++)
				buf[i] = src[i];
			pos = i + 1;
			result = malloc(sizeof(char) * (pos));
			for (i = 0; i < pos; i++) {
				if (i == pos - 1) {
					result[i] = '\0';
					break;
				}
				result[i] = buf[i];
			}
			free(buf);
			return result;
		}
		if (index == cnt) {
			pos = i;
			for (; src[i] != '\n'; i++)
				buf[i - pos] = src[i];
			pos = i - pos + 1;
			result = malloc(sizeof(char) * pos);
			for (i = 0; i < pos; i++) {
				if (i == pos - 1) {
					result[i] = '\0';
					break;
				}
				result[i] = buf[i];
			}
			free(buf);
			return result;
		}
		if (src[i] == '\n') cnt += 1;
	}
	return OK;
}
