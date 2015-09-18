#ifndef MUTEX_MUTEX_H
#define MUTEX_MUTEX_H

struct mutex {
	int value;
};


void enter_mutex(mutex* m);
void leave_mutex(mutex* m);

#endif