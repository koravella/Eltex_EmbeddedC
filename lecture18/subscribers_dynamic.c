#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdlib.h>

#define kLenName 11
#define kLenSurname 11
#define kLenPhone 11

struct subscriber {
  char name[kLenName];
  char surname[kLenSurname];
  char phone[kLenPhone];
};

int main(void) {
  struct subscriber* dir = NULL;
  int size_subscr = sizeof (struct subscriber);
  int size = 0;
  char format_title[] = "%-3s  %-10s  %-10s  %-10s\n";
  char format_line[] = "%-3d  %-10s  %-10s  %-10s\n";


  int flag = 1;
  while(flag) {
    puts("\n1) Add the subscriber's phone number");
    puts("2) Output the directory");
    puts("3) Find the subscriber's phone number");
    puts("4) Remove the subscriber's phone number");
    puts("5) Quit");
    printf("Select an action: ");
    int input;
    scanf("%d", &input);
    printf("\n");

    switch(input) {
      case 1: {
        if((dir = (struct subscriber*) realloc(dir, ++size * size_subscr))) {
          printf("Enter name: ");
          scanf("%s", dir[size - 1].name); 
          printf("Enter surname: ");
          scanf("%s", dir[size - 1].surname);
          printf("Enter phone: ");
          scanf("%s", dir[size - 1].phone);
          printf("Successfully added!\n");
        }
        else {
          perror("Out of memory!\n");
          exit(EXIT_FAILURE);
        }
        break;
      }
      case 2: {
        printf(format_title,"id", "name", "surname", "phone");
        for (size_t i = 0; i < size; i++) {
          printf(format_line, i, dir[i].name, dir[i].surname, dir[i].phone);
        }
        break;
      }
      case 3:{
        char name[kLenName];
        char surname[kLenSurname];
        printf("Enter name: ");
        scanf("%s", name); 
        printf("Enter surname: ");
        scanf("%s", surname);
        printf("Found phone numbers:\n");
        printf(format_title, "id", "name", "surname", "phone");
        for (size_t i = 0; i < size; i++) {
          if (strcmp(dir[i].name, name) == 0 &&
              strcmp(dir[i].surname, surname) == 0) {
            printf(format_line, i, dir[i].name, dir[i].surname, dir[i].phone);
          }
        }
        break;
      }
      case 4: {
        size_t id;
        printf("Enter id: ");
        scanf("%zu", &id);
        if (id < 0 || id >= size) {
          printf("The record does not exist!\n");
        }
        else {
          for (size_t i = id + 1; i < size; i++) {
            strcpy(dir[i - 1].name, dir[i].name);
            strcpy(dir[i - 1].surname, dir[i].surname);
            strcpy(dir[i - 1].phone, dir[i].phone);
          }
          if (size == 1) {
            free(dir);
            --size;
            dir = NULL;
            printf("Successfully deleted!\n");
          }
          else if ((dir = (struct subscriber*) realloc(dir, --size * size_subscr))) {
            printf("Successfully deleted!\n");
          }
          else {
            perror("Realloc failed!\n");
            exit(EXIT_FAILURE);
          }
        }
        break;
      }
      case 5: {
        flag = 0;
        continue;
      }
      default: {
        puts("Wrong selection");
        continue;
      }
    }
  }
  return 0;
}
