#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_KEY 2147483647
#define MIN_KEY -2147483648
#define BUFFERSIZE 10240000

typedef int loser_tree;

typedef struct {
  int key;
} External;

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

extern inline void Adjust(loser_tree ls[], int s, External b[], int k) {
  int t = (s + k) >> 1;
  while (t > 0) {
    // compare which of leaf and the index stored in parent is bigger and adjust
    // loser's index s store winner's index
    if (b[s].key > b[ls[t]].key) {
      s ^= ls[t];
      ls[t] ^= s;
      s ^= ls[t];
    }
    t = t >> 1;
  }

  // store winner's index in ls[0]
  ls[0] = s;
}

// create loser tree
void CreateLoserTree(loser_tree ls[], External b[], int k) {
  b[k].key = MIN_KEY;

  // initial loser tree
  for (int i = 0; i < k; i++) {
    ls[i] = k;
  }

  // adjust every loser's index
  for (int i = k - 1; i >= 0; i--) {
    Adjust(ls, i, b, k);
  }
}

extern inline void input(int i, External b[], int k, FILE *fp) {
  char *buf;

  buf = malloc(13);
  if (fgets(buf, 13, fp))
    b[i].key = atoi(buf);
  else
    b[i].key = MAX_KEY;

  free(buf);
}

void k_way_merge_sort(loser_tree ls[], External b[], int k) {
  FILE *fp[k];

  for (int i = 0; i < k; ++i) {
    char *str = malloc(13);
    snprintf(str, 13, "%s", "output_");

    char file_num[10];
    snprintf(file_num, 11, "%d", i + 1);

    strcat(str, file_num);
    strcat(str, ".txt");

    fp[i] = fopen(str, "r");
    if (!fp[i]) {
      perror(str);
      return;
    }

    free(str);
  }

  for (int i = 0; i < k; i++)
    input(i, b, k, fp[i]);

  // create loser tree
  CreateLoserTree(ls, b, k);

  FILE *final;

  final = fopen("output.txt", "w");
  if (!final) {
    perror("output.txt");
    return;
  }

  // set buffer for fprintf
  unsigned char *buf2;
  buf2 = malloc(BUFFERSIZE);
  setvbuf(final, buf2, _IOFBF, BUFFERSIZE);

  while (b[ls[0]].key != MAX_KEY) {
    // print result
    fprintf(final, "%d\n", b[ls[0]].key);

    input(ls[0], b, k, fp[ls[0]]);

    // adjust loser tree
    Adjust(ls, ls[0], b, k);

  }
  // flush buffer
  fflush(final);

  for (int i = 0; i < k; ++i) {
    fclose(fp[i]);
  }

  for (int i = 0; i < k; ++i) {
    char *str = malloc(13);
    snprintf(str, 13, "%s", "output_");

    char file_num[10];
    snprintf(file_num, 11, "%d", i + 1);

    strcat(str, file_num);
    strcat(str, ".txt");

    // remove temp file
    remove(str);

    free(str);
  }

  free(buf2);
  fclose(final);
}

extern inline void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

extern inline int Partition(int *arr, int front, int end) {
  int pivot = arr[front];
  int tmp = pivot;

  while (front < end) {
    while (front < end && arr[end] >= pivot)
      --end;

    // use assignment instead of swap
    arr[front] = arr[end];

    while (front < end && arr[front] <= pivot)
      ++front;

    // use assignment instead of swap
    arr[end] = arr[front];
  }

  arr[front] = tmp;
  return front;
}

void quick_sort(int *arr, int64_t front, int64_t end) {
  if (front >= end)
    return;

  int pivot = Partition(arr, front, end);
  quick_sort(arr, front, pivot - 1);
  quick_sort(arr, pivot + 1, end);
}

int create_sub_txt(FILE *fp, u_int64_t filelen, int num) {

  char **buf;
  int64_t size = sizeof(char *) * filelen;
  buf = malloc(size);

  for (int i = 0; i < filelen - 1; ++i) {

    buf[i] = malloc(13);
    if (!buf[i]) {
      perror("malloc");
      return -1;
    }
    
    // getline
    fgets(buf[i], 13, fp);
  }

  int *unsorted_data;
  unsorted_data = malloc(sizeof(int) * (filelen - 1));

  for (u_int64_t i = 0; i < filelen - 1; ++i) {

    // string to integer
    unsorted_data[i] = atoi(buf[i]);
    free(buf[i]);
  }

  free(buf);

  // sort
  quick_sort(unsorted_data, 0, (int64_t)filelen - 2);

  FILE *fp1;

  int len = 0;
  while (num >> len)
    ++len;

  char file_num[len + 1];
  snprintf(file_num, len + 1, "%d", num);

  // create temp file
  char *str = malloc(13);
  sprintf(str, "%s", "output_");

  strcat(str, file_num);
  strcat(str, ".txt");
  fp1 = fopen(str, "w+");
  if (!fp1) {
    perror("fopen");
    return 0;
  }

  // set buffer for fprintf
  unsigned char *buf2;
  buf2 = malloc(BUFFERSIZE);
  setvbuf(fp1, buf2, _IOFBF, BUFFERSIZE);

  for (u_int64_t j = 0; j < filelen - 1; ++j) 
    fprintf(fp1, "%d\n", unsorted_data[j]);

  // flush buffer 
  fflush(fp1);

  free(unsorted_data);
  free(str);
  free(buf2);
  fclose(fp1);
  return 1;
}

int main(int argc, char *argv[]) {

  struct timespec start, end;
  double time_used;

  // get start time
  clock_gettime(CLOCK_MONOTONIC, &start);

  FILE *fp;

  fp = fopen("input.txt", "r");
  if (!fp) {
    perror("fopen");
    return -1;
  }
  u_int64_t lines = 0;

  // counting rows
  // 32.8GB test file cost about 120sec
  char c;
  while ((c = fgetc(fp)) != EOF)
    if (c == '\n')
      lines++;
  rewind(fp);
  printf("line = %lu\n", lines);

  unsigned int count = lines / 200000000 + 1;
  u_int64_t data_num[2];

  if (!lines % count)
      data_num[0] = lines / count + 1;
  else
      data_num[0] = lines / count;

  for (int i = 0; i < count; ++i) 
    if (i < count - 1)
      create_sub_txt(fp, data_num[0] + 1, i + 1);

  if (count > 1 && lines % count != 0)
    data_num[1] = lines - data_num[0] * (count - 1);
  else
    data_num[1] = lines / count;
  create_sub_txt(fp, data_num[1] + 1, count);

  External b[count + 1];

  fclose(fp);

  loser_tree ls[count];
  k_way_merge_sort(ls, b, count);

  // get finish time
  clock_gettime(CLOCK_MONOTONIC, &end);

  // counting time
  struct timespec temp = diff(start, end);
  time_used = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;

  printf("Time = %f\n", time_used);

  return 0;
}