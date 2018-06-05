#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <assert.h>
#include <pthread.h>

//create linked list
typedef struct node{
  int num;
  struct node *next;
  int vacant;
}number;

//allow for multi-arg
struct argument{
  number *head;
  int count;
  int upperlimit;
};

//create list of numbers
void* createList(void* argpoint){
  struct argument args =  *((struct argument*) argpoint);
  number *current= args.head;
  number *adjacent= args.head->next;
  //generate random numbers
  int i;
  for(i=0;i<args.count;i++){
    current->num=rand()%args.upperlimit;
    adjacent->vacant=0;
    current=adjacent;
    current->next=malloc(sizeof(number));
    adjacent=current->next;
  }
  current->vacant=1;
  return (void*) args.head;
}

void* checkList(void* args){
  number *head=(number*)args;
  number*current=head;
  number*adjacent=head->next;
  //handle numbers within 8 of each other
  while(!(current->vacant)){
    if(abs(adjacent->num - current->num) <=8 && (adjacent->vacant==0)){
      if(adjacent->next->vacant){
	adjacent->num=0;
	adjacent->vacant=1;
	break;
      }else{
	number *temp=adjacent;
	current->next=adjacent->next;
	adjacent=current->next;
	free(temp);
	continue;
      }
    }
    current=current->next;
    adjacent=current->next;
  }
  return (void*) head;
}



main(argc,argv)
int argc;
char *argv[]; {


    int count,upperlimit,x;

/*	check to make sure we pass in arguments  */
    if(argc <= 2) {
	printf("Usage:  %s Count UpperLimit\n",argv[0]);
	exit(0);
    }
/*	we've checked to make sure arguments are there,  */
/*	it is now save to access argv[1] and argv[2]     */
    count = atoi(argv[1]);
    upperlimit = atoi(argv[2]);
    srand((int)time(NULL));
    number*head=malloc(sizeof(number));
    head->next=malloc(sizeof(number));
    
    //create first thread
    pthread_t T1ID;
    struct argument T1args;
    T1args.count=count;
    T1args.head=head;
    T1args.upperlimit=upperlimit;
    void *T1argsPointer = &T1args;
    pthread_attr_t T1attr;
    pthread_attr_init(&T1attr);
    pthread_attr_setdetachstate(&T1attr, PTHREAD_CREATE_JOINABLE);

    //create second thread
    pthread_t T2ID;
    pthread_attr_t T2attr;
    pthread_attr_init(&T2attr);
    pthread_attr_setdetachstate(&T2attr, PTHREAD_CREATE_JOINABLE);

    //cretae list
    pthread_create(&T1ID,NULL,createList,T1argsPointer);
    pthread_join(T1ID, (void**)&head);

    //check list
    pthread_create(&T2ID,NULL,checkList, (void*) head);
    pthread_join(T2ID, (void**)&head);

    number*current=head;
    number*adjacent=head->next;

    //print list
    while(!(current->vacant)){
      printf("%d\n",current->num);
      current=adjacent;
      adjacent=current->next;
    }

    return 0;
}
