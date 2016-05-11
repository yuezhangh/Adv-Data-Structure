#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef int object_t;
typedef int key_t;

typedef struct tr_n_t 
{
	key_t        	key;
	struct tr_n_t  *left;
	struct tr_n_t 	*right;
	int           	height;
	struct tr_n_t  *parent;
}tree_node_t;

typedef struct order_t 
{
	tree_node_t *S_Tree;
	tree_node_t *O_Tree;
}o_t;

tree_node_t *currentblock = NULL;
int    size_left;
tree_node_t *free_list = NULL;

tree_node_t *get_node()
{
	tree_node_t *tmp;
	if( free_list != NULL )
  	{  
		tmp = free_list;
		free_list = free_list -> left;
	}
	else
	{  
		if( currentblock == NULL || size_left == 0)
		{  
			currentblock = (tree_node_t *) malloc( BLOCKSIZE * sizeof(tree_node_t) );
			size_left = BLOCKSIZE;
		}
		tmp = currentblock++;
		size_left -= 1;
	}
	return ( tmp );
}

void return_node(tree_node_t *node)
{  
	node->left = free_list;
	free_list = node;
}

void left_rotation(tree_node_t *n)
{
	tree_node_t *tmp_node;
	key_t        tmp_key;
   
	tmp_node = n->left; 
	tmp_key  = n->key;
	n->left  = n->right;        
	n->key   = n->right->key;
	n->right = n->left->right;  
	n->left->right = n->left->left;
	n->left->left  = tmp_node;
	n->left->key   = tmp_key;
}

void right_rotation(tree_node_t *n)
{  
	tree_node_t *tmp_node;
	key_t        tmp_key;
 
	tmp_node = n->right; 
	tmp_key  = n->key;
	n->right = n->left;        
	n->key   = n->left->key;
	n->left  = n->right->left;  
	n->right->left = n->right->right;
	n->right->right  = tmp_node;
	n->right->key   = tmp_key;
}

void left_rotation_O(tree_node_t *n)
{ 
	tree_node_t *tmp_node;  
	
	tmp_node = n->left;  
	n->left = n->right;	          
	n->right  = n->left->right;	  
	n->right->parent = n;
	n->left->right = n->left->left;
	n->left->left  = tmp_node;	
	n->left->left->parent = n->left;
}

void right_rotation_O(tree_node_t *n)
{
	tree_node_t *tmp_node;
   
	tmp_node = n->right; 
	n->right = n->left;	
	n->left  = n->right->left;	  
	n->left->parent = n; 
	n->right->left = n->right->right; 
	n->right->right  = tmp_node;	
	n->right->right->parent = n->right;
}

tree_node_t *create_tree(void)
{  
	tree_node_t *tmp_node;
	
	tmp_node = get_node();
	tmp_node->left = NULL;
   
   return (tmp_node);
}

int insert_of_ST(tree_node_t *tree, key_t new_key, tree_node_t *new_object)
{
	tree_node_t *tmp_node;
	int finished;
   
	if(tree->left == NULL)
	{
		tree->left = new_object;
		tree->key = new_key;
		tree->height = 0;
		tree->right = NULL; 
	}
	else
	{
		tree_node_t * path_stack[100]; 
		int  path_st_p = 0;
		tmp_node = tree; 
		while( tmp_node->right != NULL )
		{
			path_stack[path_st_p++] = tmp_node;
				if( new_key < tmp_node->key )
				tmp_node = tmp_node->left;
				else
				tmp_node = tmp_node->right;
		}
		if( tmp_node->key == new_key )
			return (-1);
		{  
			tree_node_t *old_leaf, *new_leaf;
			old_leaf = get_node();
			old_leaf->left = tmp_node->left; 
			old_leaf->key = tmp_node->key;
			old_leaf->right  = NULL;
			old_leaf->height = 0;
         
			new_leaf = get_node();
			new_leaf->left = new_object; 
			new_leaf->key = new_key;
			new_leaf->right  = NULL;
			new_leaf->height = 0; 
         
			if( tmp_node->key < new_key )
			{   
				tmp_node->left  = old_leaf;
				tmp_node->right = new_leaf;
				tmp_node->key = new_key;
			} 
			else
			{   
				tmp_node->left  = new_leaf;
				tmp_node->right = old_leaf;
			}
			tmp_node->height = 1;
		}
      
		// rebalance
		finished = 0;
		while( path_st_p > 0 && !finished )
		{  
			int tmp_height, old_height;
			tmp_node = path_stack[--path_st_p];
			old_height = tmp_node->height;
			if( tmp_node->left->height - tmp_node->right->height == 2 )
			{  
				if( tmp_node->left->left->height - tmp_node->right->height == 1 )
				{  
					right_rotation( tmp_node );
				tmp_node->right->height = tmp_node->right->left->height + 1;
				tmp_node->height = tmp_node->right->height + 1;
				}
				else
				{  
					left_rotation( tmp_node->left );
					right_rotation( tmp_node );
					tmp_height = tmp_node->left->left->height; 
					tmp_node->left->height  = tmp_height + 1; 
					tmp_node->right->height = tmp_height + 1; 
					tmp_node->height = tmp_height + 2; 
				}
			}
			else if ( tmp_node->left->height - tmp_node->right->height == -2 )
			{  
				if( tmp_node->right->right->height - tmp_node->left->height == 1 )
				{  
					left_rotation( tmp_node );
					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
				}
				else
				{  
					right_rotation( tmp_node->right );
					left_rotation( tmp_node );
					tmp_height = tmp_node->right->right->height; 
					tmp_node->left->height  = tmp_height + 1; 
					tmp_node->right->height = tmp_height + 1; 
					tmp_node->height = tmp_height + 2; 
				}
			}
			else // update height even if there was no rotation 
			{  
				if( tmp_node->left->height > tmp_node->right->height )
					tmp_node->height = tmp_node->left->height + 1;
				else
					tmp_node->height = tmp_node->right->height + 1;
			}
			if( tmp_node->height == old_height )
				finished = 1;
		}     
   }  
   return 0;
}

int insert_of_OT(tree_node_t *tree, int comm, tree_node_t *old_object, 
                 tree_node_t *new_object)
{
	tree_node_t *tmp_node;
	int finished;
   
	if(tree->left == NULL)	
	{
		tree->left = new_object;
		tree->right = NULL;
		tree->key = -23;
		tree->height = 0;      
		tree->parent = NULL;
		new_object->parent = tree;
	}
	else
	{
		tree_node_t * path_stack[100]; 
		int path_st_p = 0;
		tree_node_t * path_stack_up[100]; 
		int path_st_p_up = 0;
		int i;     
      
		if(comm == 100)
		{
			tmp_node = tree;
			while(tmp_node->right != NULL)
			{
				path_stack[path_st_p++] = tmp_node;
				tmp_node = tmp_node->right;
			}
		}
		else if(comm == -100)	
		{
			tmp_node = tree;
			while(tmp_node->right != NULL)
			{
				path_stack[path_st_p++] = tmp_node;
				tmp_node = tmp_node->left;
			}
		}
		else
		{
			tmp_node = old_object->parent;
			while(tmp_node->parent != NULL)
			{
				tmp_node = tmp_node->parent;	      	
				path_stack_up[path_st_p_up++] = tmp_node;
			}
			path_st_p = path_st_p_up;
			for(i=0;i<path_st_p;i++)
				path_stack[i] = path_stack_up[--path_st_p_up];
		}  	
      	tree_node_t *old_leaf, *new_leaf;

		if(1 == comm || -1 == comm)	
			tmp_node = old_object->parent;
				
		old_leaf = get_node();        	 
     	old_leaf->key = -23;
     	old_leaf->right  = NULL;
      	old_leaf->height = 0; 
         
    	new_leaf = get_node();
        new_leaf->left = new_object; 
        new_leaf->key = -23;	
        new_leaf->right  = NULL;
        new_leaf->height = 0;                     
     		
     	old_leaf->left = tmp_node->left;
   		old_leaf->left->parent = old_leaf;	
       	old_leaf->parent = tmp_node; 
       	new_leaf->parent = tmp_node;
		new_object->parent = new_leaf;
	                
        if(100 == comm || 1 == comm)	
        {
			tmp_node->left = old_leaf;
          	tmp_node->right = new_leaf;
        }      	
        else	
        {
			tmp_node->left = new_leaf;
          	tmp_node->right = old_leaf;
		} 
		tmp_node->height = 1;
      
      // rebalance
      finished = 0;
      while( path_st_p > 0 && !finished )
      {  
      	int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         old_height = tmp_node->height;
         if( tmp_node->left->height - tmp_node->right->height == 2 )
         {  
         	if( tmp_node->left->left->height - tmp_node->right->height == 1 )
            {  
            	right_rotation_O( tmp_node );
               tmp_node->right->height = tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  
            	left_rotation_O( tmp_node->left );
               right_rotation_O( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - tmp_node->right->height == -2 )
         {  
         	if( tmp_node->right->right->height - tmp_node->left->height == 1 )
            {  
            	left_rotation_O( tmp_node );
               tmp_node->left->height = tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  
            	right_rotation_O( tmp_node->right );
               left_rotation_O( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else // update height even if there was no rotation 
         {  
         	if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished = 1;
      }     
   }  
   return (0);
}

tree_node_t *delete_of_ST(tree_node_t *tree, key_t delete_key)
{  
	tree_node_t *tmp_node, *upper_node, *other_node;   
	tree_node_t *deleted_object;	
	int finished;
	
	if( tree->left == NULL )
		return( NULL );
	else if( tree->right == NULL )
	{
		if(  tree->key == delete_key )
		{  
			deleted_object = tree->left;
			tree->left = NULL;
			return (deleted_object);
		}
		else
			return (NULL);
	}
	else
	{  tree_node_t * path_stack[100]; int path_st_p = 0;
      tmp_node = tree;
      while( tmp_node->right != NULL )
      {   path_stack[path_st_p++] = tmp_node;  
          upper_node = tmp_node;
          if( delete_key < tmp_node->key )
          {  tmp_node   = upper_node->left; 
             other_node = upper_node->right;
          } 
          else
          {  tmp_node   = upper_node->right; 
             other_node = upper_node->left;
          } 
      }
      if( tmp_node->key != delete_key )
         deleted_object = NULL;
      else
      {  upper_node->key   = other_node->key;
         upper_node->left  = other_node->left;
         upper_node->right = other_node->right;
         upper_node->height = other_node->height;
         
         deleted_object = tmp_node->left;
         
         return_node( tmp_node );
         return_node( other_node );

      }
      /*start rebalance*/  
      finished = 0; path_st_p -= 1;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
	    {  right_rotation( tmp_node ); 
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
	    {  left_rotation( tmp_node->left ); 
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
	    {  left_rotation( tmp_node ); 
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
	    finished = 1;
      }
      /*end rebalance*/
      return (deleted_object);
   }
}

int will = 0;
int delete_of_OT(tree_node_t *tree, tree_node_t *delete_object)
{
	tree_node_t *tmp_node, *upper_node, *other_node;   
	int finished;
   
	if(tree->left == NULL )
		return -1;
	else if(delete_object == NULL)
		return -1;  	
	else if(tree->right == NULL)	
	{  
		if(tree == delete_object->parent)
		{      	
			tree->left = NULL;
			return 0;
		}
		else
			return -1;
	}
	else
	{
		tree_node_t *path_stack[100]; 
		int path_st_p = 0;
		tree_node_t *path_stack_up[100]; 
		int path_st_p_up = 0;
		int i;
      
		tmp_node = delete_object->parent;
		while(tmp_node->parent != NULL)
		{
			tmp_node = tmp_node->parent;	      	
			path_stack_up[path_st_p_up++] = tmp_node;
		}
		path_st_p = path_st_p_up;
		for(i=0;i<path_st_p;i++)
			path_stack[i] = path_stack_up[--path_st_p_up];		
		
		upper_node = path_stack[path_st_p-1];
		tmp_node = delete_object->parent;
		if(upper_node->left == tmp_node)
	    	other_node = upper_node->right;	
		else
			other_node = upper_node->left;

				
      {
      	upper_node->key = other_node->key;
         upper_node->left = other_node->left;
         upper_node->right = other_node->right;
         upper_node->height = other_node->height;
         
         if(upper_node->left != NULL)
         	upper_node->left->parent = upper_node;
         if(upper_node->right != NULL)
         	upper_node->right->parent = upper_node;
         
         return_node(tmp_node);
         return_node(other_node);

      }      
      
      //start rebalance  
      finished = 0; path_st_p -= 1;
      while( path_st_p > 0 && !finished )
      {  
      	int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         old_height = tmp_node->height;
         if( tmp_node->left->height - tmp_node->right->height == 2 )
         {  
         	if( tmp_node->left->left->height - tmp_node->right->height == 1 )
	    		{  
	    			right_rotation_O( tmp_node ); 
               tmp_node->right->height = tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
	    		{  
	    			left_rotation_O( tmp_node->left ); 
               right_rotation_O( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - tmp_node->right->height == -2 )
         {  
         	if( tmp_node->right->right->height - tmp_node->left->height == 1 )
	    		{  
	    			left_rotation_O( tmp_node ); 
               tmp_node->left->height = tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  
            	right_rotation_O( tmp_node->right );
               left_rotation_O( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else // update height even if there was no rotation 
         {  
         	if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
	    		finished = 1;
      }
      return 0;
   }
   return 0;
}

tree_node_t *findNode_search(tree_node_t *tree, int comm, key_t query_key)
{
	tree_node_t *tmp_node;
	
	if(tree->left == NULL)
		return NULL;
	else
	{
		tmp_node = tree;
		if(0 == comm) 
		{   		
			while(tmp_node->right != NULL)
			{   
				if(query_key < tmp_node->key )
					tmp_node = tmp_node->left;
				else
					tmp_node = tmp_node->right;
			}
			if(tmp_node->key == query_key)
				return tmp_node->left;
			else
				return NULL;
		}
		else if(100 == comm)
		{
			while(tmp_node->right != NULL)
				tmp_node = tmp_node->right;
			return tmp_node->left;
		}
		else if(-100 == comm)
		{
			while(tmp_node->right != NULL)
				tmp_node = tmp_node->left;
			return tmp_node->left;
		}
   }
   return NULL;
}

o_t* create_order(void)
{
	o_t *tree = (o_t *)malloc(sizeof(o_t));	  
   
	tree->S_Tree = create_tree();
	tree->O_Tree = create_tree();
	
	return tree;	
}

void insert_top(o_t *ord, key_t a)
{
	int success;
	
	if(ord == NULL)
	{
		return;
	}
					
	tree_node_t *newNode;
	newNode = get_node();
	newNode->key = a;
	newNode->height = 0;
		
	success = insert_of_ST(ord->S_Tree, a, newNode);
	if(-1 == success)
	{
		return_node(newNode);
		return;
	}
	insert_of_OT(ord->O_Tree, 100, NULL, newNode);			
}

void insert_bottom(o_t *ord, key_t a)
{
	int success;
	
	if(ord == NULL)
	{
		return;
	}
					
	tree_node_t *newNode;
	newNode = get_node();
	newNode->key = a;
	newNode->height = 0;
		
	success = insert_of_ST(ord->S_Tree, a, newNode);
	if(-1 == success)
	{
		return_node(newNode);
		return;
	}
	insert_of_OT(ord->O_Tree, -100, NULL, newNode);			
}

void insert_after(o_t *ord, key_t a, key_t b)
{
	int success;
	
	if(ord == NULL)
	{
		return;
	}

	tree_node_t *oldNode;
	oldNode = findNode_search(ord->S_Tree, 0, b);	
	if(oldNode == NULL)
	{
		return;
	}
					
	tree_node_t *newNode;
	newNode = get_node();
	newNode->key = a;
	newNode->height = 0;
	
	success = insert_of_ST(ord->S_Tree, a, newNode);
	if(-1 == success)
	{
		return_node(newNode);
		return;
	}

	insert_of_OT(ord->O_Tree, 1, oldNode, newNode);
}

void insert_before(o_t *ord, key_t a, key_t b)
{
	int success;
	
	if(ord == NULL)
	{
		return;
	}

	tree_node_t *oldNode;
	oldNode = findNode_search(ord->S_Tree, 0, b);	
	if(oldNode == NULL)
	{
		return;
	}
						
	tree_node_t *newNode;
	newNode = get_node();
	newNode->key = a;
	newNode->height = 0;
	
	success = insert_of_ST(ord->S_Tree, a, newNode);
	if(-1 == success)
	{
		return_node(newNode);
		return;
	}

	insert_of_OT(ord->O_Tree, -1, oldNode, newNode);
}

void delete_o(o_t *ord, key_t a)
{  
	int success;
		
	if(ord == NULL)
	{
		return;
	}
	
	tree_node_t *del;
	del = delete_of_ST(ord->S_Tree, a);
	if(del == NULL)
	{
		return;
	}

	success = delete_of_OT(ord->O_Tree, del);
}

int is_before(o_t *ord, key_t a, key_t b)
{
	int rslt = -1;
	
	tree_node_t *a_Node;
	a_Node = findNode_search(ord->S_Tree, 0, a);	
	if(a_Node == NULL)
	{
		return -1;
	}
		
	tree_node_t *b_Node;
	b_Node = findNode_search(ord->S_Tree, 0, b);	
	if(b_Node == NULL)
	{
		return -1;
	}
	
	a_Node = a_Node->parent;
	b_Node = b_Node->parent;

	tree_node_t *tmp_node_a = a_Node;
	tree_node_t *tmp_node_b = b_Node;
	int height_a = 0;
	int height_b = 0;
    
	while(tmp_node_a != NULL) 
	{
      height_a++;
      tmp_node_a = tmp_node_a->parent;
	}
	while(tmp_node_b != NULL) 
	{
		height_b++;
      tmp_node_b = tmp_node_b->parent;
    }

	if(height_a != height_b) 
	{
		if (height_a > height_b) 
		{
     		while (height_a != height_b) 
     		{
				a_Node = a_Node->parent;
				height_a--;
			}
		}
		else
		{
     		while (height_b != height_a) 
     		{
				b_Node = b_Node->parent;
				height_b--;
			}
		}
	}
	
	while (a_Node->parent != b_Node->parent) 
	{
		a_Node = a_Node->parent;
		b_Node = b_Node->parent;
	}
	
	if(a_Node->parent->left == a_Node)
		rslt = 1;
	else
    	rslt = 0;

	return rslt;
}
	
long p(long q)
{ return( (1247*q +104729) % 300007 );
}

int main()
{
 long i; o_t *o; 
   printf("starting \n");
   o = create_order();
   for(i=100000; i>=0; i-- )
      insert_bottom( o, p(i) );
   for(i=100001; i< 300007; i+=2 )
   {  insert_after(o, p(i+1), p(i-1) );
      insert_before( o, p(i), p(i+1) );
   }
   printf("inserted 300000 elements. ");
   for(i = 250000; i < 300007; i++ )
      delete_o( o, p(i) );
   printf("deleted 50000 elements. ");
   insert_top( o, p(300006) );
   for(i = 250000; i < 300006; i++ )
      insert_before( o, p(i) , p(300006) );
   printf("reinserted. now testing order\n");
   for( i=0; i < 299000; i +=42 )
   {  if( is_before( o, p(i), p(i+23) ) != 1 )
      {  printf(" found error (1) \n"); exit(0);
      }
   }
   for( i=300006; i >57; i -=119 )
   {  if( is_before( o, p(i), p(i-57) ) != 0 )
      {  printf(" found error (0) \n"); exit(0);
      }
   }
   printf("finished. no problem found.\n"); 
} 
