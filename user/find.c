#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char const *path, char const *target){
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		fprintf(2, "Cannot open %s\n", path);
		exit(1);
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "Cannot stat %s\n", path);
		close(fd);
		exit(1);
	}

	switch(st.type){
		case T_FILE:
			fprintf(2, "Cannot find the directory\n");
			exit(1);
		case T_DIR:
			if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
				printf("Path too long\n");
				break;
			}
			strcpy(buf, path);
			p = buf + strlen(buf);
			*p++ = '/';
			while(read(fd, &de, sizeof(de)) == sizeof(de)){
				if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
					continue;
				memmove(p, de.name, DIRSIZ);
				p[DIRSIZ] = 0;
				if(stat(buf, &st) < 0){
					printf("Cannot stat %s\n", buf);
					continue;
				}
				if(st.type == T_FILE){
					if(strcmp(de.name, target) == 0)
						printf("%s\n", buf);
				} else if(st.type == T_DIR)
					find(buf, target);
			}
			break;
	}
	close(fd);
}

int main(int argc, char const *argv[]){
	if(argc != 3){
		fprintf(2, "Expected 3 arguments, but only received %d\n", argc);
	}

	char const *path = argv[1];
	char const *target = argv[2];
	
	find(path, target);
	exit(0);
}
