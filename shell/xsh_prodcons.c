#include <xinu.h>
#include <ctype.h>
#include <future.h>
#include <prodcons.h>
#include <stdlib.h>
#include <string.h>

int n;

sid32 produced, consumed;
int32 slot=0;
shellcmd xsh_prodcons(int nargs, char * args[]) {

    int count = 2000;
    
    //Checks if there are more than 2 arguments
    if (nargs > 2) {
        kprintf("Too many arguments!\n");
        return 0;
    } else {

        //If there are just 2 arguments
        if (nargs == 2) {
			
			//'-f' is passed to indicate the use of futures
            if(strncmp(args[1],"-f",2)==0) {

                future *f_exclusive, *f_shared, *f_queue;
 
                f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
                f_shared = future_alloc(FUTURE_SHARED);
                f_queue = future_alloc(FUTURE_QUEUE);

                // Test FUTURE_EXCLUSIVE
                resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
                resume( create(future_prod, 1024, 20, "fprod1", 1, f_exclusive) );
                // Test FUTURE_SHARED
                resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
                resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
                resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) ); 
                resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
                resume( create(future_prod, 1024, 20, "fprod2", 1, f_shared) );
                // Test FUTURE_QUEUE
                resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
                resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
                resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
                resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
                resume( create(future_prod, 1024, 20, "fprod3", 1, f_queue) );
                resume( create(future_prod, 1024, 20, "fprod4", 1, f_queue) );
                resume( create(future_prod, 1024, 20, "fprod5", 1, f_queue) );
                resume( create(future_prod, 1024, 20, "fprod6", 1, f_queue) );

                //Free the futures
               	//future_free(f_exclusive);
                //future_free(f_shared);
                //future_free(f_queue);
                return 0;
            }else if(strncmp(args[1],"-n",2) == 0) {

                future *f_exclusive, *f_shared, *f_queue;
 
                f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
                f_shared = future_alloc(FUTURE_SHARED);
                f_queue = future_alloc(FUTURE_QUEUE);

                uint32 remote_ip;
                char ip_addr[] = "192.168.1.101";
                dot2ip(ip_addr,&remote_ip);
                rcvr_pid = getpid();
                slot = udp_register(remote_ip,7,7);

                // Test FUTURE_EXCLUSIVE
                resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
                resume( create(future_prod, 1024, 20, "fprod1", 1, f_exclusive) );
                // Test FUTURE_SHARED
                resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
                resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
                resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) ); 
                resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
                resume( create(future_prod, 1024, 20, "fprod2", 1, f_shared) );
                // Test FUTURE_QUEUE
                resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
                resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
                resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
                resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
                resume( create(future_prod, 1024, 20, "fprod3", 1, f_queue) );
                resume( create(future_prod, 1024, 20, "fprod4", 1, f_queue) );
                resume( create(future_prod, 1024, 20, "fprod5", 1, f_queue) );
                resume( create(future_prod, 1024, 20, "fprod6", 1, f_queue) );

                //Free the futures
                //future_free(f_exclusive);
                //future_free(f_shared);
                //future_free(f_queue);
                return 0;
            }
            else {
                int inputLength = strlen(args[1]);
                int isDigit = 1;

                //Checks whether the input is a number or not.
                for (int i = 0; i < inputLength; ++i)
                {
                    if(isdigit(args[1][i]) == 0)
                    {
                        isDigit = 0;
                        continue;
                    }
                }
                int tempVal = atoi(args[1]);
                if(isDigit != 0 && tempVal > 0) {
                    count = tempVal;
                }
                else {
                    kprintf("Argument should always be an integer greater than zero!\n");
                    return 0;
                }
            }
        }
         //Creating semaphore
         produced = semcreate(0);
         consumed = semcreate(1);
                
         //Create process Producer and Consumer
         resume(create(producer, 1024, 20, "producer", 1, count));
         resume(create(consumer, 1024, 20, "consumer", 1, count));
         return 0;
    }
}
