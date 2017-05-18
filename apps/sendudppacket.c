#include <xinu.h>
#include <future.h>
#include <string.h>

void send_udp_packet(int32 slot_num, char* msg){
	
	if(slot_num == SYSERR){
		kprintf("\nError occured! Slot registration failed");
		return;
	}
	int count = 0;

	while(count < 1){

		if(strlen(msg) > SEND_MSG_LENGTH){
			return SYSERR;
      	}
		int send_status = udp_send(slot_num, msg, strlen(msg));
		kprintf("\nsend status %d ", send_status);
		kprintf("\nYour messege has been sent!");

		if(send_status == SYSERR){
			kprintf("\nMessage could not be sent!");		
		}

		count++;
	}
}