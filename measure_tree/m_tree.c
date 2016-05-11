#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

#define DEBUG 1
#define BLOCKSIZE 256
#define MAX(X,Y) X>Y?X:Y
#define MIN(X,Y) X<Y?X:Y

typedef int key_t;
typedef int height_t;

typedef struct list_node 
{ 
	int interval_a, interval_b;
	struct list_node *next;
} list_node_t;

typedef struct node_text
{
	key_t key;
	int interval_a;
	int interval_b;
	int measure;
	int leftmin;
	int rightmax;
	height_t height;
	list_node_t *interval_list;
	struct node_text *left;
	struct node_text *right;
	struct node_text *parent;
} m_tree_t;

m_tree_t *currentblock = NULL;
int size_left;
m_tree_t *free_list = NULL;

int nodes_taken = 0;
int nodes_returned = 0;

m_tree_t *get_node()
{ 
	m_tree_t *tmp;
  	nodes_taken += 1;
  	if (free_list != NULL)
  	{  
		tmp = free_list;
     		free_list = free_list -> right;
  	}
  	else
  	{  
		if (currentblock == NULL || size_left == 0)
     		{	  
			currentblock = (m_tree_t *) malloc(BLOCKSIZE * sizeof(m_tree_t));
        		size_left = BLOCKSIZE;
     		}
     		tmp = currentblock++;
     		size_left -= 1;
  	}
  	return (tmp);
}

void return_node(m_tree_t *node)
{  
	node->right = free_list;
   	free_list = node;
   	nodes_returned +=1;
}

m_tree_t *create_node(void)
{  
	m_tree_t *tmp_node;
   	tmp_node = get_node();
   	tmp_node->left = NULL;
	tmp_node->right = NULL;
	tmp_node->parent = NULL;
   	tmp_node->interval_a = -1;
	tmp_node->interval_b = -1;
	tmp_node->key = -1;
	tmp_node->height = -1;
	tmp_node->leftmin = -1;
	tmp_node->rightmax = -1;
	tmp_node->measure = -1;
	tmp_node->interval_list = NULL;
	return (tmp_node);
}

list_node_t *create_list_node(int a, int b)
{
	list_node_t *lnode;
	lnode = (list_node_t *)malloc(sizeof(list_node_t));
	lnode->interval_a = a;
	lnode->interval_b = b;
	lnode->next = NULL;
	return (lnode);
}

m_tree_t *create_m_tree()
{	
	m_tree_t *tmp_node;
	tmp_node = create_node();
	tmp_node->key = INT_MIN;   
    	tmp_node->interval_a = INT_MIN;
    	tmp_node->interval_b = INT_MAX;
    	tmp_node->leftmin = INT_MIN;
    	tmp_node->rightmax = INT_MAX;
    	tmp_node->height = 0;
	return (tmp_node);
}

void left_rotation(m_tree_t *tree)
{ 
	m_tree_t *tmp_node;
	key_t tmp_key;
	tmp_node = tree->left;
	tmp_key = tree->key;
	tree->left = tree->right;
	tree->key = tree->right->key;
	tree->right = tree->left->right;	
	tree->right->parent = tree; 
	tree->left->right = tree->left->left;
	tree->left->left = tmp_node;
	tree->left->key = tmp_key;
	tree->left->interval_a = tree->interval_a;
 	tree->left->interval_b = tree->key;
    	tmp_node->parent = tree->left; 
}

void right_rotation(m_tree_t *tree)
{ 
	m_tree_t *tmp_node;
	key_t tmp_key;
	tmp_node = tree->right;
	tmp_key = tree->key;
	tree->right = tree->left;
	tree->key = tree->left->key;
	tree->left = tree->right->left;
	tree->left->parent = tree; 
	tree->right->left = tree->right->right;
	tree->right->right = tmp_node;
	tree->right->key = tmp_key;
	tree->right->interval_a = tree->key;
 	tree->right->interval_b = tree->interval_b;
    	tmp_node->parent = tree->right; 
}

void insert_BST(m_tree_t *tree, int insert_key)
{
	m_tree_t *tmp_node = tree;
	int finished;
	if (tmp_node == NULL)
        {
                return;
        }

	while(tmp_node->right != NULL)
        {
        	if (insert_key < tmp_node->key)
		{
                	tmp_node = tmp_node->left;
                }
                else
                {
                	tmp_node = tmp_node->right;
                }
        }

 	if (tmp_node->key == insert_key)
        	return;
        else
        {
		m_tree_t  *left_leaf, *right_leaf;
                left_leaf = create_node();
                right_leaf = create_node();

                left_leaf->parent = tmp_node;
                left_leaf->height = 0;

                right_leaf->parent = tmp_node;
                right_leaf->height = 0;
		tmp_node->height = 0;

                if (tmp_node->key < insert_key)
                {
                        left_leaf->key = tmp_node->key;
                        right_leaf->key = insert_key;
                        tmp_node->key = insert_key;
			left_leaf->interval_list = tmp_node->interval_list;
                 }
                 else
                 {
			left_leaf->key = insert_key;
                        right_leaf->key = tmp_node->key;
			right_leaf->interval_list = tmp_node->interval_list;
		 }

		 /* Interval updation for all changed nodes */           
                 left_leaf->interval_a = tmp_node->interval_a;
                 left_leaf->interval_b = tmp_node->key;
                 right_leaf->interval_a = tmp_node->key;
                 right_leaf->interval_b = tmp_node->interval_b;

		 tmp_node->interval_list = NULL;
                 tmp_node->left = left_leaf;
                 tmp_node->right = right_leaf;
         }

	finished = 0;
	while(!finished && tmp_node != NULL)
	{
		int tmp_height, old_height;
		old_height = tmp_node->height;
		if (tmp_node->left->height - tmp_node->right->height == 2)
		{ 
			if (tmp_node->left->left->height - tmp_node->right->height == 1)
			{ 
				right_rotation(tmp_node);
				tmp_node->right->height = tmp_node->right->left->height + 1;
				tmp_node->height = tmp_node->right->height + 1;
			}
			else
			{ 
				left_rotation(tmp_node->left);
				right_rotation(tmp_node);
				tmp_height = tmp_node->left->left->height;
				tmp_node->left->height = tmp_height + 1;
				tmp_node->right->height = tmp_height + 1;
				tmp_node->height = tmp_height + 2;
			}
		}
		else if (tmp_node->left->height - tmp_node->right->height == -2)
		{ 
			if( tmp_node->right->right->height - tmp_node->left->height == 1)
			{ 
				left_rotation(tmp_node);
				tmp_node->left->height = tmp_node->left->right->height + 1;
				tmp_node->height = tmp_node->left->height + 1;
			}
			else
			{ 
				right_rotation(tmp_node->right);
				left_rotation(tmp_node);
				tmp_height = tmp_node->right->right->height;
				tmp_node->left->height = tmp_height + 1;
				tmp_node->right->height = tmp_height + 1;
				tmp_node->height = tmp_height + 2;
			}
		}
      		else 
		{ 
			if(tmp_node->left->height > tmp_node->right->height)
			{
				tmp_node->height = tmp_node->left->height + 1;
			}
			else
			{
				tmp_node->height = tmp_node->right->height + 1;
			}
		}
		if (tmp_node->height == old_height)
		{
			finished = 1;
		}
		tmp_node = tmp_node->parent;
	}
}

void insert_assoc_interval(m_tree_t *tree, int interval_a, int interval_b)
{
        list_node_t *new_lnode;
	if (tree == NULL)
        {
                return;
        }
	else
	{
		if( (interval_a >= tree->interval_a && interval_a < tree->interval_b) || 
				(interval_b-1 >= tree->interval_a && interval_b-1 < tree->interval_b) || 
					(tree->interval_a >= interval_a && tree->interval_b <= interval_b) )
		{
			
			if (tree->right == NULL)
			{
				new_lnode = create_list_node(interval_a, interval_b);
				if (tree->interval_list == NULL)
				{
					tree->interval_list = new_lnode;			
				}
				else
				{
					new_lnode -> next = tree->interval_list;
					tree->interval_list = new_lnode;
				}
			}
			insert_assoc_interval(tree->left, interval_a, interval_b);
			insert_assoc_interval(tree->right, interval_a, interval_b);
		}
		else
		{	
			return;
		}
	}
}

void update_measure(m_tree_t *tree)
{
	list_node_t *temp;
	int leftmin_temp, rightmax_temp;
	if (tree == NULL)
		return;
	else
	{
		update_measure(tree->left);
                update_measure(tree->right);
		
		if (tree->right == NULL)
		{
			if (tree->interval_list == NULL)
			{
				tree->measure = 0;
				tree->leftmin = tree->interval_a;
				tree->rightmax = tree->interval_b;
			}
		
			else
			{
				temp = tree->interval_list;
				leftmin_temp = tree->interval_list->interval_a;
				rightmax_temp = tree->interval_list->interval_b;
				tree->measure = tree->interval_b - tree->interval_a;
				while (temp != NULL)
				{
					leftmin_temp = MIN(leftmin_temp, temp->interval_a);
					rightmax_temp = MAX(rightmax_temp, temp->interval_b);
					temp = temp->next;	
				}
				tree->leftmin = leftmin_temp;
				tree->rightmax = rightmax_temp;
			}
		}
		else
		{

			tree->rightmax = MAX(tree->left->rightmax, tree->right->rightmax);
			tree->leftmin = MIN(tree->left->leftmin, tree->right->leftmin);

			if ((tree->right->leftmin < tree->interval_a) &&  (tree->left->rightmax >= tree->interval_b))
			{
				tree->measure = tree->interval_b - tree->interval_a;
			}
			else if ((tree->right->leftmin >= tree->interval_a) && (tree->left->rightmax >= tree->interval_b))
			{
				tree->measure = (tree->interval_b - tree->key) + tree->left->measure;
			}
			else if ((tree->right->leftmin < tree->interval_a) && (tree->left->rightmax < tree->interval_b))
			{
				tree->measure = tree->right->measure + (tree->key - tree->interval_a);
			}
			else if ((tree->right->leftmin >= tree->interval_a) && (tree->left->rightmax < tree->interval_b))
			{
				tree->measure = tree->right->measure + tree->left->measure;
			}
			else
			{

			}
		}
	}
}

int query_length(m_tree_t * tree)
{
        if (tree == NULL)
                return 0;
        else
        {
                update_measure(tree);
		if (tree->measure == 0)
		{
                	tree = create_m_tree();
			return 0;
		}
                return (tree->measure);
        }
}

void insert_interval(m_tree_t *tree, int a, int b) 
{
	if (tree == NULL)
	{
		return;
	}
	insert_BST(tree, a);
	insert_BST(tree, b);
	insert_assoc_interval(tree, a, b);
}

list_node_t *delete_list_node(list_node_t *head, int a, int b)
{
	list_node_t *current, *prev;
	current = head;
	prev = NULL;
	if ((current->interval_a == a) && (current->interval_b == b))
	{
		head = head->next;
	}
	else
	{
		for (current = head; current != NULL; prev = current, current = current->next)
		{
			if ((current->interval_a == a) && (current->interval_b == b))
			{
				prev->next = current->next;
			}
		}
	}
	free(current);
	return head;
}

void delete_interval(m_tree_t *tree, int interval_a, int interval_b)
{

        list_node_t *temp;
        if (tree == NULL)
        {
                return;
        }
        else
        {
		if( (interval_a >= tree->interval_a && interval_a < tree->interval_b) || 
				(interval_b-1 >= tree->interval_a && interval_b-1 < tree->interval_b) || 
					(tree->interval_a >= interval_a && tree->interval_b <= interval_b) )
		{

                        if (tree->right == NULL && tree->interval_list != NULL)
                        {
                                tree->interval_list = delete_list_node(tree->interval_list, interval_a, interval_b);
                        }
                        delete_interval(tree->left, interval_a, interval_b);
                        delete_interval(tree->right, interval_a, interval_b);
                }
                else
                {
                        return;
                }
        }
}

int main()
{
	int i; m_tree_t *t;
	printf("starting \n");
	t = create_m_tree();
	for(i=0; i< 50; i++ )
		insert_interval( t, 2*i, 2*i +1 );
	printf("inserted first 50 intervals, total length is %d, should be 50.\n", query_length(t));
		insert_interval( t, 0, 100 );
	printf("inserted another interval, total length is %d, should be 100.\n", query_length(t));
	for(i=1; i< 50; i++ )
		insert_interval( t, 199 - (3*i), 200 );
	printf("inserted further 49 intervals, total length is %d, should be 200.\n", query_length(t));
	for(i=2; i< 50; i++ )
		delete_interval(t, 2*i, 2*i +1 );
	delete_interval(t,0,100);
	printf("deleted some intervals, total length is %d, should be 150.\n", query_length(t));
	insert_interval( t, 1,2 ); 
	for(i=49; i>0; i-- )
		delete_interval( t, 199 - (3*i), 200 ); 
	insert_interval( t, 0,2 );
	insert_interval( t, 1,5 );  
	printf("deleted some intervals, total length is %d, should be 5.\n", query_length(t));
	insert_interval( t, 0, 100 );
	printf("inserted another interval, total length is %d, should be 100.\n", query_length(t));
	for(i=0; i<=300; i++ )
		insert_interval( t, 200+i, 300+i ); 
	printf("inserted 300 intervals, total length is %d, should be 500.\n", query_length(t));
	for(i=0; i<=300; i++ )
		delete_interval( t, 200+i, 300+i ); 
	printf("deleted 300 intervals, total length is %d, should be 100.\n", query_length(t));
	for(i=0; i<=100; i++ )
		insert_interval( t, 10*i, 10*i+100 ); 
	printf("inserted another 100 intervals, total length is %d, should be 1100.\n", query_length(t));
	delete_interval( t, 1,2 ); 
	delete_interval( t, 0,2 ); 
	delete_interval( t, 2,3 ); 
	delete_interval( t, 0,1 ); 
	delete_interval( t, 1,5 );
	printf("deleted some intervals, total length is %d, should be still 1100.\n", query_length(t)); 
	for(i=0; i<= 100; i++ )
		delete_interval(t, 10*i, 10*i+100);
	delete_interval(t,0,100);
	printf("deleted last interval, total length is %d, should be 0.\n", query_length(t));
	for( i=0; i<100000; i++)
	{
		insert_interval(t, i, 1000000);
	}
	printf("inserted 100000 intervals, total length is %d, should be 1000000.\n", query_length(t));
	printf("End Test\n");
}
