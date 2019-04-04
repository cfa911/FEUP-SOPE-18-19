#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>

#include <errno.h>

#include <sys/wait.h>



typedef struct {
								char * name;
								int print;
								int delete;
								int exec;
								int perm;
								int hasType;
								int hasName;
								int hasPerm;
								int hasMode;
								char * type;
								int mode;
}Options;

void sigint_handler(int signo){

  // char terminate;

  kill(getppid(),SIGSTOP);

  // printf("\n> DO YOU WISH TO TERMINATE?\n");
  // scanf("%s",&terminate);
  //
  // terminate = (char)toupper(terminate);
  //
  // if(terminate == 'N'){
  //   kill(getppid(),SIGCONT);
  // }
  // else{
    kill(getppid(),SIGQUIT);
    kill(getpid(),SIGQUIT);
  // }

}

void subscribe_SIGINT(){

  struct sigaction act;
  act.sa_handler = sigint_handler;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);

  sigaction(SIGINT,&act,NULL);

}
void printFileInfo(char * name){
  // FILE *commandFile, *commandHash;
  // char command[512] = "file ";
  // //file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256
  //
  // strcat(command, name); // file + filename
  //
  // // Setup our pipe for reading and execute our command.
  // commandFile = popen(command, "r");
  // char data[512];
  // // Error handling
  //
  // // Get the data from the process execution
  // fgets(data, 512, commandFile);
  struct stat fileStat;
  stat(name, &fileStat);
   printf("Information for %s\n", name);
  write(STDOUT_FILENO, "Filesize: ", 11);
  printf("%d bytes\n", (int)fileStat.st_size);

  write(STDOUT_FILENO, (S_ISDIR(fileStat.st_mode)) ? "d" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IRUSR) ? "r" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IWUSR) ? "w" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IXUSR) ? "x" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IRGRP) ? "r" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IWGRP) ? "w" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IXGRP) ? "x" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IROTH) ? "r" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IWOTH) ? "w" : "-", 2);
  write(STDOUT_FILENO, (fileStat.st_mode & S_IXOTH) ? "x" : "-", 2);
  char str[32];
  time_t rawtime;
  struct tm *crtTime;
  struct tm *modTime;

  crtTime = localtime(&fileStat.st_ctime);
  modTime = localtime(&fileStat.st_mtime);

  printf("\nModification Date: %d-%d-%dT%d:%d:%d", 1900 + modTime->tm_year, modTime->tm_mon, modTime->tm_mday, modTime->tm_hour, modTime->tm_min, modTime->tm_sec);
  printf("\nCreation Date: %d-%d-%dT%d:%d:%d", 1900 + crtTime->tm_year, crtTime->tm_mon, crtTime->tm_mday, crtTime->tm_hour, crtTime->tm_min, crtTime->tm_sec);

  printf("\n---------------------------\n");
}


void searchDir(char* name){
  Options options;
  struct stat sb;
char slash[2] = "/";
								DIR *current_dir;
								struct dirent *directory_info;
								char path[256];
								strcpy(path, name);
								current_dir = opendir(path);
								while ((directory_info = readdir(current_dir)) != NULL) {

									if (strcmp(directory_info->d_name, ".") != 0 && strcmp(directory_info->d_name, "..") != 0 && (directory_info->d_type == DT_DIR))
									{  //Se fores uma pasta entao da fork() e continua no loop senao imprime o ficheiro (que depois vamos filtrar)
										int pid;
										if ((pid = fork()) == 0) {
											char * new_str;
											if ((new_str = malloc(strlen(path) + 1 + 1)) != NULL) {
												new_str[0] = '\0'; // ensures the memory is an empty string
												strcat(new_str, path);
												strcat(new_str, slash);
                      
                      printFileInfo(new_str);

											}

											char * new_str2;
											if ((new_str2 = malloc(strlen(new_str) + strlen(directory_info->d_name) + 1)) != NULL) {
												new_str2[0] = '\0';
												strcat(new_str2, new_str);
												strcat(new_str2, directory_info->d_name);

											}

											strcpy(path, new_str2);
											current_dir = opendir(path);


										}
									}
									else if (strcmp(directory_info->d_name, ".") != 0 && strcmp(directory_info->d_name, "..") != 0 && directory_info->d_type != DT_DIR) {
										///////////////////search by name//////////////////////////////
                    //sleep(100);
                    char * new_str;
                    if ((new_str = malloc(strlen(path) + 40)) != NULL) {
                      new_str[0] = '\0'; // ensures the memory is an empty string
                      strcat(new_str, path);
                      strcat(new_str, slash);
                      strcat(new_str, directory_info->d_name);
                      printFileInfo(new_str);


                    }







									}
								}
								(void) closedir (current_dir);
}



int main(int argc, char **argv, char *envp[]){
    int fileIndex = argc - 1;
    FILE *commandFile, *commandHash;
    struct stat fileStat;

    //boooleans
    int hashActivated = 0;
    int writeToFile = 0;

    int md5 = 0;
    int sha1 = 0;
    int sha256 = 0;
    //
    char *hashOptions;
    const char s[2] = ",";

    if (stat(argv[fileIndex], &fileStat) < 0)
        return 1;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            hashActivated = 1; //set to true
            hashOptions = strtok(argv[i + 1], s);
            while (hashOptions != NULL)
            {
                if (strcmp(hashOptions, "md5") == 0)
                    md5 = 1;
                if (strcmp(hashOptions, "sha1") == 0)
                    sha1 = 1;
                if (strcmp(hashOptions, "sha256") == 0)
                    sha256 = 1;
                hashOptions = strtok(NULL, ",");
            }

            break;
        }
        else if(strcmp(argv[i], "-r") == 0){
          searchDir(argv[fileIndex]);
        }
    }

    char command[512] = "file ";
    //file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256

    strcat(command, argv[fileIndex]); // file + filename

    // Setup our pipe for reading and execute our command.
    commandFile = popen(command, "r");
    char data[512];
    // Error handling

    // Get the data from the process execution
    fgets(data, 512, commandFile);

    if (stat(argv[fileIndex], &fileStat) < 0)
        return 1;
    //
    // printf("Information for %s\n", argv[fileIndex]);
    //
    // printf("---------------------------\n");
    //puts(data);
    //fputs(data, stdout);

    //do hashing
    char command2[512];
    char data2[512];

    if (hashActivated == 1 && (md5 || sha1 || sha256))
    {

        if (md5)
        {

            write(STDOUT_FILENO, ",", 2);
            char code[8] = "md5sum ";
            strcat(command2,code);
            strcat(command2, argv[fileIndex]);
            commandHash = popen(command2, "r");
            fgets(data2, 512, commandHash);
            fputs(data2, stdout);
        }
    }

    // write(STDOUT_FILENO, "Filesize: ", 11);
    // printf("%d bytes\n", (int)fileStat.st_size);
    //
    // write(STDOUT_FILENO, (S_ISDIR(fileStat.st_mode)) ? "d" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IRUSR) ? "r" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IWUSR) ? "w" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IXUSR) ? "x" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IRGRP) ? "r" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IWGRP) ? "w" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IXGRP) ? "x" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IROTH) ? "r" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IWOTH) ? "w" : "-", 2);
    // write(STDOUT_FILENO, (fileStat.st_mode & S_IXOTH) ? "x" : "-", 2);
    // char str[32];
    // time_t rawtime;
    // struct tm *crtTime;
    // struct tm *modTime;

    // crtTime = localtime(&fileStat.st_ctime);
    // modTime = localtime(&fileStat.st_mtime);
    //
    // printf("\nModification Date: %d-%d-%dT%d:%d:%d", 1900 + modTime->tm_year, modTime->tm_mon, modTime->tm_mday, modTime->tm_hour, modTime->tm_min, modTime->tm_sec);
    // printf("\nCreation Date: %d-%d-%dT%d:%d:%d", 1900 + crtTime->tm_year, crtTime->tm_mon, crtTime->tm_mday, crtTime->tm_hour, crtTime->tm_min, crtTime->tm_sec);
    //
    // printf("\n---------------------------\n");

    return 0;
}
