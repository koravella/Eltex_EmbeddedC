#include <stdio.h>
#include <string.h>

#define kSize 100
#define kLenName 11
#define kLenSurname 11
#define kLenPhone 11

struct subscriber {
  char name[kLenName];
  char surname[kLenSurname];
  char phone[kLenPhone];
};

int main(void) {
  struct subscriber directory[kSize];
  int used[kSize] = { 0 };
  char str_format[] = "%-3s  %-10s  %-10s  %-10s\n";

  int flag = 1;
  while(flag) {
    puts("1) Add the subscriber's phone number");
    puts("2) Output the directory");
    puts("3) Find the subscriber's phone number");
    puts("4) Remove the subscriber's phone number");
    puts("5) Quit");
    printf("Select an action: ");
    int input;
    scanf("%d", &input);

    switch(input) {
      case 1: {
        int vacant;
        for (vacant = 0; vacant < kSize; vacant++) {
          if (used[vacant] == 0) {
            used[vacant] = 1;
            break;
          }
        }
        if (vacant == kSize) {
          puts("The directory is full");
        }
        printf("Enter name: ");
        scanf("%s", directory[vacant].name); 
        printf("Enter surname: ");
        scanf("%s", directory[vacant].surname);
        printf("Enter phone: ");
        scanf("%s", directory[vacant].phone);
        break;
      }
      case 2: {
        printf(str_format,"id", "name", "surname", "phone");
        for (int i = 0; i < kSize; i++) {
          if (used[i] == 1) {
            printf(str_format, i, directory[i].name,
                   directory[i].surname, directory[i].phone);
          }
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
        printf(str_format, "id", "name", "surname", "phone");
        for (int i = 0; i < kSize; i++) {
          if (used[i] == 1 &&
              strcmp(directory[i].name, name) == 0 &&
              strcmp(directory[i].surname, surname) == 0) {
            printf(str_format, i, directory[i].name,
                   directory[i].surname, directory[i].phone);
          }
        }
        break;
      }
      case 4: {
        size_t id;
        printf("Enter id: ");
        scanf("%zu", &id);
        used[id] = 0;
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

