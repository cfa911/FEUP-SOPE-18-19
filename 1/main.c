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
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>


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

void removeSpaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}

void printFileInfo(char * name){

  FILE *commandFile;
  char command[512] = "file ";
  strcat(command, name); // file + filename

  // Setup our pipe for reading and execute our command.
  commandFile = popen(command, "r");
   char data[512];
  // // Error handling
  //
  // // Get the data from the process execution
  fgets(data, 512, commandFile);

  struct stat fileStat;
  stat(name, &fileStat);



  for(int i=0; i< 512;i++){
  if(data[i]=='\n')
  data[i]='\0';
  else if(data[i]==':')
  data[i]=',';
  }

  removeSpaces(data);

  printf("%s", data);
  printf(",%d,", (int)fileStat.st_size);

  char permissions[11]="";
  sprintf(permissions,"%s%s%s%s%s%s%s%s%s%s",(S_ISDIR(fileStat.st_mode)) ? "d" : "-",(fileStat.st_mode & S_IRUSR) ? "r" : "-",(fileStat.st_mode & S_IWUSR) ? "w" : "-",
(fileStat.st_mode & S_IXUSR) ? "x" : "-",(fileStat.st_mode & S_IRGRP) ? "r" : "-",(fileStat.st_mode & S_IWGRP) ? "w" : "-",
(fileStat.st_mode & S_IXGRP) ? "x" : "-",(fileStat.st_mode & S_IROTH) ? "r" : "-",(fileStat.st_mode & S_IWOTH) ? "w" : "-",(fileStat.st_mode & S_IXOTH) ? "x" : "-");
  printf("%s,", permissions );

  struct tm *crtTime;
  struct tm *modTime;

  crtTime = localtime(&fileStat.st_ctime);
  modTime = localtime(&fileStat.st_mtime);

  printf("%d-%d-%dT%d:%d:%d,", 1900 + modTime->tm_year, modTime->tm_mon, modTime->tm_mday, modTime->tm_hour, modTime->tm_min, modTime->tm_sec);
  printf("%d-%d-%dT%d:%d:%d,", 1900 + crtTime->tm_year, crtTime->tm_mon, crtTime->tm_mday, crtTime->tm_hour, crtTime->tm_min, crtTime->tm_sec);

  printf("\n\n");
}


void searchDir(char* name){

  char slash[2] = "/";
	DIR *current_dir;
	struct dirent *directory_info;
	char path[256];
	strcpy(path, name);
	current_dir = opendir(path);

  while ((directory_info = readdir(current_dir)) != NULL) {
    if (strcmp(directory_info->d_name, ".") != 0 && strcmp(directory_info->d_name, "..") != 0 && (directory_info->d_type == DT_DIR)){
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
     int status;
     //waitpid(pid,&status,WNOHANG);
		}
       else if (strcmp(directory_info->d_name, ".") != 0 && strcmp(directory_info->d_name, "..") != 0 && directory_info->d_type != DT_DIR) {
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
	(void) closedir(current_dir);
}


int main(int argc, char **argv, char *envp[]){
    int fd, fileIndex = argc - 1;
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
        }
        else if(strcmp(argv[i], "-r") == 0){
          searchDir(argv[fileIndex]);
        }
        else if(strcmp(argv[i], "-o") == 0){
          writeToFile = 1;
          fd = open(argv[++i], O_RDWR | O_TRUNC| O_CREAT, S_IRUSR| S_IRGRP | S_IROTH);
          dup2(fd, STDOUT_FILENO);
        }
    }

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            writeToFile = 1; //set to true
            fd = open(argv[++i], O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
            dup2(fd, STDOUT_FILENO);
            break;
        }
    }
    char command[512] = "file ";

    strcat(command, argv[fileIndex]); // file + filename

    // Setup our pipe for reading and execute our command.
    commandFile = popen(command, "r");
    char data[80];
    // Error handling

    // Get the data from the process execution
    pclose(commandFile);
    fgets(data, 80, commandFile);

    if (stat(argv[fileIndex], &fileStat) < 0)
        return 1;

    printf("Information for %s\n", argv[fileIndex]);

    printf("---------------------------\n");
    //puts(data);
    printf("%s", data);

    //do hashing
    char command2[80] = "";
    char data2[80];
    if (hashActivated == 1 && (md5 || sha1 || sha256))
    {

        if (md5)
        {

            write(STDOUT_FILENO, ",", 2);
            char code[8] = "md5sum ";

            strcat(command2, code);
            strcat(command2, argv[fileIndex]);
            commandHash = popen(command2, "r");
            fgets(data2, 80, commandHash);
            printf("%s", data2);
        }
        if (sha1)
        {
            write(STDOUT_FILENO, ",", 2);
            char code[9] = "sha1sum ";
            memset(command2, 0, sizeof command2);
            memset(data2, 0, sizeof data2);
            strcat(command2, code);
            strcat(command2, argv[fileIndex]);
            commandHash = popen(command2, "r");
            fgets(data2, 80, commandHash);
            printf("%s", data2);
        }
        if (sha256)
        {

            write(STDOUT_FILENO, ",", 2);
            char code[11] = "sha256sum ";
            memset(command2, 0, sizeof command2);
            memset(data2, 0, sizeof data2);
            strcat(command2, code);
            strcat(command2, argv[fileIndex]);
            commandHash = popen(command2, "r");
            fgets(data2, 80, commandHash);
            printf("%s", data2);
        }
    }
    pclose(commandHash);
    write(STDOUT_FILENO, "Filesize: ", 11);
    printf("%d bytes\n", (int)fileStat.st_size);

        if (sha256){
            memset(command2, 0, sizeof command2);
            memset(data2, 0, sizeof data2);
            write(STDOUT_FILENO, ",", 2);
            char code[11] = "sha256sum ";
            strcat(command2, code);
        }

        strcat(command2, argv[fileIndex]);
        commandHash = popen(command2, "r");
        fgets(data2, 512, commandHash);
        printf(data2);
    }
    return 0;
}
