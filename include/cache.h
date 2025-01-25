#ifndef CACHE_H
#define CACHE_H

#include <unistd.h>

#define BLOCK_SIZE 1024  // Размер блока в байтах
#define CACHE_SIZE 5     // Максимальный размер кэша (количество блоков)

typedef struct CacheBlock {
    off_t offset;  // Смещение блока в файле
    char data[BLOCK_SIZE];  // Данные блока
    struct CacheBlock *prev, *next;  // Для реализации двусвязного списка
} CacheBlock;

// Инициализация кэша
void cache_init();

// Поиск блока в кэше
CacheBlock* cache_find(off_t offset);

// Перемещение блока в начало кэша (Most Recently Used)
void cache_move_to_front(CacheBlock *block);

// Добавление блока в кэш
void cache_add(off_t offset, const char *data);

// Получение размера кэша (для проверки переполнения)
int cache_size();

// Освобождение памяти кэша
void cache_free();

#endif
