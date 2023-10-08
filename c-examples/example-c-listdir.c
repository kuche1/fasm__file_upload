
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>


#define BUF_SIZE 1024

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


struct linux_dirent {
	unsigned long   d_ino;
	unsigned long   d_off;
	unsigned short  d_reclen; 
	char        d_name[];
};


int main(void){
	//int dir = openat(AT_FDCWD, ".", O_RDONLY|O_NONBLOCK|O_DIRECTORY|O_CLOEXEC);
	int fd = open(".", O_RDONLY | O_DIRECTORY);

	char buf[BUF_SIZE];
	int bpos;
	char d_type;
	struct linux_dirent *d;

	for( ; ; ) {
		int nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);

		if(nread == 0)
			break;

		printf("--------------- nread=%d ---------------\n", nread);

		for(bpos=0; bpos<nread; ) {
			d = (struct linux_dirent *)(buf+bpos);
			d_type = *(buf + bpos + d->d_reclen - 1);
			if (d_type == DT_REG) {
	            printf("%s\n", d->d_name);
			}
            bpos += d->d_reclen;
		}
	}

	close(fd);

	return 0;
}
