#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Constants
#define VIRTUAL_MEMORY_SIZE 128
#define MAIN_MEMORY_SIZE 32
#define PAGE_SIZE 8
#define NUM_VIRTUAL_PAGES (VIRTUAL_MEMORY_SIZE / PAGE_SIZE)
#define NUM_MAIN_PAGES (MAIN_MEMORY_SIZE / PAGE_SIZE)

char command[128] = "";
int fifo = 0, lru = 0;
int accessOrder[4];
int rear = 0;

// Structure to represent a page table entry
struct PageTable {
  int validBit;
  int dirtyBit;
  int m_pageNumber;
  int v_pageNumber;
};

struct Memory {
  int address, data, page;
};

struct Memory main_memory[4][8];
struct Memory virtual_memory[128];
struct PageTable p_table[16];

void init() {
  int value = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++) {
      // main_memory[i][j].address = value++;
      main_memory[i][j].data = -1;
      main_memory[i][j].page = i;
      // main_memory[i][j].timeStamp = 0;
    }
  }

  for (int i = 0; i < 16; i++) {
    p_table[i].v_pageNumber = i;
    p_table[i].validBit = 0;
    p_table[i].dirtyBit = 0;
    // p_table[i].timeStamp = 0;
    p_table[i].m_pageNumber = i;
  }
  for (int i = 0; i < 128; i++) {
    virtual_memory[i].address = i;
    virtual_memory[i].data = -1;
  }
}

int pageReplacement(int va) {
  // printf("Hi \n");
  int vp = (int)(va / 8);
  int popped = accessOrder[0];
  int poppedm = p_table[popped].m_pageNumber;
  for (int i = 0; i < 3; i++) {
    accessOrder[i] = accessOrder[i + 1];
  }
  accessOrder[3] = vp;
  for (int i = 0; i < 8; i++) {
    virtual_memory[popped * 8 + i].data = main_memory[poppedm][i].data;
  }
  p_table[popped].validBit = 0;
  p_table[popped].dirtyBit = 0;
  p_table[popped].m_pageNumber = popped;
  p_table[vp].validBit = 1;
  p_table[vp].m_pageNumber = poppedm;

  for (int j = 0; j < 8; j++) {
    main_memory[poppedm][j].address = (vp * 8 + j);
    main_memory[poppedm][j].data = virtual_memory[vp * 8 + j].data;
  }

  return poppedm;
}

int mainIndex = 0;

void readfunc(int va) {
  int v_pagNumber = (int)(va / 8);
  int offset = va % 8;
  if (p_table[v_pagNumber].validBit == 0) {
    printf("A Page Fault Has Occurred \n");
    p_table[v_pagNumber].validBit = 1;
    if (rear == 4) {
      pageReplacement(va);
    } else {
      accessOrder[rear] = v_pagNumber;
      rear++;
      int value = v_pagNumber * 8;
      for (int i = 0; i < 8; i++) {
        main_memory[mainIndex][i].address = value++;
      }
      p_table[v_pagNumber].m_pageNumber = mainIndex;
      for (int j = 0; j < 8; j++) {
        if (main_memory[mainIndex][j].address == va) {
          printf("%d\n", main_memory[mainIndex][j].data);
          break;
        }
      }

      mainIndex++;
    }
  } else {
    if (lru == 1) {
      int pos;
      if (accessOrder[rear - 1] != (int)(va / 8)) {
        for (int i = 0; i < rear; i++) {
          if (accessOrder[i] == (int)(va / 8)) {
            int temp = accessOrder[i];
            for (int j = i; j < rear - 1; j++) {
              accessOrder[j] = accessOrder[j + 1];
            }
            accessOrder[rear - 1] = (int)(va / 8);
            break;
          }
        }
      }
    }
    int m_pageNumber = p_table[v_pagNumber].m_pageNumber;
    for (int i = 0; i < 8; i++) {
      if (main_memory[m_pageNumber][i].address == va) {
        printf("%d\n", main_memory[m_pageNumber][i].data);
        break;
      }
    }
  }
}

void writefunc(int va, int val) {
  int v_pageNumber = (int)(va / 8);
  int offset = va % 8;
  p_table[v_pageNumber].dirtyBit = 1;
  if (p_table[v_pageNumber].validBit == 0) {
    printf("A Page Fault Has Occurred \n");
    p_table[v_pageNumber].validBit = 1;
    if (rear == 4) {
      int m_page = pageReplacement(va);
      for (int i = 0; i < 8; i++) {
        if (main_memory[m_page][i].address == va) {
          main_memory[m_page][i].data = val;
        }
      }
    } else {
      accessOrder[rear] = v_pageNumber;
      rear++;
      // int m_pageNumer = p_table[v_pageNumber].m_pageNumber;
      int value = v_pageNumber * 8;
      for (int i = 0; i < 8; i++) {
        main_memory[mainIndex][i].address = value++;
      }
      for (int i = 0; i < 8; i++) {
        if (main_memory[mainIndex][i].address == va) {
          main_memory[mainIndex][i].data = val;
          break;
        }
      }
      // main_memory[mainIndex][va].data = val;
      p_table[v_pageNumber].m_pageNumber = mainIndex;
      mainIndex++;
    }
  } else {
    if (lru == 1) {
      int pos;
      if (accessOrder[rear - 1] != (int)(va / 8)) {
        for (int i = 0; i < rear; i++) {
          if (accessOrder[i] == (int)(va / 8)) {
            int temp = accessOrder[i];
            for (int j = i; j < rear - 1; j++) {
              accessOrder[j] = accessOrder[j + 1];
            }
            accessOrder[rear - 1] = (int)(va / 8);
            break;
          }
        }
      }
    }
    int m_pageNumer = p_table[v_pageNumber].m_pageNumber;
    for (int i = 0; i < 8; i++) {
      if (main_memory[m_pageNumer][i].address == va) {
        main_memory[m_pageNumer][i].data = val;
        break;
      }
    }
  }
}

void showmainfunc(int pp) {
  if (pp >= 0 && pp < 4) {
    for (int i = 0; i < 8; i++) {
      printf("%d: %d\n", (pp * 8 + i), main_memory[pp][i].data);
    }
  }
}

void showptablefunc() {
  for (int i = 0; i < NUM_VIRTUAL_PAGES; i++) {
    printf("%d:%d:%d:%d\n", p_table[i].v_pageNumber, p_table[i].validBit,
           p_table[i].dirtyBit, p_table[i].m_pageNumber);
  }
  /*printf("Access Order List \n");
  for (int i = 0; i < rear; i++) {
    printf("%d ", accessOrder[i]);
  }
  printf("\n");*/
}

void loop() {
  do {
    char input[100];
    char args1[128] = "";
    char args2[128] = "";
    printf("> ");
    fgets(input, 80, stdin);
    int command_count = sscanf(input, "%s %s %s", command, args1, args2);
    /*printf("command - %s ",command);
    printf("args1 - %s ",args1);
    printf("args2 - %s ",args2);
    //printf("command - %s ",args);*/
    if (strcmp(command, "read") == 0) {
      readfunc(atoi(args1));
    } else if (strcmp(command, "write") == 0) {
      writefunc(atoi(args1), atoi(args2));
    } else if (strcmp(command, "showmain") == 0) {
      // showmainfunc(atoi(args1));
      showmainfunc(atoi(args1));
    } else if (strcmp(command, "showptable") == 0) {
      showptablefunc();
    } else if (strcmp(command, "quit") == 0) {
      break;
    } else {
      printf("Invalid Command\n");
    }
  } while (1);
}

int main(int argc, char** argv) {
  if (argv[1] == NULL || strcmp(argv[1], "FIFO") == 0)
    fifo = 1;
  else if (strcmp(argv[1], "LRU") == 0)
    lru = 1;
  // printf("LRU detected \n ");
  init();
  loop();
  return 0;
}