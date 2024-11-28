/* 
 * Operating Systems  (2INCO)  Practical Assignment
 * Interprocess Communication
 *
 * Jie Liu (1799525)
 * Augustin Lassus (1797441)
 * Luis Fernadez Gu (1804189)
 *
 * Grading:
 * Your work will be evaluated based on the following criteria:
 * - Satisfaction of all the specifications
 * - Correctness of the program
 * - Coding style
 * - Report quality
 * - Deadlock analysis
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>      // for perror()
#include <unistd.h>     // for getpid()
#include <mqueue.h>     // for mq-stuff
#include <time.h>       // for time()

#include "messages.h"
#include "request.h"

static void rsleep (int t);


int main (int argc, char * argv[])
{
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the message queue (whose name is provided in the
    //    arguments)
    //  * repeatingly:
    //      - get the next job request 
    //      - send the request to the Req message queue
    //    until there are no more requests to send
    //  * close the message queue
    
    mqd_t mq_request;
    MQ_CLIENT2DEALER_MESSAGE req_msg;
    req_msg.id = -2;
    req_msg.service_type = -2;
    req_msg.data = -2;
    int request_left = -2;

    mq_request = mq_open (argv[1], O_WRONLY);

    // todo b1
    printf("client starts executing\n");
    // b1

    do
    {
        request_left = getNextRequest(&req_msg.id, &req_msg.data, &req_msg.service_type);
        if(request_left <= NO_REQ){
            // todo b2
            printf("No message left from client\n");
            // b2
            req_msg.id = TERMINATION_CODE;
            mq_send(mq_request, (char *) &req_msg, sizeof(req_msg), 0);
            break;            
        }
        // todo b3
        printf("Client sentd - id: %d, type: %d, data: %d\n",
            req_msg.id, req_msg.service_type, req_msg.data);
            // b3
        mq_send(mq_request, (char *) &req_msg, sizeof(req_msg), 0);
    } while (true);
    

    mq_close(mq_request);
    return (0);
}
