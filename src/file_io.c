#include "file_io.h"
#include "cache.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

const char* one_arm_bandit() {
    const char *symbols[] = {"💎", "💩", "🔒"};
    int spin = rand() % 3;
    return symbols[spin];
}

int roulette() {
    return rand() % 36;
}

int lab2_open(const char *path) {
    printf("🎰 Крутится слот-машина: %s\n", one_arm_bandit());
    return open(path, O_CREAT | O_RDWR, 0644);
}

int lab2_close(int fd) {
    printf("🎰 Закрытие файла: %s\n", one_arm_bandit());
    return close(fd);
}

ssize_t lab2_read(int fd, void *buf, size_t count) {
    off_t offset = lseek(fd, 0, SEEK_CUR);
    CacheBlock *cached_block = cache_find(offset);

    printf("🎰 Чтение данных: %s\n", one_arm_bandit());

    if (cached_block) {
        memcpy(buf, cached_block->data, count); 
        cache_move_to_front(cached_block);
        return count;
    } else {
        int result = roulette();
        if (result <= 10) { 
            printf("🎰 Рулетка: Плохая удача! Кэш не найден, но данные будут записаны медленно.\n");
            sleep(2);
        }
        ssize_t bytes_read = read(fd, buf, count);
        if (bytes_read > 0) {
            cache_add(offset, buf);
        }
        return bytes_read;
    }
}

ssize_t lab2_write(int fd, const void *buf, size_t count) {
    off_t offset = lseek(fd, 0, SEEK_CUR);
    CacheBlock *cached_block = cache_find(offset);

    printf("🎰 Запись данных: %s\n", one_arm_bandit());

    if (cached_block) {
        memcpy(cached_block->data, buf, count); 
    } else {
        cache_add(offset, buf);
    }

    // Симуляция эффекта рулетки
    int result = roulette();
    if (result <= 10) {
        printf("🎰 Рулетка: Плохая удача! Запись будет выполнена с ошибками.\n");
        return -1;
    }

    return write(fd, buf, count);
}

off_t lab2_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}

int lab2_fsync(int fd) {
    printf("🎰 Синхронизация: %s\n", one_arm_bandit());
    return fsync(fd);
}
