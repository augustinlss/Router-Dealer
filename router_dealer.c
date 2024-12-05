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
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>    
#include <unistd.h>    // for execlp
#include <mqueue.h>    // for mq


#include "settings.h"  
#include "messages.h"

char client2dealer_name[30];
char dealer2worker1_name[30];
char dealer2worker2_name[30];
char worker2dealer_name[30];

#define STUDENT_NAME        "JAL"

int main (int argc, char * argv[])
{
  if (argc != 1)
  {
    fprintf (stderr, "%s: invalid arguments\n", argv[0]);
  }
  
  // TODO:
    //  * create the message queues (see message_queue_test() in
    //    interprocess_basic.c) Done!
    //  * create the child processes (see process_test() and
    //    message_queue_test())
    //  * read requests from the Req queue and transfer them to the workers
    //    with the Sx queues
    //  * read answers from workers in the Rep queue and print them
    //  * wait until the client has been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues
    // contain your goup number (to ensure uniqueness during testing)
  
  struct mq_attr attr;
  int router_pid = getpid();
  mqd_t mq_client2dealer;
  mqd_t mq_dealer2worker1;
  mqd_t mq_dealer2worker2;
  mqd_t mq_worker2dealer;
  pid_t client_1;

  sprintf(client2dealer_name, "/Req_queue_%s_%d", STUDENT_NAME, router_pid);
  sprintf(dealer2worker1_name, "/S1_queue_%s_%d", STUDENT_NAME, router_pid);
  sprintf(dealer2worker2_name, "/S2_queue_%s_%d", STUDENT_NAME, router_pid);
  sprintf(worker2dealer_name, "/Rsp_queue_%s_%d", STUDENT_NAME, router_pid);

  attr.mq_maxmsg = MQ_MAX_MESSAGES;
  attr.mq_msgsize = sizeof (MQ_CLIENT2DEALER_MESSAGE);
  mq_client2dealer = mq_open (client2dealer_name, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

  attr.mq_maxmsg = MQ_MAX_MESSAGES;
  attr.mq_msgsize = sizeof (MQ_DEALER2WORKER_MESSAGE);
  mq_dealer2worker1 = mq_open (dealer2worker1_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

  attr.mq_maxmsg = MQ_MAX_MESSAGES;
  attr.mq_msgsize = sizeof (MQ_DEALER2WORKER_MESSAGE);
  mq_dealer2worker2 = mq_open (dealer2worker2_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

  attr.mq_maxmsg = MQ_MAX_MESSAGES;
  attr.mq_msgsize = sizeof (MQ_DEALER2WORKER_MESSAGE);
  mq_worker2dealer = mq_open (worker2dealer_name, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

  client_1 = fork();    //create the client process

  if(client_1 == 0) {
    // client process starts executing
    execlp("./client", "client", client2dealer_name, NULL);

  } else {
    // actions performed by dealer(parent process)
    for (int i = 0; i < N_SERV1; i++)
    {
      pid_t temp = fork();    //create N_serv1 worker1
      if (temp == 0) {
        execlp(
          "./worker_s1", "worker_s1",
           dealer2worker1_name, worker2dealer_name, NULL);
      }       
    }

    for (int i = 0; i < N_SERV2; i++)
    {
      pid_t temp = fork();    //create N_serv2 worker2
      if (temp == 0) {
        execlp(
          "./worker_s2", "worker_s2",
          dealer2worker2_name, worker2dealer_name, NULL);
      }  
    }
    
    MQ_CLIENT2DEALER_MESSAGE msg_c2d;
    MQ_DEALER2WORKER_MESSAGE msg_d2w;
    MQ_DEALER2WORKER_MESSAGE msg_w2d;
    
    int isBuffered = 0;
    // 0 -> not buffered
    // 1 -> buffered message

    //message count
    int msg_send_cnt = 0;
    int msg_rec_cnt = 0;
    //message count

    while (true) {
      struct mq_attr attr_c2d;
      struct mq_attr attr_w2d;      

      // fixed the deadlock. Using the buffer scheme.
      mq_getattr(mq_client2dealer, &attr_c2d);
      if (attr_c2d.mq_curmsgs > 0 || isBuffered > 0) {
        // only c2d channel has something left or it buffered message before, it executes
        // fetches and sends operation.
        if (isBuffered == 0) {  
          // if no message is buffered, then go on fetching message from c2d channel. 
          mq_receive(mq_client2dealer, (char *) &msg_c2d, sizeof(msg_c2d), NULL);
          if (msg_c2d.id == TERMINATION_CODE) {
            // termination code received from the client, which means no more message in
            // the c2d channel
            break;
          }          
          msg_d2w.id = msg_c2d.id;
          msg_d2w.data = msg_c2d.data;
        }
              
        if (msg_c2d.service_type == 1 && N_SERV1 > 0) {
          // the message shall be processed by worker1
          struct mq_attr attr_d2w;
          mq_getattr(mq_dealer2worker1, &attr_d2w);
          
          if (attr_d2w.mq_curmsgs < MQ_MAX_MESSAGES) {    
            //d2w1 channel has space left to send out message 
            mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);
            msg_send_cnt++;
            isBuffered = 0;            
          } else {
            // d2w1 channel is full, buffer the message
            isBuffered = 1;
          }                
        }else if (msg_c2d.service_type == 2 && N_SERV2 > 0) {
          // the message shall be processed by worker2
          struct mq_attr attr_d2w;
          mq_getattr(mq_dealer2worker2, &attr_d2w);          
          
          if (attr_d2w.mq_curmsgs < MQ_MAX_MESSAGES) {
            //d2w2 channel has space left to send out message 
            mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);
            msg_send_cnt++;
            isBuffered = 0;            
          }else{
            // d2w1 channel is full, buffer the message
            isBuffered = 1;
          }                    
        }
      }
      mq_getattr(mq_worker2dealer, &attr_w2d);
      if (attr_w2d.mq_curmsgs > 0) {   
        //fetching message from the w2d channel if any left here
        mq_receive(mq_worker2dealer, (char*) &msg_w2d, sizeof(msg_w2d), NULL);
        msg_rec_cnt++;
        printf("%d -> %d\n",msg_w2d.id, msg_w2d.data);
      }
    }

    int client_status = 0;
    waitpid(client_1, &client_status, 0);   //release the resouce of the client
    
    while (msg_send_cnt != msg_rec_cnt)
    {
      // go on fetching the response from workers unitl all message has been processed(msg_send_cnt=msg_rec_cnt).
      mq_receive(mq_worker2dealer, (char*) &msg_w2d, sizeof(msg_w2d), NULL);
      msg_rec_cnt++;
      printf("%d -> %d\n",msg_w2d.id, msg_w2d.data);
    }

    // sends out termination sign to all workers
    for (int i = 0; i < N_SERV1; i++)
    {
      msg_d2w.id = TERMINATION_CODE;
      mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);      
    }

    for (int i = 0; i < N_SERV2; i++)
    {
      msg_d2w.id = TERMINATION_CODE;
      mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);      
    }

    // wait for all workers to terminate and release their resources.
    for (int i = 0; i < (N_SERV1 + N_SERV2); i++)
    {
        wait(NULL);
    }
  }

  mq_close(mq_client2dealer);
  mq_close(mq_dealer2worker1);
  mq_close(mq_dealer2worker2);
  mq_close(mq_worker2dealer);
  mq_unlink(client2dealer_name);
  mq_unlink(dealer2worker1_name);
  mq_unlink(dealer2worker2_name);
  mq_unlink(worker2dealer_name);
  return (0);
}
