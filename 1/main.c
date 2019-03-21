#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

int main(int argc, int **argv, int *envp[])
{
    int fileIndex = argc - 1;
    FILE *pf;
    struct stat fileStat;
    if (stat((char *)argv[fileIndex], &fileStat) < 0)
        return 1;

    char command[512] = "file ";
    //file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256

    strcat(command, (char *)argv[fileIndex]);

    // Setup our pipe for reading and execute our command.
    pf = popen(command, "r");
    char data[512];
    // Error handling

    // Get the data from the process execution
    fgets(data, 512, pf);
    
    if (stat((char *)argv[fileIndex], &fileStat) < 0)
        return 1;

    printf("Information for %s\n", (char *)argv[fileIndex]);
    
    printf("---------------------------\n");
    //puts(data); 
    fputs(data,stdout);
    write(STDOUT_FILENO,"Filesize: ",11);
    printf("%d bytes\n", (int)fileStat.st_size);

    write(STDOUT_FILENO,(S_ISDIR(fileStat.st_mode)) ? "d" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IRUSR) ? "r" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IWUSR) ? "w" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IXUSR) ? "x" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IRGRP) ? "r" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IWGRP) ? "w" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IXGRP) ? "x" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IROTH) ? "r" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IWOTH) ? "w" : "-",2);
    write(STDOUT_FILENO,(fileStat.st_mode & S_IXOTH) ? "x" : "-",2);
    char str[32];
    time_t rawtime;
    struct tm *crtTime;
    struct tm *modTime;

    crtTime = localtime(&fileStat.st_ctime);
    modTime = localtime(&fileStat.st_mtime);

    printf("\nModification Date: %d-%d-%dT%d:%d:%d", 1900 + modTime->tm_year,modTime->tm_mon,modTime->tm_mday,modTime->tm_hour,modTime->tm_min,modTime->tm_sec);
    printf("\nCreation Date: %d-%d-%dT%d:%d:%d", 1900 + crtTime->tm_year,crtTime->tm_mon,crtTime->tm_mday,crtTime->tm_hour,crtTime->tm_min,crtTime->tm_sec);

    printf("\n---------------------------\n");

    return 0;
}