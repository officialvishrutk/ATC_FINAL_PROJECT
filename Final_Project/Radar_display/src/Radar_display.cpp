#include <iostream>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <cfenv>
#include <ctime>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <signal.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <sys/socket.h>
//#include <net/netdb.h>
//#include <sys/name.h>
using namespace std;

#define CONSOLE2 "RADAR"
#define ATTACH_POINT_NAME "myname"
#define DISPLAY_POINT_NAME "display"

typedef struct _pulse msg_header_t;

typedef struct from_atc{
	msg_header_t hdr;
	char *message;
}send_to_atc;

typedef struct _my_datac {
//    int timer;
//	msg_header_t hdr;
	 int id;
	 int x;
	 int y;
	 int z;
	 int x_speed;
	 int y_speed;
	 int z_speed;

} my_data_t_c;

typedef struct {
    int type;
    my_data_t_c data;
} my_message_t;

void *client_display(void* args) {
	my_data_t_c *atc_data=(my_data_t_c*)args;
    int server_coid_display; //server connection ID.
    if ((server_coid_display = name_open(CONSOLE2, 0)) == -1) {
        //return EXIT_FAILURE;
    }
    while(1){
    for(int i=0;i<27;i++){
    /* We would have pre-defined data to stuff here */
//    for(int i=0;i<15;i++){
		send_to_atc msgc;
		msgc.hdr.type = 0x00;
		msgc.hdr.subtype = 0x01;
		msgc.message="Please Provide Plane Data";
		/* Do whatever work you wanted with server connection */
		my_data_t_c data;
		if (MsgSend(server_coid_display, &msgc, sizeof(msgc), &data,sizeof(data)) == -1) {
			perror("Error in communication");
		}

		//rintf("ddddddddddddd  %d %d %d  ",data.x,data.y,data.z);
//		if(data.id == 1) printf("id %d: x = %d y = %d z = %d\n",data.id,data.x,data.y,data.z);
		printf("%d",data.id);
		if(data.x==100000)
			{
			if(data.y==100000)
			{
				if( data.z >= 40000 && data.z <= 15000)
				{

					printf("%d Plane within Radar",data.id);
				}
			}
			}
		else
		{
			printf("id %d: x = %d y = %d z = %d\n",data.id,data.x,data.y,data.z);
			printf("%d Plane outside Radar\n",data.id);
		}

//		break;
//		else
//		{
//			printf("%d  ",data.id );
//		}
    }
    printf("\n");
		sleep(1);
//}
//		i++;
    }

    /* Close the connection */
    name_close(server_coid_display);
  //  return EXIT_SUCCESS;
}
int main() {
    std::cout << "Hello" << std::endl;
    my_data_t_c *shared_data=(my_data_t_c *)malloc(sizeof(my_data_t_c)*15);
    pthread_t connect_with_atc;
    pthread_create(&connect_with_atc,NULL,&client_display,shared_data);
    pthread_join(connect_with_atc,NULL);
//    for(int i=0;i<15;i++)
//    {
//
//
//    }
    return 0;
    }
















