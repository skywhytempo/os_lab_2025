#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>
#include "find_min_max.h"
#include "utils.h"

//Запуск: ./parallel_min_max --seed 42 --array_size 1000000 --pnum 4

static bool timeout_happened = false;

void SigAlarmHandler(int sig) {
  timeout_happened = true;
}


int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;
  int timeout = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if (seed <= 0) {
              printf("seed is a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling 
            if (array_size <= 0) {
              printf("array_size is a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling  
            if (pnum <= 0) {
              printf("pnum is a positive number\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          case 4:
            timeout = atoi(optarg);
            // your code here
            // error handling 
            if (timeout <= 0) {
              printf("timeout is a positive number\n");
              return 1;
            }
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  //каждому дочернему процессу - по пайпу
  int pipefd[pnum][2];
  int chunk_size = array_size / pnum;

  pid_t child_pids[pnum];

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (int i = 0; i < pnum; i++) {

    if (!with_files) {
      if (pipe(pipefd[i]) == -1) {
        printf("Pipe failed!\n");
        return 1;
      }
    }
    
    pid_t child_pid = fork();

    if (child_pid >= 0) {
      // Успешный форк
      active_child_processes += 1;
      if (child_pid == 0) {
      
        // Дочерний процесс

        unsigned int begin = i * chunk_size;
        unsigned int end = (i == pnum - 1) ? array_size : (i + 1) * chunk_size;
        struct MinMax local_min_max = GetMinMax(array, begin, end);

        if (with_files) {

          char filename[256];
          sprintf(filename, "result_%d.txt", i);

          FILE *file = fopen(filename, "w");
          if (file == NULL) {
            printf("Could not open file for writing\n");
            return 1;
          }
          fprintf(file, "%d %d", local_min_max.min, local_min_max.max);
          fclose(file);

        } else {
          close(pipefd[i][0]);
          write(pipefd[i][1], &local_min_max, sizeof(struct MinMax));
          close(pipefd[i][1]);
        }

        free(array);
        return 0;
      } else {
        // Родительский процесс
        child_pids[i] = child_pid;
        if (!with_files) {
          close(pipefd[i][1]);
        }
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  // Настройка таймаута при помощи сигналов
  if (timeout > 0) {
    signal(SIGALRM, SigAlarmHandler);
    alarm(timeout);
  }


  // Ожидаем дочерние процессы
  while (active_child_processes > 0) {
    if (timeout_happened) {
      printf("\n[TIMEOUT] Time limit (%d sec) exceeded! Sending SIGKILL to children...\n", timeout);
      for (int i = 0; i < pnum; i++) {
        kill(child_pids[i], SIGKILL);
      }
      // Собираем завершенные процессы
      while (active_child_processes > 0) {
        waitpid(-1, NULL, 0);
        active_child_processes -= 1;
      }
      printf("All children terminated by timeout.\n");
      free(array);
      //if (!with_files);
      return 1;
    }

    if (timeout > 0) {
      // Неблокирующее ожидание при заданном таймауте
      pid_t pid = waitpid(-1, NULL, WNOHANG);
      if (pid > 0) {
        active_child_processes--;
      } else {
        usleep(10000); // 10 мс пауза, чтобы не нагружать процессор
      }
    } else {
      // Обычное блокирующее ожидание, если таймаут не задан
      wait(NULL);
      active_child_processes -= 1;
    }
  }

    // Отключаем таймер, если процессы успели завершиться вовремя
  if (timeout > 0) {
    alarm(0);
  }

  // Из локальных минмакс собираем глобальный
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {

      char filename[256];
      sprintf(filename, "result_%d.txt", i);

      FILE *file = fopen(filename, "r");
      if (file == NULL) {
        printf("Could not open file for reading\n");
        return 1;
      }
      fscanf(file, "%d %d", &min, &max);
      fclose(file);
      remove(filename);

    } else {

      struct MinMax local_min_max;
      read(pipefd[i][0], &local_min_max, sizeof(struct MinMax));
      close(pipefd[i][0]);
      min = local_min_max.min;
      max = local_min_max.max;
      
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
