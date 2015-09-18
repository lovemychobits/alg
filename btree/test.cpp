#include "btree.h"

int main() {
	btree btr(3);
	
	btr.insert(10);
	btr.insert(12);
	btr.insert(50);
	btr.insert(11);
	btr.print();

	btr.insert(20);
	btr.insert(22);
	btr.print();

	btr.insert(33);
	btr.insert(35);
	btr.print();

	btr.insert(40);
	btr.print();

	btr.insert(42);
	btr.print();

	btr.insert(13);
	btr.insert(1);
	btr.insert(23);
	btr.print();

	//============================================================
	btr.del(23);
	btr.del(22);
	btr.print();

	btr.del(40);
	btr.print();


	btr.del(35);
	btr.print();

	btr.del(42);
	btr.print();

	return 0;
}