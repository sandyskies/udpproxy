#include "daemon_init.h"
#include "signal_handler.h"


int 
get_maxfd(void){
    struct rlimit tmp_rlimit;
    getrlimit(RLIMIT_NOFILE,&tmp_rlimit);

    return tmp_rlimit.rlim_max;
}

int
daemon_init(const char *pname, int facility)
{
	int		i, maxfd;
	pid_t	pid;

	if ( (pid = fork()) < 0)
		return (-1);
	else if (pid)
		exit(0);			/* parent terminates */

	/* child 1 continues... */

	if (setsid() < 0)			/* become session leader */
		return (-1);

	signal(SIGHUP, sighup_handler);
	if ( (pid = fork()) < 0)
		return (-1);
	else if (pid)
		exit(0);			/* child 1 terminates */

	/* child 2 continues... */

//	daemon_proc = 1;			/* for err_XXX() functions */

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
