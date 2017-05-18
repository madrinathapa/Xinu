#include <xinu.h>
#include <future.h>

syscall future_free(future* f){
	
	intmask mask;
	mask = disable();
    
    freemem(f, sizeof(future));

	restore(mask);
	return OK;
}
