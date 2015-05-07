#include "daemon_init.h"


int 
get_maxfd(void){
    struct rlimit tmp_rlimit;
    getrlimit(RLIMIT_NOFILE,&tmp_rlimit);

    return tmp_rlimit.rlim_max;
}


int
daemon_init(const char *pname, int facility)
{
	int		i, maxfd, pid_fd;
	pid_t	pid;

	if ( (pid = fork()) < 0)
		return (-1);
	else if (pid)
		exit(0);			/* parent terminates */

	/* child 1 continues... */

	if (setsid() < 0)			/* become session leader */
		return (-1);

	if ( (pid = fork()) < 0)
		return (-1);
	else if (pid)
		exit(0);			/* child 1 terminates */

	/* child 2 continues... */

//	daemon_proc = 1;			/* for err_XXX() functions */
//
    if (access(PID_FILE,0) == 0 ){
        printf("Pid file exists, unclean shutdown before?\n");
        exit(1)
    }
    if( (pid_fd = open(PID_FILE,O_RDWR|O_CREAT)) < 0){
        printf()
    }
    

	chdir("/");				/* change working directory */

	/* close off file descriptors */
    maxfd = get_maxfd(); 
	for (i = 0; i < maxfd; i++)
		close(i);



	/* redirect stdin, stdout, and stderr to /dev/null */

	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	openlog(pname, LOG_PID, facility);

	return (0);				/* success */
}
