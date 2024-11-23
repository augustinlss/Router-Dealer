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

#ifndef MESSAGES_H
#define MESSAGES_H

// define the data structures for your messages here

#define TERMINATION_CODE -1 

typedef struct
{
    int                     id;
    int                     service_type;
    int                     data;
} MQ_CLIENT2DEALER_MESSAGE;

typedef struct
{
    int                     id;
    int                     data;
} MQ_DEALER2WORKER_MESSAGE;

#endif
