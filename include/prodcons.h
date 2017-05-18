/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

/*function Prototype*/
void consumer(int count);
void producer(int count);

/* Semaphore Declarations */
extern sid32 produced, consumed;