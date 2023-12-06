#ifndef CSTACK_H
#define CSTACK_H


#define STACK_HANLDER_MAX 10 //максимальное количество хэндлеров
#define STACK_NODE_MAX_SIZE 1073741824 //не дадим записать за раз больше гигабайта (и то наверное дофига)
#define STACK_NODE_MAX_COUNT 800 //максимальное число узлов у стека

typedef int hstack_t;

struct node
{
    struct node * prev;
    char * data;
    unsigned int size;
};

struct _g_table
{
	struct node * point;
    unsigned int size;
};




hstack_t stack_new(void);
void stack_free(const hstack_t stack);
int stack_valid_handler(const hstack_t stack);
unsigned int stack_size(const hstack_t stack);
void stack_push(const hstack_t stack, const void* data_in, const unsigned int size);
unsigned int stack_pop(const hstack_t stack, void* data_out, const unsigned int size);

#endif /* CSTACK_H */
