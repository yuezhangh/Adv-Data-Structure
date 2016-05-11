#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256
#define MAX 2000003

typedef char object_t;

typedef struct l_node {  
    char *key;
    object_t *obj;
    struct l_node  *next; 
} list_node_t;

typedef struct htp_l_node { 
	int a; 
    struct htp_l_node *next; 
} htp_l_node_t; 

typedef struct hf_p_t{ 
	int b;   
	int size; 
	struct htp_l_node *a_list;
} hf_param_t;

typedef struct bl_filter{ 
	char tables[8][250000];
    int (*hashfunction)(char *, hf_param_t); 
    hf_param_t hf_param[8]; 
} bf_t;

char mask[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };

list_node_t *currentblock = NULL;
int size_left;
list_node_t *free_list = NULL;

list_node_t *get_node()
{ list_node_t *tmp;
    if( free_list != NULL )
    {  tmp = free_list;
        free_list = free_list -> next;
    }
    else
    {  if( currentblock == NULL || size_left == 0)
        {  currentblock = 
            (list_node_t *) malloc( BLOCKSIZE * sizeof(list_node_t) );
            size_left = BLOCKSIZE;
        }
        tmp = currentblock++;
        size_left -= 1;
    }
    return( tmp );
}

void return_node(list_node_t *node)
{  node->next = free_list;
    free_list = node;
}

int uni_hash_f(char *key, hf_param_t hfp)
{  int sum;
    htp_l_node_t *list;
    sum = hfp.b;
    list = hfp.a_list;
    while( *key != '\0' )
    {  if( list->next == NULL )
        {   list->next = (htp_l_node_t *) get_node();
            list->next->next = NULL;
            list->a = rand()%MAX;
        }
        sum += ( abs((list->a)*((int) *key)))%MAX;
        key += 1;
        list = list->next;
    }
    return( sum%hfp.size );
}

bf_t *create_bf() {
	bf_t *tmp;
	int i,j;
	tmp = (bf_t *) malloc(sizeof(bf_t));
	for(i = 0; i < 8; i++) {
		tmp->hf_param[i].b = rand()%MAX;
		tmp->hf_param[i].size = 250000*8;
		tmp->hf_param[i].a_list = (htp_l_node_t *) get_node();
		tmp->hf_param[i].a_list->next = NULL;
		for(j = 0; j < 250000; j++) {
			tmp->tables[i][j] = 0;
		}
	}
	tmp->hashfunction = uni_hash_f;
	return tmp;
}

void insert_bf(bf_t *b, char *s) {
	int i;
	for(i = 0; i < 8; i++) {
		int j = b->hashfunction(s, b->hf_param[i]);
		b->tables[i][j/8] = b->tables[i][j/8] | mask[j%8];
	}
}

int is_element(bf_t *b, char *q) {
	int i;
	for(i = 0; i < 8; i++) {
		int j = b->hashfunction(q, b->hf_param[i]);
		if((b->tables[i][j/8] & mask[j%8]) == 0) return 0;
	}
	return 1;
}

