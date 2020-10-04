#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct timespec diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp;
}

int create_txt(size_t s) { // size
  FILE *fp;

  fp = fopen("input.txt", "w");
  if (!fp) {
    perror("fopen");
    return 0;
  }
  unsigned char *buf;
  buf = malloc(10240000);
  setvbuf(fp, buf, _IOFBF, 10240000);

  int len = 0;
  int num = 0;

  for (size_t i = 0; i < s; i++) {

    num = rand();
    len = 0;

    for (int i = num; i > 0; i /= 10)
      ++len;

    if (rand() % 2) {
      num *= -1;
      ++len;
    }

    fprintf(fp, "%d\n", num);
  }
  fflush(fp);

  fclose(fp);

  return 1;
}

int main(int argc, char *argv[]) {
  // 儲存時間用的變數
  struct timespec start, end;
  double time_used;

  // 計算開始時間
  clock_gettime(CLOCK_MONOTONIC, &start);

  size_t size = 32 * 1024;
  size *= 1024 * 1024 / 11.5;

  create_txt(size / 4);

  // 計算結束時間
  clock_gettime(CLOCK_MONOTONIC, &end);

  // 計算實際花費時間
  struct timespec temp = diff(start, end);
  time_used = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;

  printf("Time = %f\n", time_used);

  return 0;
}