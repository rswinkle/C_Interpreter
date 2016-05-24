#ifndef CLIST_H
#define CLIST_H

typedef struct list_head
{
	struct list_head* next;
	struct list_head* prev;
} list_head;


#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(list_head* item, list_head* prev, list_head* next)
{
	next->prev = item;
	item->next = next;
	item->prev = prev;
	prev->next = item;
}


static inline void list_add(list_head* item, list_head* head)
{
	__list_add(item, head, head->next);
}


static inline void list_add_tail(list_head* item, list_head* head)
{
	__list_add(item, head->prev, head);
}

static inline void __list_del(list_head* prev, list_head* next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void list_del(list_head* item)
{
	__list_del(item->prev, item->next);
}

static inline void list_replace(list_head* old, list_head* item)
{
	item->next = old->next;
	item->prev = old->prev;
	item->next->prev = item;
	item->prev->next = item;
}

static inline int list_is_last(const list_head *list, const list_head *head)
{
	return list->next == head;
}

static inline int list_empty(const list_head *head)
{
	return head->next == head;
}


static inline int list_is_singular(const list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ((type*)((char*)(ptr) - offsetof(type, member)))

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) \
	(!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * __list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * This variant doesn't differ from list_for_each() any more.
 * We don't do prefetching in either case.
 */
#define __list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @type:   the type of the struct this is embedded in
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, type, head, member)				\
	for (pos = list_entry((head)->next, type, member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, type, member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @type:   the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, type, head, member)			\
	for (pos = list_entry((head)->prev, type, member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, type, member))


/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @type:   the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, type, head, member) 		\
	for (pos = list_entry(pos->member.next, type, member);	\
	     &pos->member != (head);	\
	     pos = list_entry(pos->member.next, type, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @type:   the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, type, head, member)		\
	for (pos = list_entry(pos->member.prev, type, member);	\
	     &pos->member != (head);	\
	     pos = list_entry(pos->member.prev, type, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @type:   the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, type, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = list_entry(pos->member.next, type, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @type:   the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, type, head, member)			\
	for (pos = list_entry((head)->next, type, member),	\
		n = list_entry(pos->member.next, type, member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, type, member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @type:   the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, type, head, member) 		\
	for (pos = list_entry(pos->member.next, type, member), 		\
		n = list_entry(pos->member.next, type, member);		\
	     &pos->member != (head);						\
	     pos = n, n = list_entry(n->member.next, type, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @type:   the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, type, head, member) 			\
	for (n = list_entry(pos->member.next, type, member);		\
	     &pos->member != (head);						\
	     pos = n, n = list_entry(n->member.next, type, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @type:   the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, type, head, member)		\
	for (pos = list_entry((head)->prev, type, member),	\
		n = list_entry(pos->member.prev, type, member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.prev, type, member))



typedef struct int_list
{
	int data;
	list_head list;
} int_list;




void list_swap(list_head* p, list_head* q);
void sort_list(list_head* head, int cmp(list_head*, list_head*));

#endif


#ifdef CLIST_IMPLEMENTATION

void list_swap(list_head* p, list_head* q)
{
	list_head tmp;
	tmp = *p;

	p->next = q->next;
	p->prev = q->prev;
	p->next->prev = p;
	p->prev->next = p;

	q->next = tmp.next;
	q->prev = tmp.prev;
	q->next->prev = q;
	q->prev->next = q;
}


void sort_list(list_head* head, int cmp(list_head*, list_head*))
{
	list_head* p, *q;

	int swapped, ret;
	do {
		swapped = 0;

		for (p = head->next; p->next != head; ) {
			q = p->next;
			ret = cmp(p, q);
			if (ret > 0) {
				p->next = q->next;
				p->next->prev = p;
				q->prev = p->prev;
				q->prev->next = q;

				p->prev = q;
				q->next = p;

				swapped = 1;
			} else {
				p = p->next;
			}
		}
	} while (swapped);
}


#endif
