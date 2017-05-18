#include <xinu.h>
#include <future.h>

syscall future_get(future *f, int *value) {
    intmask mask;
    mask = disable();
    pid32 pid = getpid();

    if (f-> state == FUTURE_EMPTY) {
        if (f-> flag == FUTURE_EXCLUSIVE) {
            f->state = FUTURE_WAITING;
            f->pid = pid;
            suspend(pid);
        } 
        else if (f-> flag == FUTURE_SHARED || f-> flag == FUTURE_QUEUE) {
            f->state=FUTURE_WAITING;
            fut_enqueue(pid,f->get_queue);
            suspend(pid);
        } 
    }
    if (f-> state == FUTURE_WAITING) {
        if (f-> flag == FUTURE_EXCLUSIVE) {
            restore(mask);
            return SYSERR;
        } 
        else if (f-> flag == FUTURE_SHARED) {
            fut_enqueue(pid,f->get_queue);
            suspend(pid);
        } 
        else if (f-> flag == FUTURE_QUEUE) {
            fut_enqueue(pid,f->get_queue);

            if(!is_queueempty(f->set_queue)){          
                resume(fut_dequeue(f->set_queue));
            }
            suspend(pid);
        }

    } 
    if (f-> state == FUTURE_VALID) {
        *value = *(f->value);
        *value = fetch_udp_packet(slot);
        

        if (f-> flag == FUTURE_EXCLUSIVE) {
            f->state = FUTURE_EMPTY;
            restore(mask);
            return OK;
        } 
        else if (f-> flag == FUTURE_SHARED) {
            
            if(is_queueempty(f->get_queue)){
                f->state = FUTURE_EMPTY;
            }
            restore(mask);
            return OK;
        }

        else if (f-> flag == FUTURE_QUEUE) {

            if(is_queueempty(f->get_queue)){
                f->state = FUTURE_EMPTY;
            }
            else{
                f->state = FUTURE_WAITING;
            }

            if(!is_queueempty(f->set_queue)){
                resume(fut_dequeue(f->set_queue));
            }
            restore(mask);
            return OK;
        }

    }
    restore(mask);
    return OK; 
}