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
int free_operations = 0;
int allow_bet = 0;

void one_arm_bandit(char *result) {
    for (int i = 0; i < 3; i++) {
        strcpy(&result[i * 4], symbols[rand() % 3]);
    }
    result[12] = '\0';
}

void play_media(const char *file, const char *intf, const char *play_and_exit) {
    char command[256];
    snprintf(command, sizeof(command), "vlc --intf %s %s %s > /dev/null 2>&1 &", intf, file, play_and_exit);
    system(command);
}

void handle_slot_machine();
void play_red_light_green_light();

void handle_slot_machine() {
    int bet_op = 0;
    char bet = 0;
    if (free_operations == 0 && (allow_bet == 0 || allow_bet == 2)){
      if (allow_bet == 0) {
        printf("Хотите делать ставки? <y/n>: \n");
        scanf("%c", &bet);
        if (bet == 'n') {
          allow_bet = 1;
        } else {
          allow_bet = 2;
        }
        bet = 0;
      }
      if (allow_bet != 1) {
        printf("Введите ставку: <y/n> <число операций>:\n");
        scanf("%c %d", &bet, &bet_op);
      }
    }
    
    char bandit_result[13];
    one_arm_bandit(bandit_result);
    printf("🎰 Слот-машина: %s\n", bandit_result);
    if (free_operations > 0) {
      printf("✨ Операция проходит мгновенно.\n");
      free_operations -= 1;
    } else if (free_operations < 0) {
      printf("💩 Операция будет медленной...\n");
      sleep(2);
      free_operations += 1;
    } else {
      if (strcmp(bandit_result, "💎💎💎") == 0 || 
          strcmp(bandit_result, "💩💩💩") == 0 || 
          strcmp(bandit_result, "🔒🔒🔒") == 0) {
          printf("✨ Удача! Операция проходит мгновенно.\n");
          if (bet == 'y') {
            free_operations = bet_op-1;
            printf("Ставка сыграла\n");
          } else if (bet == 'n') {
            free_operations = -bet_op+1;
            play_media("casino.mp4", "dummy", "");
          }
          play_media("GAMBLECORE.mp4", "dummy", "--play-and-exit");
      } else {
          printf("💩 Неудача! Операция будет медленной...\n");
          if (bet == 'n') {
            free_operations = bet_op-1;
            printf("Ставка сыграла\n");
          } else if (bet == 'y'){
            free_operations = -bet_op;
            play_media("casino.mp4", "dummy", "");
          }
          play_media("dang-it.mp3", "dummy", "--play-and-exit");
          
          sleep(2);
      }
    }
}

int roulette() {
    int result = rand() % 6;
    if (result == 0) {
        printf("🔫 Рулетка: Плохая удача! Вы мертвы.\n");
        play_media("roulette.mp4", "dummy", "--play-and-exit");
        exit(EXIT_FAILURE);
    }
    printf("🔫 Рулетка: А вы везунчик.\n");
    return result;
}

int lab2_open(const char *path) {
    roulette();  
    return open(path, O_CREAT | O_RDWR , 0644);
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
