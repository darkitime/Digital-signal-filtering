#include <cassert>

#include "Signal.h"

void test_signal() {
	//создаем сигнал
	Signal s1(3);
	s1.setValue(0, 1.0);
	s1.setValue(1, 2.0);
	s1.setValue(2, 3.0);
	assert(s1.getValue(0) == 1.0);
	assert(s1.getValue(1) == 2.0);
	assert(s1.getValue(2) == 3.0);
	assert(s1.getSize() == 3);

	//тест копирования
	Signal s2 = s1; // конструктор копирования
	assert(s2.getValue(0) == 1.0);
	assert(s2.getValue(1) == 2.0);
	assert(s2.getValue(2) == 3.0);
	assert(s2.getSize() == 3);

	//тест присваивания
	Signal s3(2);
	s3 = s1; // оператор присваивания
	assert(s3.getValue(0) == 1.0);
	assert(s3.getValue(1) == 2.0);
	assert(s3.getValue(2) == 3.0);
	assert(s3.getSize() == 3);

	//тест сложения ( разные размеры )
	Signal s4(5);
	s4.setValue(0, 10.0);
	s4.setValue(4, 50);
	Signal sum = s1 + s4;
	assert(sum.getSize() == 5);
	assert(sum.getValue(0) == 11.0); // 1 + 10
	assert(sum.getValue(1) == 2.0);  // 2 + 0
	assert(sum.getValue(2) == 3.0);  // 3 + 0
	assert(sum.getValue(3) == 0.0);  // 0 + 0
	assert(sum.getValue(4) == 50.0); // 0 + 50

	//тест умножения на скаляр
	Signal scaled = s1 * 2.0;
	assert(scaled.getSize() == 3);
	assert(scaled.getValue(0) == 2.0); // 1 * 2
	assert(scaled.getValue(1) == 4.0); // 2 * 2
	assert(scaled.getValue(2) == 6.0); // 3 * 2

	//тест конкатенации
	Signal con = s1.concat(s4);
	assert(con.getSize() == 8);
	assert(con.getValue(0) == 1.0);
	assert(con.getValue(2) == 3.0);
	assert(con.getValue(3) == 10.0); // начало s4
	assert(con.getValue(7) == 50.0); // конец s4

	std::cout << "All tests passed!" << std::endl;
}

void runTest() {
	test_signal();
}
