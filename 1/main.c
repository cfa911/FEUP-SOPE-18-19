#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, int *argv[], int *envp[])
{
    int fileIndex = argc - 1;
    FILE *pf;

    struct stat fileStat;
    if (stat(argv[1], &fileStat) < 0)
        return 1;

    char command[20] = "file ";
    //file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256

    strcat(command, argv[fileIndex]);

    // Setup our pipe for reading and execute our command.
    pf = popen(command, "r");
    char data[512];
    // Error handling

    // Get the data from the process execution
    fgets(data, 512, pf);

    printf(data);

    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
    /*
    switch (bits)
    {

    case S_IRWXU: // owner has read, write, and execute permission
    case S_IRUSR: // owner has read permission
    case S_IWUSR: // owner has write permission
    case S_IXUSR: // owner has execute permission
    case S_IRWXG: // group has read, write, and execute permission
    case S_IRGRP: // group has read permission
    case S_IWGRP: // group has write permission
    case S_IXGRP: // group has execute permission
    case S_IRWXO: // others (not in group) have read, write, and execute permission
    case S_IROTH: // others have read permission
    case S_IWOTH: // others have write permission
    case S_IXOTH: // others have execute permission
        break;
    }*/

    return 0;
}