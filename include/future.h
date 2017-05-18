#ifndef _FUTURE_H_
#define _FUTURE_H_
 
/* define states */
#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1         
#define FUTURE_VALID 	  2         

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1	
#define FUTURE_SHARED     2	    
#define FUTURE_QUEUE      3     

//For network
#define RCV_TIMEOUT 10000
#define SEND_MSG_LENGTH 40
#define RECVR_MSG_LENGTH 100

typedef struct qnode 
{
   pid32 pid;
   struct qnode *next;
}node;

typedef struct futent
{
   int *value;		
   int flag;		
   int state;         	
   pid32 pid;
   node* set_queue;     
   node* get_queue;
}future;

/* Interface for system call */
future* future_alloc(int future_flags);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);
 
uint future_cons(future *fut);
uint future_prod(future *fut);

void fut_enqueue(pid32 curr_pid, node* curr_queue);
pid32 fut_dequeue(node* curr_queue);
node* init_fqueue();
int is_queueempty(node *fq);

//For network
extern int32 slot;
extern pid32 rcvr_pid;
void send_udp_packet(int32 slot, char* msg);
int fetch_udp_packet(int32 slot);

#endif /* _FUTURE_H_ */