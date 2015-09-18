#include "mutex.h"

int main() {
	mutex m;
	m.value = 0;
	enter_mutex(&m);
	leave_mutex(&m);
	enter_mutex(&m);
	leave_mutex(&m);
	return 0;
}