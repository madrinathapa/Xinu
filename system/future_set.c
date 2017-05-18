#include <xinu.h>
#include <future.h>

syscall future_set(future* f, int* value) {
	
    intmask mask;
    mask = disable();
    pid32 pid = getpid();

    if (f-> state == FUTURE_VALID ) {
        if (f-> flag == FUTURE_EXCLUSIVE) {
            restore(mask);
            return SYSERR;
        }
         else if (f-> flag == FUTURE_QUEUE) {
            fut_enqueue(pid,f->set_queue);

            if(!is_queueempty(f->get_queue))
            {
                resume(fut_dequeue(f->get_queue));
            }

            suspend(pid);
         }
     }  
    if (f-> state == FUTURE_WAITING) {

            f->state = FUTURE_VALID;
            *f->value = *value;
            send_udp_packet(slot, *value);

        if (f-> flag == FUTURE_EXCLUSIVE) {
            resume(f->pid);
        } 
        else if (f-> flag == FUTURE_SHARED) {

            while(!is_queueempty(f->get_queue))
            {
                resume(fut_dequeue(f->get_queue));
            }
        } 
        else if (f-> flag == FUTURE_QUEUE) {

            if(!is_queueempty(f->get_queue)){
                resume(fut_dequeue(f->get_queue));
            }
        }
    } 
    if (f-> state == FUTURE_EMPTY) {

        if (f-> flag == FUTURE_EXCLUSIVE || f-> flag == FUTURE_SHARED || f-> flag == FUTURE_QUEUE) {
            f->state = FUTURE_VALID;
            *f->value = *value;
            *f->value = fetch_udp_packet(slot);
        } 
    }
    restore(mask);
    return OK;
} 
