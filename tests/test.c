#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int fd = open("ls", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	printf("%d", fd);
	close(fd);
	return 0;
}
