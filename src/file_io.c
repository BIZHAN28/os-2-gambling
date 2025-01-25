#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include "file_io.h"
#include "cache.h"

const char* symbols[] = {"\U0001F48E", "\U0001F4A9", "\U0001F512"};

void one_arm_bandit(char *result) {
    for (int i = 0; i < 3; i++) {
        strcpy(&result[i * 4], symbols[rand() % 3]);
    }
    result[12] = '\0';
}

void play_media(const char *file, const char *intf) {
    char command[256];
    snprintf(command, sizeof(command), "vlc --intf %s --play-and-exit %s > /dev/null 2>&1 &", intf, file);
    system(command);
}

void handle_slot_machine();
void play_red_light_green_light();

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
        
        if (rand() % 10 == 0) { // 10% шанс
            play_red_light_green_light();
        } else {
            sleep(2);
        }
    }
}

void play_red_light_green_light() {
    int distance = 500; 
    int light_duration;
    int is_red_light = rand() % 2; 
    time_t light_start, now;
    struct termios oldt, newt;

    printf("\n🚦 Игра \"Красный свет/Зелёный свет\" начинается! Пройдите 500 метров.\n");
    printf("🔴 Красный свет - можно идти\n🟢 Зелёный свет - нельзя идти\n");
    
    // Настройка для чтения клавиш
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (distance > 0) {
        light_duration = 5 + rand() % 6; // Длительность света от 5 до 10 секунд
        time(&light_start);

        printf("\n%s Свет горит %s на %d секунд.\n",
               is_red_light ? "🔴" : "🟢",
               is_red_light ? "красный" : "зелёный",
               light_duration);

        while (1) {
            time(&now);

            // Если время текущего света истекло
            if (difftime(now, light_start) >= light_duration) {
                is_red_light = !is_red_light; // Меняем свет
                break;
            }

            // Проверяем нажатие клавиши пробел
            if (is_red_light && getchar() == ' ') {
                distance -= 10;
                printf("🚶‍♂️ Вы прошли 10 метров. Осталось: %d метров.\n", distance);
                if (distance <= 0) break;
            } else if (!is_red_light && getchar() == ' ') {
                printf("❌ Вас заметили! Игра окончена.\n");
                play_media("failure.mp3", "dummy");
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("🎉 Поздравляем! Вы успешно прошли дистанцию.\n");
    play_media("success.mp3", "dummy");

    // Возвращаем старые настройки терминала
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

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

int lab2_open(const char *path) {
    roulette();  
    return open(path, O_CREAT | O_RDWR, 0644);
}

int lab2_close(int fd) {
    roulette();  
    return close(fd);
}

ssize_t lab2_read(int fd, void *buf, size_t count) {
    handle_slot_machine();

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

ssize_t lab2_write(int fd, const void *buf, size_t count) {
    handle_slot_machine();

    off_t offset = lseek(fd, 0, SEEK_CUR);
    CacheBlock *cached_block = cache_find(offset);
    if (cached_block) {
        memcpy(cached_block->data, buf, count);
    } else {
        cache_add(offset, buf);
    }
    return write(fd, buf, count);
}

int lab2_fsync(int fd) {
    handle_slot_machine();
    return fsync(fd);
}

off_t lab2_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}
