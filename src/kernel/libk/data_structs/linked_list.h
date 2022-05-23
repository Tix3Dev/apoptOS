#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <stddef.h>

typedef struct slist
{
	struct slist *next;
} slist_t;

// Taken from linux
#define container_of(ptr, type, member) ({                         \
	const typeof(((type *)0)->member) *__mptr = (ptr);             \
	type *__t = (type *)((char *)__mptr - offsetof(type, member)); \
	__mptr == NULL ? NULL : __t;                                   \
})

// Retrieve the next element in the list
#define list_next(input, member) ({                                  \
	typeof(input) *_next_ = (input)->member.next;                    \
	!_next_ ? NULL : container_of(_next_, typeof(*(input)), member); \
})

// Iterate over a singly or doubly linked list.
// Parameters:
// [] out:    The input->next data will be written to 'out'.
// [] member: Name of the list member in the struct 'input'.
// [] input:  An instance of a struct.
#define list_foreach(out, member, input) \
	for (typeof(input) out = input; out != NULL; out = list_next(out, member))

#define list_append(node, member, input) ({           \
	__list_append(&(input)->member, &(node)->member); \
})

static inline void __list_append(slist_t *head, slist_t *node)
{
	slist_t *prev = head;
	while (head)
		prev = head, head = head->next;

	prev->next = node;
}

#define list_set_next(node, member, input) ({                              \
	typeof(node) node_to_append = node;                                    \
	slist_t *head = &(input)->member;                                 \
	slist_t *next = !node_to_append ? NULL : &node_to_append->member; \
	head->next = next;                                                     \
})

#endif
