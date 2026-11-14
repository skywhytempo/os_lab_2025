CC=gcc
CFLAGS=-pthread -Wall

all: mutex_no_lock mutex_with_lock 

mutex: mutex.c
	$(CC) $(CFLAGS) -o mutex mutex.c

mutex_lock: mutex_lock.c
	$(CC) $(CFLAGS) -o mutex_lock mutex_lock.c

factorial: factorial.c
	$(CC) $(CFLAGS) -o factorial factorial.c

deadlock: deadlock.c
	$(CC) $(CFLAGS) -o deadlock deadlock.c

clean:
	rm -f mutex mutex_lock deadlock factorial