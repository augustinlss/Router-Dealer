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

  // todo 
  pid_t workers1[N_SERV1];
  pid_t workers2[N_SERV2];
  // todo end


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

  client_1 = fork();

  if(client_1 == 0) {

    execlp("./client", "client", client2dealer_name, NULL);

  } else {
    for (int i = 0; i < N_SERV1; i++)
    {
      pid_t temp = fork();
      if (temp == 0) {
        execlp(
          "./worker_s1", "worker_s1",
           dealer2worker1_name, worker2dealer_name, NULL);
      }
      // todo 
      workers1[i] = temp;
      // todo end      
    }

    for (int i = 0; i < N_SERV2; i++)
    {
      pid_t temp = fork();
      if (temp == 0) {
        execlp(
          "./worker_s2", "worker_s2",
          dealer2worker2_name, worker2dealer_name, NULL);
      }
      // todo 
      workers2[i] = temp;
      // todo end
    }
    
    MQ_CLIENT2DEALER_MESSAGE msg_c2d;
    MQ_DEALER2WORKER_MESSAGE msg_d2w;
    MQ_DEALER2WORKER_MESSAGE msg_w2d;
    

    while (true) {
      struct mq_attr attr_c2d;
      struct mq_attr attr_w2d;

      mq_getattr(mq_client2dealer, &attr_c2d);
      if (attr_c2d.mq_curmsgs > 0) {
        mq_receive(mq_client2dealer, (char *) &msg_c2d, sizeof(msg_c2d), NULL);
        if (msg_c2d.id == TERMINATION_CODE) {
          // todo 
          printf("client to dealer queue empty, and first loop stop\n");
          // todo end
          break;
        }
        msg_d2w.id = msg_c2d.id;
        msg_d2w.data = msg_c2d.data;
        if (msg_c2d.service_type == 1) {
          mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);
        }else {
          mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);
        }
      }
      // todo
      // else{
      //   printf("client to dealer channel empty\n");
      // }
      // todo end

      mq_getattr(mq_worker2dealer, &attr_w2d);
      if (attr_w2d.mq_curmsgs > 0) {
        // todo
        printf("msg found in the repsonse channel\n");
        // todo end
        mq_receive(mq_worker2dealer, (char*) &msg_w2d, sizeof(msg_w2d), NULL);
        printf("%d -> %d\n",msg_w2d.id, msg_w2d.data);
      }
      // todo
      // else{
      //   printf("response channel empty\n");
      // }
      // todo end
      // todo
      // sleep(10);
      // todo end
    }

    int client_status = 0;
    waitpid(client_1, &client_status, 0);
    // todo 
    printf("From dealer, client process released, with status: %d\n", client_status);
    // todo end

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

    // todo
    printf("From dealer, all termination signals send\n");
    // todo end
    
    int num_termination_from_workers = 0;
    while (num_termination_from_workers != (N_SERV1 + N_SERV2)) {
      mq_receive(mq_worker2dealer, (char*) &msg_w2d, sizeof(msg_w2d), NULL);
      if (msg_w2d.id == TERMINATION_CODE) {
        // todo
        printf("From dealer, termination signal received from worker\n");
        // todo end
        num_termination_from_workers++;
      }
      else {
        printf("%d -> %d\n",msg_w2d.id, msg_w2d.data);
      }
    }

    // todo
    printf("From dealer, second while loop end\n"); 
    // todo end

    for (int i = 0; i < (N_SERV1 + N_SERV2); i++)
    {
        wait(NULL);
    }


    // int clien_status = 0;
    // int cliend_id = 0;

    // while (client_1 != cliend_id) {
      
    //   if (mq_receive(mq_client2dealer, (char *) &msg_c2d, sizeof(msg_c2d), NULL) != -1) {
    //     msg_d2w.id = msg_c2d.id;
    //     msg_d2w.data = msg_c2d.data;

    //     if (msg_c2d.service_type == 1) {
    //       mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);
    //     }else {
    //       mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);
    //     }

    //     mq_receive(mq_worker2dealer, (char*) &msg_w2d, sizeof(msg_w2d), NULL);
    //     printf("%d -> %d\n",msg_w2d.id, msg_w2d.data);

    //     cliend_id = waitpid(client_1, &clien_status, WNOHANG);
    //     printf("from first while\n");
    //   }

    // }

    // while (true) {
    //   printf("from first while\n");
    //   int is_msg_c2d_left = mq_receive(mq_client2dealer, (char *) &msg_c2d, sizeof(msg_c2d), NULL);
    //   // if (is_msg_c2d_left == -1 && errno == EAGAIN) {
    //   if (is_msg_c2d_left == -1) {
    //     break;
    //   } 

    //   msg_d2w.id = msg_c2d.id;
    //   msg_d2w.data = msg_c2d.data;

    //   if (msg_c2d.service_type == 1) {
    //     mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);
    //   }else {
    //     mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);
    //   }

    //   mq_receive(mq_worker2dealer, (char*) &msg_w2d, sizeof(msg_w2d), NULL);
    //   printf("%d -> %d\n",msg_w2d.id, msg_w2d.data);
    // }


    // for (int i = 0; i < N_SERV1; i++)
    // {
    //   msg_d2w.id = TERMINATION_CODE;
    //   mq_send(mq_dealer2worker1, (char*) &msg_d2w, sizeof(msg_d2w), 0);
    //   
    // }

    
    

    // for (int i = 0; i < N_SERV2; i++)
    // {
    //   msg_d2w.id = TERMINATION_CODE;
    //   mq_send(mq_dealer2worker2, (char*) &msg_d2w, sizeof(msg_d2w), 0);
    //   
    // }

    // for (int i = 0; i < N_SERV1 + N_SERV2; i++)
    // {
    //     wait(NULL);
    // }
    
     
  }

  // todo

  int status_c;
  pid_t cid_c = waitpid(client_1, &status_c, WNOHANG);
  if (cid_c == 0 || cid_c == -1) {
      printf("Client has been released before.\n");
    } 
  else {
      printf("Client has NOT been released. waitpid returns %d\n", cid_c);
  }
  
  int worker_cnt = 0;
  for (int i = 0; i < N_SERV1; i++)
  {
    worker_cnt++;
    int status_w;
    pid_t cid = waitpid(workers1[i], &status_w, WNOHANG);
    if (cid == 0 || cid == -1) {
      printf("Worker1, number: %d, has been released before.\n", worker_cnt);
    } 
    else {
      printf("Worker1, number: %d, has NOT been released. waitpid returns %d\n", worker_cnt, cid);
    }
  }
  worker_cnt = 0;

  for (int i = 0; i < N_SERV2; i++)
  {
    worker_cnt++;
    int status_w;
    pid_t cid = waitpid(workers2[i], &status_w, WNOHANG);
    if (cid == 0 || cid == -1) {
      printf("Worker2, number: %d, has been released before.\n", worker_cnt);
    } 
    else {
      printf("Worker2, number: %d, has NOT been released. waitpid returns %d\n", worker_cnt, cid);
    }
  }
  
  // todo end

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
