#include <xinu.h>
#include <future.h>

void send_udp_packet(int32 slot, char* msg);

uint future_prod(future *fut) {

    int i, j;

   	j = (int)fut; 

   	for (i=0; i<1000; i++) {
   		j += i;
   	}
    
	kprintf("Value produced: %u\n", j);
    future_set(fut, &j);
    return OK;
}
