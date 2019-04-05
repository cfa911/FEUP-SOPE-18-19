#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

/*
    char text[1000];
    FILE *fp=fopen("filename", "r");
    int i=0;
    while(feof(fp))
    text[i++] = fgetc(fp);
    text[i]='\0';
*/

int main(int argc, char **argv, char *envp[])
{
    int fileIndex = argc - 1;
    int fd;
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
    //file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256

    strcat(command, argv[fileIndex]); // file + filename

    // Setup our pipe for reading and execute our command.
    commandFile = popen(command, "r");
    char data[80];
    // Error handling

    // Get the data from the process execution
    fgets(data, 80, commandFile);
    pclose(commandFile);

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

    return 0;
}