/* 
 * Operating Systems (2INCO) Practical Assignment
 * Interprocess Communication
 *
 * Service 2 implementation
 *
 */

#include "service2.h"

// todo
// #include <stdio.h>
// end

int service(int data) {
	if (data <= 1)
		return data;

	int n = data * 1023;
	for (int i = 1; i < data && n > 1; ++i) {

		if (n % 2 == 0)
			n = n / 2;
		else
			n = 3 * n + 1;
	}

	return n;
}

// todo
// int main(int argc, char const *argv[])
// {
// 	int data1 = 5;
// 	int id1 = 2;
// 	int data2 = 10;
// 	int id2 = 3;

// 	int result1 = service(data1);
// 	int result2 = service(data2);
	

// 	printf("id: %d, type: 2, input: %d. result: %d\n", id1, data1, result1);
// 	printf("id: %d, type: 2, input: %d. result: %d\n", id2, data2, result2);

// 	return 0;
// }
// end
