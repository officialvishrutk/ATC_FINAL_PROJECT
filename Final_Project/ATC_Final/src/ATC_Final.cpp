#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>
#define consoleid "operatorid"
#define ATTACH_POINT "myname"
#define dis_point "display_plane"
#define CONSOLE "operator"
#define CONSOLE2 "RADAR"
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

using namespace std::chrono;

#define ONE_THOUSAND 1000
#define ONE_MILLION 1000000

#define OFFSET 10000000
#define PERIOD 50000000

sigset_t sigst;

void wait_next_activation() {
    int dummy;
    /* suspend calling process until a signal is pending */
    sigwait(&sigst, &dummy);
}

int start_periodic_timer(uint64_t offset, int period) {
    struct itimerspec timer_spec;
    struct sigevent sigev;
    timer_t timer;
    const int signal = SIGALRM;
    int res;

    /* set timer parameters */
    timer_spec.it_value.tv_sec = offset / ONE_MILLION;
    timer_spec.it_value.tv_nsec = (offset % ONE_MILLION) * ONE_THOUSAND;
    timer_spec.it_interval.tv_sec = period / ONE_MILLION;
    timer_spec.it_interval.tv_nsec = (period % ONE_MILLION) * ONE_THOUSAND;

    sigemptyset(&sigst); // initialize a signal set
    sigaddset(&sigst, signal); // add SIGALRM to the signal set
    sigprocmask(SIG_BLOCK, &sigst, NULL); //block the signal

    /* set the signal event a timer expiration */
    memset(&sigev, 0, sizeof(struct sigevent));
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = signal;

    /* create timer */
    res = timer_create(CLOCK_MONOTONIC, &sigev, &timer);

    if (res < 0) {
        perror("Timer Create");
        exit(-1);
    }

    /* activate the timer */
    return timer_settime(timer, 0, &timer_spec, NULL);
}

static void task_body(void) {
    static int cycles = 0;
    static uint64_t start;
    uint64_t current;
    struct timespec tv;

    if (start == 0) {
        clock_gettime(CLOCK_MONOTONIC, &tv);
        start = tv.tv_sec * ONE_THOUSAND + tv.tv_nsec / ONE_MILLION;
    }

    clock_gettime(CLOCK_MONOTONIC, &tv);
    current = tv.tv_sec * ONE_THOUSAND + tv.tv_nsec / ONE_MILLION;

    if (cycles > 0) {
        std::cerr << "Ave interval between instances: " << (double)(current-start)/cycles << " milliseconds\n";
    }

    cycles++;
}

//sigset_t sigst;
typedef struct _pulse msg_header_t;

typedef struct _my_datac{

	    msg_header_t hdr;
	    int timer;

	}my_data_t_c;

typedef struct{
	msg_header_t hdr;
	char *message;
}from_operator;

typedef struct {

int id;
int x;
int y;
int z;
int x_speed;
int y_speed;
int z_speed;
} Radar;

typedef struct _my_data {

	    msg_header_t hdr;
	    int id;
	    int id_1;
	    int id_2;
	    int id_3;
	    char coll[20];
	    Radar sendarr[1200];


	}my_data_t;


    void *server(void *args) {
    	Radar *csv_data=(Radar*)args;
    	from_operator msg;
        name_attach_t *attach;
        int i = 0;
        Radar *t_data=(Radar*)malloc(sizeof(Radar));

        int rcvid;
        my_data_t_c msgc;

        /* Create a local name (/dev/name/local/...) */
        if ((attach = name_attach(NULL, CONSOLE, 0)) == NULL) {
            //return EXIT_FAILURE;
        }

        /* Do your MsgReceive's here now with the chid */
        while (1) {
            int timer;

            /* Server will block in this call, until a client calls MsgSend to send a message to
             * this server through the channel named "myname", which is the name that we set for the channel,
             * i.e., the one that we stored at ATTACH_POINT and used in the name_attach call to create the channel. */
            rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

            /* In the above call, the received message will be stored at msg when the server receives a message.
             * Moreover, rcvid */

            if (rcvid == -1) {/* Error condition, exit */
                break;
            }

            if (rcvid == 0) {/* Pulse received */
                switch (msg.hdr.code) {
                case _PULSE_CODE_DISCONNECT:
                    /*
                     * A client disconnected all its connections (called
                     * name_close() for each name_open() of our name) or
                     * terminated
                     */
                    ConnectDetach(msg.hdr.scoid);
                    break;
                case _PULSE_CODE_UNBLOCK:
                    /*
                     * REPLY blocked client wants to unblock (was hit by
                     * a signal or timed out).  It's up to you if you
                     * reply now or later.
                     */
                    break;
                default:
                    /*
                     * A pulse sent by one of your processes or a
                     * _PULSE_CODE_COIDDEATH or _PULSE_CODE_THREADDEATH
                     * from the kernel?
                     */
                    break;
                }
                continue;
            }

            /* name_open() sends a connect message, must EOK this */
            if (msg.hdr.type == _IO_CONNECT ) {
                MsgReply( rcvid, EOK, NULL, 0 );
                continue;
            }

            /* Some other QNX IO message was received; reject it */
            if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX ) {
                MsgError( rcvid, ENOSYS );
                continue;
            }

            //	   /* Here are the messages that you will work on, i.e., messages that will have a meaning
            if (msg.hdr.type == 0x00) {
				 if (msg.hdr.subtype == 0x01) {
					 /* A message (presumable ours) received, handle */
            	if(i==0){

					  t_data=csv_data;
				  }
           //                      std::cout << "Display Collision detected between " << msg.message << std::endl;
                           	  printf("%s",msg.message);
                                 Radar data;
                                 data.id=t_data->id;
                                 data.x=t_data->x;
                                 data.y=t_data->y;
                                 data.z=t_data->z;
                                 data.x_speed=t_data->x_speed;
                                 data.y_speed=t_data->y_speed;
                                 data.z_speed=t_data->z_speed;
             					MsgReply(rcvid,EOK,&data,sizeof(data));
             				//	printf("%d %d",data->id,data->id);
             					t_data++;
             				//	sleep(1);
             					i++;
             					if(i==27){
             						//csv_data=t_data;
             						i=0;
             						///////update input data




             					}
             				//	printf("%d,%d,%d",t_data->x,t_data->y,t_data->z);
                             }
                         }
//            break;
        }

   //     return 0;
    }

    void *server2(void *args) {
      	Radar *csv_data=(Radar*)args;
      	from_operator msg;
          name_attach_t *attach;

          int rcvid;
          my_data_t_c msgc;
          int i = 0;
          Radar *t_data=(Radar*)malloc(sizeof(Radar));

          /* Create a local name (/dev/name/local/...) */
          if ((attach = name_attach(NULL, CONSOLE2, 0)) == NULL) {
              //return EXIT_FAILURE;
          }

          /* Do your MsgReceive's here now with the chid */
          printf("Starting Server2\n");
          while (1) {
              int timer;

              /* Server will block in this call, until a client calls MsgSend to send a message to
               * this server through the channel named "myname", which is the name that we set for the channel,
               * i.e., the one that we stored at ATTACH_POINT and used in the name_attach call to create the channel. */
              rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

              /* In the above call, the received message will be stored at msg when the server receives a message.
               * Moreover, rcvid */

              if (rcvid == -1) {/* Error condition, exit */
                  break;
              }

              if (rcvid == 0) {/* Pulse received */
                  switch (msg.hdr.code) {
                  case _PULSE_CODE_DISCONNECT:
                      /*
                       * A client disconnected all its connections (called
                       * name_close() for each name_open() of our name) or
                       * terminated
                       */
                      ConnectDetach(msg.hdr.scoid);
                      break;
                  case _PULSE_CODE_UNBLOCK:
                      /*
                       * REPLY blocked client wants to unblock (was hit by
                       * a signal or timed out).  It's up to you if you
                       * reply now or later.
                       */
                      break;
                  default:
                      /*
                       * A pulse sent by one of your processes or a
                       * _PULSE_CODE_COIDDEATH or _PULSE_CODE_THREADDEATH
                       * from the kernel?
                       */
                      break;
                  }
                  continue;
              }

              /* name_open() sends a connect message, must EOK this */
              if (msg.hdr.type == _IO_CONNECT ) {
                  MsgReply( rcvid, EOK, NULL, 0 );
                  continue;
              }

              /* Some other QNX IO message was received; reject it */
              if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX ) {
                  MsgError( rcvid, ENOSYS );
                  continue;
              }

              //	   /* Here are the messages that you will work on, i.e., messages that will have a meaning

//            t_data=NULL;
              if (msg.hdr.type == 0x00) {
                  if (msg.hdr.subtype == 0x01) {
                      /* A message (presumable ours) received, handle */
//                      std::cout << "Display Collision detected between " << msg.message << std::endl;
              if(i==0){

            	  t_data=csv_data;
              }
                	  printf("%s",msg.message);
                      Radar data;
                      data.id=t_data->id;
                      data.x=t_data->x;
                      data.y=t_data->y;
                      data.z=t_data->z;
                      data.x_speed=t_data->x_speed;
                      data.y_speed=t_data->y_speed;
                      data.z_speed=t_data->z_speed;
  					MsgReply(rcvid,EOK,&data,sizeof(data));
  				//	printf("%d %d",data->id,data->id);
  					t_data++;

  					i++;
  					if(i==27){
  						//csv_data=t_data;
  						i=0;
  						///////update input data
//  						sleep(20);
  						for(int i=0;i<27;i++){
  							csv_data->x+=100;
  							csv_data->y+=100;
  							csv_data->z+=100;
  						}


  					}
  				//	printf("%d,%d,%d",t_data->x,t_data->y,t_data->z);
                  }
              }
  //            break;
          }

     //     return 0;
      }

    void read_csv(void * args){
    	Radar *csv_data=(Radar*)args;//have to create it malloc
    	char name[20]="Planes-med.csv";
    	char line[4098];
    	//struct top *tptr=(struct top*)arg;
    	int *data=(int*)malloc(8*sizeof(int));
    	/* open the shared memory segment */
    	FILE *file;
    	file = fopen(name, "r");
    	while(fgets(line,4098,file)){
    		char* tmp = strdup(line);
    		char* tok;
    		int i=0;
    		//data[i++]=tptr->plane_count;
    		for(tok=strtok(line,",");  tok && *tok;tok = strtok(NULL, ",")){
    			 data[i++]=atoi(tok);

    		}

    			csv_data->id=data[0];
    			csv_data->x=data[1];
    			csv_data->y=data[2];
    			csv_data->z=data[3];
    			csv_data->x_speed=data[4];
				csv_data->y_speed=data[5];
				csv_data->z_speed=data[6];

				csv_data++;

//				csv_data->=data[7];



    		free(tmp);

    	}

    	fclose(file);


    }


    void run_server(void *args){
    	   Radar *csv_data=(Radar*)args;
    	   pthread_t run_server;
    	   pthread_create(&run_server,NULL,&server,csv_data);

    }
   void run_server2(void *args){
	   Radar *csv_data=(Radar*)args;
	   pthread_t run_server2;
	   pthread_create(&run_server2,NULL,&server2,csv_data);
	   pthread_join(run_server2,NULL);

   }

int main(void){
	Radar myradar;

float dist_p_p1,dist_p_p2,dist_p_p3;
float time =5;
printf("HELLO");
//Radar arr[1200];
Radar *csv_data=(Radar*)malloc(sizeof(Radar)*26);
my_data_t_c msgc;

pthread_t thread_radar[1200];
read_csv(csv_data);

run_server(csv_data);
run_server2(csv_data);

   return 0;

}
