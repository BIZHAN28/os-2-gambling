#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "file_io.h"
#include "cache.h"

const char* symbols[] = {"\U0001F48E", "\U0001F4A9", "\U0001F512"};

// Генерация результата для слот-машины
void one_arm_bandit(char *result) {
    for (int i = 0; i < 3; i++) {
        strcpy(&result[i * 4], symbols[rand() % 3]);
    }
    result[12] = '\0';
}

// Вызов видеоплеера
void play_media(const char *file, const char *intf) {
    char command[256];
    snprintf(command, sizeof(command), "vlc --intf %s --play-and-exit %s &", intf, file);
    system(command);
}

// Слот-машина
void handle_slot_machine() {
    char bandit_result[13];
    one_arm_bandit(bandit_result);
    printf("🎰 Слот-машина: %s\n", bandit_result);

    if (strcmp(bandit_result, "💎💎💎") == 0 || 
        strcmp(bandit_result, "💩💩💩") == 0 || 
        strcmp(bandit_result, "🔒🔒🔒") == 0) {
        printf("✨ Удача! Операция проходит мгновенно.\n");
        play_media("GAMBLECORE.mp4", "dummy");
    } else {
        printf("💩 Неудача! Операция будет медленной...\n");
        play_media("dang-it.mp3", "dummy");
        sleep(2);
    }
}

// Русская рулетка
int roulette() {
    int result = rand() % 6;
    if (result == 0) {
        printf("🔫 Рулетка: Плохая удача! Вы мертвы.\n");
        play_media("roulette.mp4", "dummy");
        exit(EXIT_FAILURE);
    }
    printf("🔫 Рулетка: А вы везунчик.\n");
    return result;
}

// Обертка для открытия файла
int lab2_open(const char *path) {
    roulette();  // Русская рулетка при открытии файла
    return open(path, O_CREAT | O_RDWR, 0644);
}

// Обертка для закрытия файла
int lab2_close(int fd) {
    roulette();  // Русская рулетка при закрытии файла
    return close(fd);
}

// Обертка для чтения файла
ssize_t lab2_read(int fd, void *buf, size_t count) {
    handle_slot_machine();  // Слот-машина перед чтением

    off_t offset = lseek(fd, 0, SEEK_CUR);
    CacheBlock *cached_block = cache_find(offset);
    if (cached_block) {
        memcpy(buf, cached_block->data, count);
        cache_move_to_front(cached_block);
        return count;
    } else {
        ssize_t bytes_read = read(fd, buf, count);
        if (bytes_read > 0) {
            cache_add(offset, buf);
        }
        return bytes_read;
    }
}

// Обертка для записи файла
ssize_t lab2_write(int fd, const void *buf, size_t count) {
    handle_slot_machine();  // Слот-машина перед записью

    off_t offset = lseek(fd, 0, SEEK_CUR);
    CacheBlock *cached_block = cache_find(offset);
    if (cached_block) {
        memcpy(cached_block->data, buf, count);
    } else {
        cache_add(offset, buf);
    }
    return write(fd, buf, count);
}

// Обертка для синхронизации файла
int lab2_fsync(int fd) {
    handle_slot_machine();  // Слот-машина перед синхронизацией
    return fsync(fd);
}
off_t lab2_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}