#include<stdio.h> 
#include<stdlib.h> 
  
/*I used the code for a linked list from: http://www.zentut.com/c-tutorial/c-linked-list/
  And I changed the struct to add the arrival time for each person in line.*/

typedef struct node
{
    int arrivalTime;
    struct node* next;
} node;
 
typedef void (*callback)(node* arrivalTime);
 
node* create(int arrivalTime,node* next)
{
    node* new_node = (node*)malloc(sizeof(node));

    if(new_node == NULL)
    {
        printf("Error creating a new node.\n");
        exit(0);
    }

    new_node->arrivalTime = arrivalTime;
    new_node->next = next;
 
    return new_node;
}
 
node* append(node* head, int arrivalTime)
{
    if(head == NULL)
    {
        return NULL;
    }

    node *cursor = head;
    while(cursor->next != NULL)
    {
        cursor = cursor->next;
    }
 
    node* new_node =  create(arrivalTime,NULL);
    cursor->next = new_node;
 
    return head;
}

node* remove_front(node* head)
{
    if(head == NULL)
    {
        return NULL;
    }

    node *front = head;
    head = head->next;
    front->next = NULL;
    
    if(front == head)
    {
        head = NULL;
    }

    free(front);
    return head;
}

node* search(node* head,int data)
{
 
    node *cursor = head;

    while(cursor!=NULL)
    {
        if(cursor->arrivalTime == data)
        {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

int count(node *head)
{
    node *cursor = head;
    int c = 0;

    while(cursor != NULL)
    {
        c++;
        cursor = cursor->next;
    }
    return c;
}