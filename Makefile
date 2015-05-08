udpproxy:main.c proxy.c  common_lib.c  daemon_init.c  queue.c conf_parse.c acl_control.c  load_balance.c
	gcc -g -o udpproxy $^ -lpthread
clean:
	rm -f udpproxy
