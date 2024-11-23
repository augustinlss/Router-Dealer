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

#define STUDENT_NAME        "JieAugustinLuis"

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
  
  // int mq_maxmsg = 10;
  struct mq_attr attr;
  int router_pid = getpid();
  mqd_t mq_client2dealer;
  mqd_t mq_dealer2worker1;
  mqd_t mq_dealer2worker2;
  mqd_t mq_worker2dealer;
  pid_t client_1;
  pid_t workers1[N_SERV1];
  pid_t workers2[N_SERV2];

  sprintf(client2dealer_name, "/Req_queue_%s_%d", STUDENT_NAME, router_pid);
  sprintf(dealer2worker1_name, "/S1_queue_%s_%d", STUDENT_NAME, router_pid);
  sprintf(dealer2worker2_name, "/S2_queue_%s_%d", STUDENT_NAME, router_pid);
  sprintf(worker2dealer_name, "/Rsp_queue_%s_%d", STUDENT_NAME, router_pid);

  attr.mq_maxmsg = MQ_MAX_MESSAGES;
  attr.mq_msgsize = sizeof (MQ_CLIENT2DEALER_MESSAGE);
  mq_client2dealer = mq_open (client2dealer_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

  attr.mq_maxmsg = MQ_MAX_MESSAGES;
  attr.mq_msgsize = sizeof (MQ_DEALER2WORKER_MESSAGE);
  mq_dealer2worker1 = mq_open (dealer2worker1_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

  attr.mq_maxmsg = MQ_MAX_MESSAGES;
  attr.mq_msgsize = sizeof (MQ_DEALER2WORKER_MESSAGE);
  mq_dealer2worker2 = mq_open (dealer2worker2_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

  attr.mq_maxmsg = MQ_MAX_MESSAGES;
  attr.mq_msgsize = sizeof (MQ_DEALER2WORKER_MESSAGE);
  mq_worker2dealer = mq_open (worker2dealer_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

  client_1 = fork();

  if(client_1 == 0) {

    execlp("client", "client", "client2dealer_name", NULL);

  } else {

    int clien_status = 0;
    int cliend_id = 0;

    for (int i = 0; i < N_SERV1; i++)
    {
      pid_t temp = fork();
      if (temp > 0) {
        workers1[i] = temp;
      } else {
        execlp(
          "./worker_s1", "worker_s1",
           dealer2worker1_name, worker2dealer_name, NULL);
      }
      
    }

    for (int i = 0; i < N_SERV2; i++)
    {
      pid_t temp = fork();
      if (temp > 0) {
        workers2[i] = temp;
      } else {
        execlp(
          "./worker_s1", "worker_s1",
          dealer2worker2_name, worker2dealer_name, NULL
        );
      }
    }

    MQ_CLIENT2DEALER_MESSAGE msg_c2d;
    MQ_DEALER2WORKER_MESSAGE msg_d2w;
    // MQ_DEALER2WORKER_MESSAGE msg_d2w2;
    MQ_DEALER2WORKER_MESSAGE msg_w2d;
    
    while (client_1 != cliend_id) {
      mq_receive(mq_client2dealer, (char *) &msg_c2d, sizeof(msg_c2d), NULL);
      msg_d2w.id = msg_c2d.id;
      msg_d2w.data = msg_c2d.data;

      if (msg_c2d.service_type == 1) {
        mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);
      }else {
        mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);
      }

      mq_receive(mq_worker2dealer, (char*) &msg_w2d, sizeof(msg_w2d), NULL);
      printf("%d -> %d\n",msg_w2d.id, msg_w2d.data);

      cliend_id = waitpid(client_1, &clien_status, WNOHANG);
    }

    while (true) {
      int is_msg_c2d_left = mq_receive(mq_client2dealer, (char *) &msg_c2d, sizeof(msg_c2d), NULL);
      if (is_msg_c2d_left == -1 && errno == EAGAIN) {
        break;
      } 

      msg_d2w.id = msg_c2d.id;
      msg_d2w.data = msg_c2d.data;

      if (msg_c2d.service_type == 1) {
        mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);
      }else {
        mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);
      }

      mq_receive(mq_worker2dealer, (char*) &msg_w2d, sizeof(msg_w2d), NULL);
      printf("%d -> %d\n",msg_w2d.id, msg_w2d.data);
    }


    for (int i = 0; i < N_SERV1; i++)
    {
      msg_d2w.id = TERMINATION_CODE;
      mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);
      wait(NULL);
    }

    for (int i = 0; i < N_SERV2; i++)
    {
      msg_d2w.id = TERMINATION_CODE;
      mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);
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
