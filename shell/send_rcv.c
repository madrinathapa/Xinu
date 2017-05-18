#include <xinu.h>
#include <future.h>

void send_rcv(char *remote_ip_dot, uint32 remote_port, uint16 local_port){
	uint32 remote_ip;
    char *arr[][20] = {"192.168.1.102","192.168.1.103","192.168.1.104","192.168.1.105","192.168.1.106","192.168.1.107","192.168.1.108","192.168.1.109","192.168.1.1010","192.168.1.111"};
    char *portArr[][20] = {"50411","50412","50413","50414","50415","50416","50417","50418","50419","50420"};
    dot2ip(remote_ip_dot,&remote_ip);
	int32 new_slot = udp_register(remote_ip,remote_port,local_port);
     
	if(new_slot == SYSERR){
		printf("\nError occurred! UDP could not be registered");
		return;					
	}

	int count = 0;
	while(count < 1){
		char send_msg[SEND_MSG_LENGTH] = "This is XINU here!";
		int send_status = udp_send(new_slot, send_msg, SEND_MSG_LENGTH);
		if(send_status == SYSERR){
			printf("\nSending failed!");		
		}
		else{
			char recv_msg[RECVR_MSG_LENGTH]= "";
			int rcv_status = udp_recv(new_slot, &recv_msg, RECVR_MSG_LENGTH, RCV_TIMEOUT);
			if(rcv_status == TIMEOUT){
				printf("\nTimed out!!");
			}
			else if(rcv_status == SYSERR){
				printf("\nMessage recieved failed!");
			}			
			else{
				printf("\n Message recieved: %s",recv_msg);		
			}     
		}
		count++;	
			
	}
	int new_release = udp_release(new_slot);
    
	if(new_release == SYSERR){
		printf("\nUDP slot couldn't be released!");
	}
}