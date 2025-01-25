#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

const char* symbols[] = {"💎", "💩", "🔒"};

void one_arm_bandit(char *result) {
    for (int i = 0; i < 3; i++) {
        result[i] = symbols[rand() % 3][0];
    }
    result[3] = '\0'; 
}

int roulette() {
    return rand() % 6;
}


ssize_t lab2_read(int fd, void *buf, size_t count) {
    off_t offset = lseek(fd, 0, SEEK_CUR);

    char bandit_result[4];
    one_arm_bandit(bandit_result);
    printf("🎰 Слот-машина: %s\n", bandit_result);
	
    if (strcmp(bandit_result, "💎💎💎") == 0 || strcmp(bandit_result, "💩💩💩") == 0 || strcmp(bandit_result, "🔒🔒🔒") == 0) {
        printf("✨ Слот-машина дала удачу! Операция проходит мгновенно.\n");
    }else {
        printf("💩 Слот-машина дала неудачу! Операция будет медленной...\n");
        sleep(2);
    }


    CacheBlock *cached_block = cache_find(offset);
    if (cached_block) {
        memcpy(buf, cached_block->data, count);
        cache_move_to_front(cached_block);
        return count;
    } else {
        int result = roulette();
        if (result == 5) {
            printf("🔫 Рулетка: Плохая удача! Вы мертвы.\n");
			exit(EXIT_FAILURE);
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

    char bandit_result[4];
    one_arm_bandit(bandit_result);
    printf("🎰 Слот-машина: %s\n", bandit_result);

    if (strcmp(bandit_result, "💎💎💎") == 0 || strcmp(bandit_result, "💩💩💩") == 0 || strcmp(bandit_result, "🔒🔒🔒") == 0) {
        printf("✨ Слот-машина дала удачу! Операция проходит мгновенно.\n");
    }else {
        printf("💩 Слот-машина дала неудачу! Операция будет медленной...\n");
        sleep(2);
    }

    CacheBlock *cached_block = cache_find(offset);
    if (cached_block) {
        memcpy(cached_block->data, buf, count);
    } else {
        cache_add(offset, buf); 
    }

    int result = roulette();
    if (result == 5) {
        printf("🔫 Рулетка: Плохая удача! Вы мертвы.\n");
        exit(EXIT_FAILURE);
    }

    return write(fd, buf, count);
}


int lab2_fsync(int fd) {

    char bandit_result[4];
    one_arm_bandit(bandit_result);
    printf("🎰 Слот-машина: %s\n", bandit_result);
    if (strcmp(bandit_result, "💎💎💎") == 0 || strcmp(bandit_result, "💩💩💩") == 0 || strcmp(bandit_result, "🔒🔒🔒") == 0) {
        printf("✨ Слот-машина дала удачу! Операция проходит мгновенно.\n");
    }else {
        printf("💩 Слот-машина дала неудачу! Операция будет медленной...\n");
        sleep(2);
    }

    return fsync(fd);
}
