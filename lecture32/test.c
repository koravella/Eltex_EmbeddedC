#define _DEFAULT_SOURCE
       #include <dirent.h>
       #include <stdio.h>
       #include <stdlib.h>

       int
       main(void)
       {
           struct dirent **namelist;
           int n;

           n = scandir("/home/denisdesktop/MyGithub/Eltex_EmbeddedC", &namelist, NULL, alphasort);
           if (n == -1) {
               perror("scandir");
               exit(EXIT_FAILURE);
           }

           while (n--) {
               printf("%s\n", namelist[n]->d_name);
               free(namelist[n]);
           }
           free(namelist);

           exit(EXIT_SUCCESS);
       }
