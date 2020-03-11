#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NUMBER 20 //the length of array
typedef struct node{
	void* data;
	int type;
	struct node* next;
}NODE; 
typedef struct node* PNODE;

PNODE create(void);
/*Create a new linked list with an empty node*/
PNODE InsertFro(void *x, int type, PNODE head);
/*Insert a new node before head of a linked list*/
PNODE InsertBack(void *x, int type, PNODE head);
/*Insert a new node after the final node of a linked list*/
PNODE InsertI(void *x, int type, PNODE head, int i);
/*Insert a new node at position i*/
void Print(PNODE head);
/*Printf key value of all the nodes in a linked list in order.*/ 
PNODE DeleteHead(PNODE head);
/*Delete head of a linked list.*/
PNODE DeleteTail(PNODE head);
/*Delete the final node of a linked list.*/
PNODE DeleteI(PNODE head, int i);
/*Delete the node i*/
PNODE Destroy(PNODE head);
/*Destroy a linked list.*/
PNODE Invert(PNODE head);
/*Invert a linked list.*/

int main(void){
	PNODE head;
	int choice, insertPlace, deletePlace, type, num;
	int *x;
	double *y;
	char (*a)[NUMBER];  //Define three kinds of pointers
	do{
		printf("1:create 2:insert 3:print 4:delete 5:destroy 6:invert 0:exit\n");
		scanf("%d", &choice);
		switch(choice){
			case 0:
				break;
			case 1:
				head = create();
				break;
			case 2:
				printf("1:insert before head 2:insert after tail 3:insert at position i\n");
				scanf("%d", &insertPlace);  //Choose where to insert
			    printf("which data type? 0:int 1:double 2:string\n");
				scanf("%d", &type);  //store the type of data stored
				printf("Input data:");
				switch(insertPlace){
					case 1:
						switch(type){  
							case 0:    //insert:int
								x = (int*)malloc(sizeof(int)); 
							    scanf("%d", x);
							    head = InsertFro(x, type, head); 
							    break;
							case 1:    //insert:double
								y = (double*)malloc(sizeof(double));
							    scanf("%lf", y);
								head = InsertFro(y, type, head); 
							    break;
							case 2:    //insert:string
								a = (char(*)[NUMBER])malloc(NUMBER * sizeof(char));
							    scanf("%s", *a);
								head = InsertFro(*a, type, head); 
								break;	
							default:
								printf("Illegal Input!\n\n");
								break;
						}
					    break;
					case 2:
						switch(type){
							case 0:    //insert:int
								x = (int*)malloc(sizeof(int));
							    scanf("%d", x);
							    head = InsertBack(x, type, head); 
							    break;
							case 1:    //insert:double
								y = (double*)malloc(sizeof(double));
							    scanf("%lf", y);
								head = InsertBack(y, type, head); 
							    break;
							case 2:    //insert:string
								a = (char(*)[NUMBER])malloc(NUMBER * sizeof(char));
							    scanf("%s", *a);
								head = InsertBack(*a, type, head); 
								break;
							default:
								printf("Illegal Input!\n\n");
							    break;	
						}
					    break;
					case 3:
						switch(type){
							case 0:    //insert:int
								x = (int*)malloc(sizeof(int)); 
							    scanf("%d", x);
							    printf("Input i:");
                		        scanf("%d", &num);
							    head = InsertI(x, type, head, num); 
							    break;
							case 1:    //insert:double
								y = (double*)malloc(sizeof(double));
							    scanf("%lf", y);
							    printf("Input i:");
                		        scanf("%d", &num);
								head = InsertI(y, type, head, num); 
							    break;
							case 2:    //insert:string
								a = (char(*)[NUMBER])malloc(NUMBER * sizeof(char));
							    scanf("%s", *a);
							    printf("Input i:");
                	        	scanf("%d", &num);
								head = InsertI(*a, type, head, num); 
								break;	
							default:
								printf("Illegal Input!\n\n");
								break;
						}
					    break;
					    default:
								printf("Illegal Input!\n\n");
								break;
				}
				break;
		    case 3:
		    	Print(head);
		    	break;
		    case 4:
                printf("1:delete the head 2:delete the tail 3:delete the ith number\n");
                scanf("%d", &deletePlace);
                switch(deletePlace){
                	case 1:
                		head = DeleteHead(head);
                		break;
                	case 2:
                		head = DeleteTail(head);
                		break;
                	case 3:
                		printf("Input i:");
                		scanf("%d", &num);
                		head = DeleteI(head, num);
                		break;
                	default:
                		printf("Illegal Input!\n\n");
                		break;
				}
				break;
			case 5:
				head = Destroy(head);
				break;
			case 6:
                head = Invert(head); 
                break;
			default:
				printf("Illegal Input!\n\n");
				break;
		}
	}while(choice != 0);
} 

/*Create a new linked list with an empty node*/
PNODE create(void){
	PNODE ptr;
	ptr = (PNODE)malloc(sizeof(NODE));
	ptr->next = NULL;
	return ptr;
}
 
/*Insert a new node before head of a linked list*/
PNODE InsertFro(void *x, int type, PNODE head){
	PNODE ptr;
	ptr = (PNODE)malloc(sizeof(NODE));
	ptr->data = x;
	ptr->type = type;
	ptr->next = head->next;
	head->next = ptr;
	return head;
} 

/*Insert a new node after the final node of a linked list*/
PNODE InsertBack(void *x, int type, PNODE head){
	PNODE ptr, ptr1;
	ptr = (PNODE)malloc(sizeof(NODE));
	ptr->data = x;
	ptr->type = type;
	ptr->next = NULL;
	if(head->next == NULL){
	    head->next = ptr;
        return head;
	}	    
	ptr1 = head->next;
	while(ptr1->next != NULL){
		ptr1 = ptr1->next;
	}
	ptr1->next = ptr;
	return head;
}

/*Insert a node at position i*/ 
PNODE InsertI(void *x, int type, PNODE head, int i)
{
	PNODE p, q, ptr;
	ptr = (PNODE)malloc(sizeof(NODE));
	ptr->data = x;
	ptr->type = type;
	ptr->next = NULL;
	int j, n = 0;
	p = head;
	while(p->next != NULL){    //get the number of nodes
		p = p->next;
		n++;
	}
    if(i < 1 || i > n + 1){  //check if position i exists
    	printf("Error!NO Enough Nodes!\n");
	}
	else if(i == n + 1){
		p->next = ptr;
	}
	else{
		q = head;
	    for(j = 1; j < i ; j++){ 
		    q = q->next;
	    }
	    ptr->next = q->next;
	    q->next = ptr;
	}
	return head; 
}

/*Printf key value of all the nodes in a linked list in order.*/ 
void Print(PNODE head){
	head = head->next;
	if(head == NULL){
		printf("NULL\n");
		return;
	}
	while(head != NULL){
		switch(head->type){
			case 0:
				printf("%d ", *(int*)head->data);
				break;
			case 1:
				printf("%lf ", *(double*)head->data);
				break;
			case 2:
				printf("%s ", *(char(*)[NUMBER])head->data);
				break;
		}
		head = head->next;
	}
	printf("\n");
}

/*Delete head of a linked list.*/
PNODE DeleteHead(PNODE head)   
{
	PNODE p, q;
	p = head->next;
	if(p == NULL){
		printf("NO Element!");
	} 
	else{
		q = p->next;
		free(p->data);
		free(p);
		head->next = q;
	}
	return head; 	 
}

/*Delete the final node of a linked list.*/
PNODE DeleteTail(PNODE head)	
{
	PNODE p, q;
	p = q = head->next;
	if(p == NULL){
		printf("NO Element!\n");
	} 
	else{
		while(q->next != NULL){
			p = q;
			q = q->next;
		}
		free(q->data);
		free(q);
		p->next = NULL;
	}
	return head; 	 
}

/*Delete the node i*/
PNODE DeleteI(PNODE head,int i)
{
	PNODE p, q;
	int j, n = 0;
	p = head->next;
	while(p){
		p = p->next;
		n++;
	}
	if(i < 1 || i > n){  //check if node i exists
    	printf("Error!Not Found!\n");
	}
	else{
		p = q = head;
	    for(j = 0; j < i ; j++){ 
	        p = q;
		    q = q->next;
	    }
	    p->next = q->next;
	    free(q->data);
	    free(q);
	}
	return head; 
}

/*Destroy a linked list.*/
PNODE Destroy(PNODE head)   
{
	PNODE p, q;  
	
    p = head->next;	           
    while(p){
	    free(p->data);  
        q = p->next;  
        free(p);  
        p = q;  
    }
	free(head); 
	head = NULL;        
	return head; 
}

/*Invert a linked list.*/
PNODE Invert(PNODE head)
{
	PNODE p, t, s;
	if(head->next == NULL || head->next->next == NULL)
	    return head;
	p = head->next;
	s = p->next;
	p->next = NULL;
	while(s->next != NULL){
		t = s->next;
		s->next = p;
		p = s;
		s = t;
	}
	s->next = p;
	head->next = s;
	return head;
}

