/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

/**
  Node Data Structure
*/
typedef struct _node_t
{
	void* item;
	struct _node_t* next;
} node_t;


node_t* node_init			();

void*	insert				(node_t* n, void* item);
void*	insert_at			(node_t* n1, node_t* n2);
void*	get_item			(node_t* n);
void*	node_destroy		(node_t* n);


//=========================================//


/**
  Priqueue Data Structure
*/
typedef struct _priqueue_t
{
	int queueSize;
	node_t* head;
	int (*comp) (const void*, const void*);
} priqueue_t;


void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);


#endif /* LIBPQUEUE_H_ */
