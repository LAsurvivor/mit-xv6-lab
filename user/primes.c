#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void new_proc(int p[2]){
	int prime;
	int flag;
	int n;

	close(p[1]);
	
	if(read(p[0], &prime, 4) != 4){
		fprintf(2, "Child process failed to read\n");
		exit(1);
	}
	printf("prime %d\n", prime);

	flag = read(p[0], &n, 4);
	if(flag){
		int new_p[2];
		pipe(new_p);
		if(fork() == 0){
			new_proc(new_p);
		} else {
			close(new_p[0]);
			if(n % prime) write(new_p[1], &n, 4);

			while(read(p[0], &n, 4)){
				if(n % prime) write(new_p[1], &n, 4);
			}
			close(p[0]);
			close(new_p[1]);
			wait(0);
		}
	}
	exit(0);
}

int main(int argc, char const *argv[])
{
	int p[2];
	pipe(p);
	
	if(fork() == 0){	// children
		new_proc(p);
	} else {	// parent
		close(p[0]);
		
		for(int i = 2; i <= 35; i++){
			if(write(p[1], &i, 4) != 4){
				fprintf(2, "Parent process failed to write %d into the pipe\n", i);
				exit(1);
			}
		}
		
		close(p[1]);
		wait(0);
		exit(0);
	}
	return 0;
}
