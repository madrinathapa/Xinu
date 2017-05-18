#include <xinu.h>
#include <future.h>

int fetch_udp_packet(int32 slot_num){
	if(slot_num == SYSERR){
		kprintf("\nError occured! Slot registration failed");
		return;					
	}
	int count = 0;
	while(count < 1){
		char msg_rcvd[RECVR_MSG_LENGTH] = "";
		int status_rcvd = udp_recv(slot_num, &msg_rcvd, RECVR_MSG_LENGTH, RCV_TIMEOUT);

		if(status_rcvd != SYSERR){
		    if(status_rcvd == TIMEOUT){
				kprintf("\nReceiver timed out!!");
			}else{
				kprintf("\nReceived message is: %s ", msg_rcvd);
                return atoi(msg_rcvd);
			}	
		}	
		count++;
	}
}
