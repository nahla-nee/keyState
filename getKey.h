#ifndef __keyStateMOD__
#define __keyStateMOD__

#ifdef __cplusplus
extern "C"{
#endif

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


//are you really too lazy to write your own version of this?
int openKS(void){
	int fd = open("/dev/keyState", O_RDONLY);
	return fd;
}
/*this literally takes longer to type out than just calling close()
what am I doing with my life*/
void closeKS(int fd){
	close(fd);
}

/*impelmentation that lets the user pass their own file descriptor.
saves cost of opening and closing a file everytime*/
char getKeyStateFd(int fd, int key){
	char keyState;
	lseek(fd, key, SEEK_SET);
	if(read(fd, (void*)&keyState, 1) == -1){
		return -1;
	}
	return keyState;
}

char getKeyState(int key){
	int fd = open("/dev/keyState", O_RDONLY);
	if(fd = -1){
		return -1;
	}
	char keyState;
	lseek(fd, key, SEEK_SET);
	if(read(fd, (void*)&keyState, 1) == -1){
		return -1;
	}
	close(fd);
	return keyState;
}

#ifdef __cplusplus
}
#endif

#endif /*keyStateMOD*/
