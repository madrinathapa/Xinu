#include <xinu.h>
#include <future.h>

int fetch_udp_packet(int32 slot);
pid32 rcvr_pid;

uint future_cons(future *fut) {

    int i, status;
    status = future_get(fut, &i);

    if (status < 1) {
        kprintf("Future_get failed\n");
        return -1;
    }

    kprintf("Value consumed: %u \n", i);

    if((fut->state == FUTURE_EXCLUSIVE) || (fut->state == FUTURE_SHARED && is_queueempty(fut->get_queue))) {
    	send(rcvr_pid, "OK");
    }
    else{
    	if(is_queueempty(fut->get_queue) && !is_queueempty(fut->set_queue)){
    		send(rcvr_pid, "OK");
    	}
    }
    return OK;
}
