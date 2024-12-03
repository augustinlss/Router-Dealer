/* 
 * Operating Systems (2INCO) Practical Assignment
 * Interprocess Communication
 *
 * Contains functions that are used by the clients
 *
 */

#include "request.h"

// Array of requests
const Request requests[] = { {1, 26, 1}, {2, 5, 2}, {3, 10, 2}, {5, 13, 1}, {4, 3, 1} };
// const Request requests[] = { };
// const Request requests[] = { {1, 26, 1}, {2, 5, 1}, {3, 10, 1}, {5, 13, 1}, {4, 3, 1}, 
// 	{6, 26, 2}, {7, 5, 2}, {8, 10, 2}, {10, 13, 2}, {9, 3, 2},
// 	{1, 26, 1}, {2, 5, 1}, {3, 10, 1}, {5, 13, 1}, {4, 3, 1}, 
// 	{6, 26, 2}, {7, 5, 2}, {8, 10, 2}, {10, 13, 2}, {9, 3, 2}};
// const Request requests[] = { {1, 26, 1}, {2, 5, 1}, {3, 10, 1}, {5, 13, 1}, {4, 3, 1}, 
// 	{6, 26, 1}, {7, 5, 1}, {8, 10, 1}, {10, 13, 1}, {9, 3, 1},
// 	{1, 26, 1}, {2, 5, 1}, {3, 10, 1}, {5, 13, 1}, {4, 3, 1}, 
// 	{6, 26, 1}, {7, 5, 2}, {8, 10, 2}, {10, 13, 2}, {9, 3, 2}};

// Places the information of the next request in the parameters sent by reference.
// Returns NO_REQ if there is no request to make.
// Returns NO_ERR otherwise.
int getNextRequest(int* jobID, int* data, int* serviceID) {
	static int i = 0;
	static int N_REQUESTS = sizeof(requests) / sizeof(Request);

	if (i >= N_REQUESTS) 
		return NO_REQ;

	*jobID = requests[i].job;
	*data = requests[i].data;
	*serviceID = requests[i].service;		
	++i;
	return NO_ERR;
		
}
