/* 
 * Operating Systems (2INCO) Practical Assignment
 * Interprocess Communication
 *
 * Service 1 implementation
 *
 */
#include "service1.h"

// todo
// #include <stdio.h>
// end


int service(int data) {
	if (data <= 1)
		return data;

	int n1 = 0, n2 = 1, n3;
	for (int i = 1; i < data; ++i) {
		n3 = n1 + n2;
		n1 = n2;
		n2 = n3;
	}

	return n3;
}

// todo
// int main(int argc, char const *argv[])
// {
// 	int data1 = 26;
// 	int id1 = 1;
// 	int data2 = 13;
// 	int id2 = 5;
// 	int data3 = 3;
// 	int id3 = 4;

// 	int result1 = service(data1);
// 	int result2 = service(data2);
// 	int result3 = service(data3);

// 	printf("id: %d, type: 1, input: %d. result: %d\n", id1, data1, result1);
// 	printf("id: %d, type: 1, input: %d. result: %d\n", id2, data2, result2);
// 	printf("id: %d, type: 1, input: %d. result: %d\n", id3, data3, result3);
// 	return 0;
// }
// end
