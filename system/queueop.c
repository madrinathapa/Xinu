#include <xinu.h>
#include <future.h>
#include <stdio.h>
#include <stdlib.h>

//Method for initializing the queue
node* init_fqueue(){
    
    node *new_node=getmem(sizeof(node));
    new_node->next = NULL;
    return new_node;

}

//Method for checking if the queue is empty
int is_queueempty(node *n){
    
    if(n->next == NULL){
        return 1;
    }
    else{
        return 0;
    }

}

//Method for adding a new node value
void fut_enqueue(pid32 curr_pid, node* curr_queue){
    
    node* new_node = (node*)getmem(sizeof(node));
    new_node->pid = getpid();
    new_node->next = NULL;

    node *prev = curr_queue;
    while(prev->next){
        prev = prev->next;
    }
    prev->next = new_node;
}

pid32 fut_dequeue(node* curr_queue){
    
    if(curr_queue->next != NULL){
        node *free_node = curr_queue->next;
        pid32 free_pid = free_node->pid;
        curr_queue->next = free_node->next;
        freemem(free_node,sizeof(node));
        return free_pid;
    }

}
