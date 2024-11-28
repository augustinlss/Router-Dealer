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
#include "service1.h"

static void rsleep (int t);


int main (int argc, char * argv[])
{
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names are provided in the
    //    arguments)
    //  * repeatedly:
    //      - read from the S1 message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do the job 
    //      - write the results to the Rsp message queue
    //    until there are no more tasks to do
    //  * close the message queues

    mqd_t mq_d2w1;
    mqd_t mq_resp;
    MQ_DEALER2WORKER_MESSAGE received;
    received.id = 114514;
    received.data = 114514;
    MQ_DEALER2WORKER_MESSAGE response;

    mq_d2w1 = mq_open(argv[1], O_RDONLY);
    mq_resp = mq_open(argv[2], O_WRONLY);

    // todo b0
    printf("worker 1 starts executing\n");
    // b0

    do
    {
        mq_receive(mq_d2w1, (char *) &received, sizeof(received), NULL);
        // if (mq_receive(mq_d2w1, (char *) &received, sizeof(received), NULL) != -1) {
        //     rsleep(WAITING_TIME);
        //     // sleep(100);
        //     if (received.id == TERMINATION_CODE) {
        //     printf("From woker1, termination received\n");
        //         break;
        //     }
        //     response.id = received.id;
        //     response.data = service(received.data);
        //     printf("From woker1, rec id: %d, rec data: %d, resp data:%d\n",
        //         received.id, received.data, response.data);
        //     mq_send(mq_resp, (char *) &response, sizeof(response), 0);
        // } 
        rsleep(WAITING_TIME);
        if (received.id == TERMINATION_CODE) {
            // todo b1
            printf("From woker1, termination received\n");
            // b1
            response.id = TERMINATION_CODE;
            mq_send(mq_resp, (char *) &response, sizeof(response), 0);
            break;
        }
        response.id = received.id;
        response.data = service(received.data);
        // todo b1
        printf("From woker1, rec id: %d, rec data: %d, resp data:%d\n",
            received.id, received.data, response.data);
            // todo b2
        mq_send(mq_resp, (char *) &response, sizeof(response), 0);
        //TODO test block1
        // else {
        //     printf("worker 1 did not receive anything, loop go on\n");
        // }
        //test block1
        
    } while (true);
    
    mq_close(mq_d2w1);
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
