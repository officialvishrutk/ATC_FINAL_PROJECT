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

#define CONSOLE "operator"
#define ATTACH_POINT_NAME "myname"
#define DISPLAY_POINT_NAME "display"

typedef struct _pulse msg_header_t;

typedef struct from_atc{
	msg_header_t hdr;
	char *message;
}send_to_atc;

typedef struct _my_datac {
//    int timer;

	 int id;
	 int x;
	 int y;
	 int z;
	 int x_speed;
	 int y_speed;
	 int z_speed;


} my_data_t_c;

typedef struct my_data{
	my_data_t_c *marr;
}data_arr;
typedef struct {
    int type;
    my_data_t_c data;
} my_message_t;


void *client_display(void* args) {
	printf(".......");
	data_arr *atc_data=(data_arr*)args;
    int server_coid_display; //server connection ID.
    if ((server_coid_display = name_open(CONSOLE, 0)) == -1) {
        //return EXIT_FAILURE;
    }
    atc_data->marr=(my_data_t_c*)malloc(sizeof(my_data_t_c)*27);
    my_data_t_c *temp=(my_data_t_c*)malloc(sizeof(my_data_t_c));
    my_data_t_c *temp2=(my_data_t_c*)malloc(sizeof(my_data_t_c));
    temp=atc_data->marr;

    while(1){
    //	printf("loop open");
    /* We would have pre-defined data to stuff here */
    for(int i=0;i<27;i++){
		send_to_atc msgc;
		msgc.hdr.type = 0x00;
		msgc.hdr.subtype = 0x01;
		msgc.message="Please Provide Plane Data";
		/* Do whatever work you wanted with server connection */
		my_data_t_c data;
		if (MsgSend(server_coid_display, &msgc, sizeof(msgc), &data, sizeof(my_data_t_c)) == -1) {
			perror("Error in communication");
		}

			printf("%d",data.id);

			atc_data->marr->id=data.id;
			atc_data->marr->x=data.x;
			atc_data->marr->y=data.y;
			atc_data->marr->z=data.z;
			atc_data->marr->x_speed=data.x_speed;
			atc_data->marr->y_speed=data.y_speed;
			atc_data->marr->z_speed=data.z_speed;
			atc_data->marr++;


    }
			for (int i = 0; i < 27; i++)
				{
				printf("Plane %d\n",temp->id);
//				temp++;
					int x1 = temp->x;
					int y1 = temp->y;
					int z1 = temp->z;
					temp2=temp+1;
					for (int j=i+1; j<27; j++)
					{
						int x2 = temp2->x;
						int y2 = temp2->y;
						int z2 = temp2->z;

						int x_diff = abs(x1 - x2);
						int y_diff = abs(y1 - y2);
						int z_diff = abs(z1 - z2);

						if ((z_diff <= 1000) && (x_diff <= 3000) && (y_diff <= 3000))
						{
							printf("Collision Detected Between Plane %d and Plane %d\n",temp->id,temp2->id);
						}
						else printf("Plane %d and Plane %d have safe distance\n",temp->id,temp2->id);
						temp2++;
					}
					temp++;
				}
			sleep(3);
    }


    /* Close the connection */
    name_close(server_coid_display);
  //  return EXIT_SUCCESS;
}

int main() {
    std::cout << "Hello" << std::endl;
    data_arr *shared_data=(data_arr*)malloc(sizeof(data_arr));
    pthread_t connect_with_atc;
    pthread_create(&connect_with_atc,NULL,&client_display,shared_data);
    pthread_join(connect_with_atc,NULL);

    return 0;
}
