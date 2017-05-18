#include <xinu.h>
#include <future.h>

future* future_alloc(int future_flag){
        intmask mask;
        mask = disable();
        
        //Allocating space for the future	
		future *f;
		f = getmem(sizeof(future));
		f->value = getmem(sizeof(int));
		//Setting the state and flag value for the future
		f->state = FUTURE_EMPTY;
		f->flag = future_flag;    

        //Initialize both the set_queue and get_queue
        f->get_queue = init_fqueue();
        f->set_queue = init_fqueue();
		restore(mask);
		return f;
}
