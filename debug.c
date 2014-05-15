#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char buffer;
    int fd = open("debug.log", O_RDONLY | O_CREAT, 0644);
    while(1) {   	
      	int x = read(fd, &buffer, sizeof(char));
    	if (x) printf("%c", buffer);
    }
    close(fd);
    return 0;
}


