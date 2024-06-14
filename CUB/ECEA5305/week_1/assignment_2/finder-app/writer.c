// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <syslog.h>
//
// // You do not need to make your "writer" utility create directories which do not exist.  You can assume the directory is created by the caller.
//
// // Setting up syslog function
// void setup_syslog() {
//   openlog("writer.c", LOG_PID | LOG_NDELAY, LOG_USER); // Replace "my_program" with your program name
// }
//
// // Main function entrypoint
// int main(int argc, char *argv[]){
//
//   setup_syslog();
//
//   if(argc < 2){
//     printf("Not enough arguments");
//   }
//   // Extract commandline arguments passed to executable
//   const char* writeFile = argv[0];
//   const char* writeStr = argv[1];
//
//
//   syslog(LOG_DEBUG, "Writing %s to %s", writeStr, writeFile);
//
//   // Initialize file pointer
//   FILE *fp = fopen(writeFile, "w");
//
//
//   // Error checks if file pointer is invalid
//   if (fp == NULL) {
//   perror("Error opening file");
//   return 1;  // Indicate error
//   }
//
//   // Write string to the file
//   int bytes_written = fprintf(fp, "%s", writeStr);
//
//   // Error checking for write
//   if (bytes_written < 0) {
//     perror("Error writing to file");
//     fclose(fp);
//     return 1;
//   }
//
//   // Close file pointer and output success to console
//   fclose(fp);
//   // Close syslog
//   closelog();
//   printf("String written successfully!\n");
//   return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

void setup_syslog(){
  printf("Setting up syslog\n");
  openlog("writer.c", LOG_PID|LOG_CONS, LOG_USER);
}


int main(int argc, char *argv[]) {
    // openlog("writer", LOG_PID|LOG_CONS, LOG_USER);

    setup_syslog();

    if (argc != 3) {
        syslog(LOG_ERR, "Error: Two arguments required: <file_path> <text_string>");
        closelog();
        return 1;
    }

    char *writeFile = argv[1];
    char *writeStr = argv[2];
    FILE *file = fopen(writeFile, "w");

    if (file == NULL) {
    perror("Error opening file");
    return 1;  // Indicate error
    }

    // fputs(writestr, file);
    int bytes_written = fprintf(file, "%s", writeStr);

    syslog(LOG_DEBUG, "Writing '%s' to '%s'", writeStr, writeFile);

    fclose(file);
    closelog();

    return 0;
}
