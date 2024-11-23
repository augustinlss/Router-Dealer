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
#include "service2.h"

static void rsleep (int t);

char* name = "NO_NAME_DEFINED";
mqd_t dealer2worker;
mqd_t worker2dealer;


int main (int argc, char * argv[])
{
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names are provided in the
    //    arguments)
    //  * repeatedly:
    //      - read from the S2 message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do the job 
    //      - write the results to the Rsp message queue
    //    until there are no more tasks to do
    //  * close the message queues

    mqd_t mq_d2w;
    mqd_t mq_resp;
    MQ_DEALER2WORKER_MESSAGE received;
    received.id = 0;
    MQ_DEALER2WORKER_MESSAGE response;

    mq_d2w  = mq_open(argv[1], O_RDONLY);
    mq_resp = mq_open(argv[2], O_RDONLY);

    do
    {
        mq_receive(mq_d2w, (char *) &received, sizeof(received), NULL);
        rsleep(10000);
        if (received.id == TERMINATION_CODE) {
            break;
        }
        response.id = received.id;
        response.data = service(received.data);
        mq_send(mq_resp, (char *) &response, sizeof(response), 0);
    } while (true);
    
    mq_close(mq_d2w);
    mq_close(mq_resp);
    return(0);
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}
