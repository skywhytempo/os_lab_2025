#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Дочерний процесс завершается мгновенно
        printf("Дочерний процесс, PID: %d. Завершается\n", getpid());
        exit(0);
    } else {
        // Родитель НЕ вызывает wait(), а просто спит 20 секунд
        printf("Родительский процесс, PID: %d. Дочерний процесс(зомби): %d\n", getpid(), pid);
        printf("Родительский процесс спит. Дочерний процесс теперь зомби!\n");
        sleep(30);
        printf("Родительский процесс завершается.\n");
    }
    /*
        PID    PPID STAT CMD
        sitei@skywhytempo:/mnt/c/Users/igore/Downloads/me/Science/stud/ОС и сети/os_lab_2025/lab4/src$ ps -o pid,ppid,stat,cmd -p 780,781
            PID    PPID STAT CMD
            780     342 S+   ./zombie
            781     780 Z+   [zombie] <defunct>
    */

    return 0;
}