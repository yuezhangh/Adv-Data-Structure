#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256
#define NUM_MAX 100

typedef struct tr_n_t
{
	int key;
    struct tr_n_t  *left;
    struct tr_n_t *right;
    int height; 
}text_t;

text_t *currentblock = NULL;
int    size_left;
text_t *free_list = NULL;

text_t *get_node()
{
	text_t *tmp;
	if( free_list != NULL )
	{
		tmp = free_list;
		free_list = free_list -> left;
	}
	else
	{
		if( currentblock == NULL || size_left == 0)
		{
			currentblock = (text_t *) malloc( BLOCKSIZE * sizeof(text_t) );
			size_left = BLOCKSIZE;
		}
		tmp = currentblock++;
		size_left -= 1;
	}
	return( tmp );
}

void return_node(text_t *node)
{
	node->left = free_list;
	free_list = node;
}


text_t *create_text(void)
{
	text_t *tmp_node;
	tmp_node = get_node();
	tmp_node->left = NULL;
	return( tmp_node );
}

int length_text(text_t *txt)
{
	int num;
	num = txt->key;
	return (num);
}

void left_rotation(text_t *n)
{
	text_t *tmp_node;
	int tmp_key;
	tmp_node = n->left; 
	tmp_key  = n->key;
	n->left  = n->right;        
	n->key   = n->right->key;
	n->right = n->left->right;  
	n->left->right = n->left->left;
	n->left->left  = tmp_node;
	n->left->key   = tmp_key;
}

void right_rotation(text_t *n)
{
	text_t *tmp_node;
	int tmp_key;
	tmp_node = n->right; 
	tmp_key  = n->key;
	n->right = n->left;        
	n->key   = n->left->key;
	n->left  = n->right->left;  
	n->right->left = n->right->right;
	n->right->right  = tmp_node;
	n->right->key   = tmp_key;
}

char * get_line(text_t *txt, int index)
{
	text_t *tmp_node;
	int n;
	char * line;
	tmp_node = txt;
	n = index;
	if(n > tmp_node->key)
	{
		printf("line not exist\n");
		return(NULL);
	}
	else if(n == tmp_node->key)
	{
		while(tmp_node->right != NULL)
		{
			tmp_node = tmp_node->right;
		}
		//printf("found line\n");
		line = (char *) tmp_node->left;
		return(line);
	}
	else
	{
		while( tmp_node->right != NULL )
		{
			if(n <= tmp_node->left->key)
			{
				tmp_node = tmp_node->left;
			}
			else
			{
				n = n - tmp_node->left->key;
				tmp_node = tmp_node->right;
			}
		}
		//printf("found line\n");
		line = (char *) tmp_node->left;
		return(line);
	}
}

char * set_line(text_t *txt, int index, char * new_line)
{
	text_t *tmp_node;
	int n;
	char * line;
	tmp_node = txt;
	n = index;
	if(n > tmp_node->key)
	{
		printf("line not exist\n");
		return(NULL);
	}
	else if(n == tmp_node->key)
	{
		while(tmp_node->right != NULL)
		{
			tmp_node = tmp_node->right;
		}
		line = (char *) tmp_node->left;
		tmp_node->left = (text_t *) new_line;
		return(line);
	}
	else
	{
		while( tmp_node->right != NULL )
		{
			if(n <= tmp_node->left->key)
			{
				tmp_node = tmp_node->left;
			}
			else
			{
				n = n - tmp_node->left->key;
				tmp_node = tmp_node->right;
			}
		}
		line = (char *) tmp_node->left;
		tmp_node->left = (text_t *) new_line;
		return(line);
	}
}

void append_line(text_t *txt, char *new_line)
{
	text_t *tmp_node;
	text_t *old_leaf, *new_leaf;
	if( txt->left == NULL )
	{
		txt->left = (text_t *) new_line;
		txt->key  = 1;
		txt->height = 0;
		txt->right  = NULL; 
	}
	else
	{
		text_t * path_stack[NUM_MAX];
		int  path_st_p = 0;
		tmp_node = txt;
		while( tmp_node->right != NULL )
		{
			path_stack[path_st_p++] = tmp_node;
			tmp_node = tmp_node->right;
		}
		/*perform the insert*/	
		old_leaf = get_node();
        old_leaf->left = tmp_node->left; 
        old_leaf->key = 1;
        old_leaf->right  = NULL;
        old_leaf->height = 0;
        new_leaf = get_node();
        new_leaf->left = (text_t *) new_line; 
        new_leaf->key = 1;
        new_leaf->right  = NULL;
        new_leaf->height = 0;
		tmp_node->left = old_leaf;
		tmp_node->right = new_leaf;
		tmp_node->height = 1;
		tmp_node->key = tmp_node->left->key + tmp_node->right->key;
		/* rebalance */
		while( path_st_p > 0)
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
					tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
					tmp_node->height = tmp_node->right->height + 1;
					tmp_node->key = tmp_node->left->key + tmp_node->right->key;
				}
				else
				{
					left_rotation( tmp_node->left );
					right_rotation( tmp_node );
					tmp_height = tmp_node->left->left->height; 
					tmp_node->left->height  = tmp_height + 1; 
					tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
					tmp_node->right->height = tmp_height + 1; 
					tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
					tmp_node->height = tmp_height + 2; 
					tmp_node->key = tmp_node->left->key + tmp_node->right->key;
				}
			}
			else if ( tmp_node->left->height - tmp_node->right->height == -2 )
			{
				if( tmp_node->right->right->height - tmp_node->left->height == 1 )
				{
					left_rotation( tmp_node );
					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
					tmp_node->height = tmp_node->left->height + 1;
					tmp_node->key = tmp_node->left->key + tmp_node->right->key;
				}
				else
				{
					right_rotation( tmp_node->right );
					left_rotation( tmp_node );
					tmp_height = tmp_node->right->right->height; 
					tmp_node->left->height  = tmp_height + 1; 
					tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
					tmp_node->right->height = tmp_height + 1; 
					tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
					tmp_node->height = tmp_height + 2; 
					tmp_node->key = tmp_node->left->key + tmp_node->right->key;
				}
			}
			else /* update height even if there was no rotation */ 
			{
				if( tmp_node->left->height > tmp_node->right->height )
				{
					tmp_node->height = tmp_node->left->height + 1;
					tmp_node->key = tmp_node->left->key + tmp_node->right->key;
				}
				else
				{
					tmp_node->height = tmp_node->right->height + 1;
					tmp_node->key = tmp_node->left->key + tmp_node->right->key;
				}
			}
		}
	}
	//printf("append line successfully\n");
}

void insert_line(text_t *txt, int index, char *new_line)
{
	int n;
	text_t *tmp_node;
	text_t *old_leaf, *new_leaf;
	if( txt->left == NULL )
	{
		txt->left = (text_t *) new_line;
		txt->key  = 1;
		txt->height = 0;
		txt->right  = NULL; 
	}
	else
	{
		text_t * path_stack[NUM_MAX];
		int  path_st_p = 0;
		tmp_node = txt;
		n = index;
		if( n > tmp_node->key )
		{
			append_line(txt, new_line);
		}
		else
		{
			while( tmp_node->right != NULL )
			{
				path_stack[path_st_p++] = tmp_node;
				if(n <= tmp_node->left->key)
				{
					tmp_node = tmp_node->left;
				}
				else
				{
					n = n - tmp_node->left->key;
					tmp_node = tmp_node->right;
				}
			}
			/*perform the insert*/	
			old_leaf = get_node();
			old_leaf->left = tmp_node->left; 
			old_leaf->key = 1;
			old_leaf->right  = NULL;
			old_leaf->height = 0;
			new_leaf = get_node();
			new_leaf->left = (text_t *) new_line; 
			new_leaf->key = 1;
			new_leaf->right  = NULL;
			new_leaf->height = 0;
			tmp_node->left = new_leaf;
			tmp_node->right = old_leaf;
			tmp_node->height = 1;
			tmp_node->key = tmp_node->left->key + tmp_node->right->key;
			/* rebalance */
			while( path_st_p > 0)
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
						tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
						tmp_node->height = tmp_node->right->height + 1;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
					else
					{
						left_rotation( tmp_node->left );
						right_rotation( tmp_node );
						tmp_height = tmp_node->left->left->height; 
						tmp_node->left->height  = tmp_height + 1; 
						tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
						tmp_node->right->height = tmp_height + 1; 
						tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
						tmp_node->height = tmp_height + 2; 
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
				}
				else if ( tmp_node->left->height - tmp_node->right->height == -2 )
				{
					if( tmp_node->right->right->height - tmp_node->left->height == 1 )
					{
						left_rotation( tmp_node );
						tmp_node->left->height = tmp_node->left->right->height + 1;
						tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
						tmp_node->height = tmp_node->left->height + 1;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
					else
					{
						right_rotation( tmp_node->right );
						left_rotation( tmp_node );
						tmp_height = tmp_node->right->right->height; 
						tmp_node->left->height  = tmp_height + 1; 
						tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
						tmp_node->right->height = tmp_height + 1; 
						tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
						tmp_node->height = tmp_height + 2; 
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
				}
				else /* update height even if there was no rotation */ 
				{
					if( tmp_node->left->height > tmp_node->right->height )
					{
						tmp_node->height = tmp_node->left->height + 1;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
					else
					{
						tmp_node->height = tmp_node->right->height + 1;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
				}
			}
			//printf("insert line %d successfully\n",index);
		}
	}
}

char * delete_line(text_t *txt, int index)
{
	text_t *tmp_node, *upper_node, *other_node;
	char *deleted_object;
	int n;
	if( txt->left == NULL )
	{
		printf("text no content\n");
		return( NULL );
	}
	else if( txt->right == NULL )
	{
		if(  txt->key == index )
		{
			deleted_object = (char *) txt->left;
			txt->left = NULL;
			printf("delete successfully\n");
			return( deleted_object );
		}
		else
		{
			printf("line not exist\n");
			return( NULL );
		}
	}	
	else
	{
		text_t * path_stack[NUM_MAX]; int path_st_p = 0;
		tmp_node = txt;
		n = index;
		if(n > tmp_node->key)
		{
			printf("line not exist\n");
			return(NULL);
		}
		else
		{
			while( tmp_node->right != NULL )
			{
				path_stack[path_st_p++] = tmp_node;
				upper_node = tmp_node;
				if( n <= tmp_node->left->key )
				{
					tmp_node = upper_node->left;
					other_node = upper_node->right;
				}
				else
				{
					n = n - tmp_node->left->key;
					tmp_node = upper_node->right;
					other_node = upper_node->left;
				}
			}
            upper_node->key = other_node->key;
            upper_node->left = other_node->left;
            upper_node->right = other_node->right;
            upper_node->height = other_node->height;
            deleted_object = (char *) tmp_node->left;
            return_node(tmp_node);
            return_node(other_node);
			/*start rebalance*/  
			path_st_p -= 1;
			while( path_st_p > 0)
			{
				int tmp_height, old_height;
				tmp_node = path_stack[--path_st_p];
				old_height= tmp_node->height;
				if( tmp_node->left->height - tmp_node->right->height == 2 )
				{
					if( tmp_node->left->left->height - tmp_node->right->height == 1 )
					{
						right_rotation( tmp_node ); 
						tmp_node->right->height = tmp_node->right->left->height + 1;
						tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
						tmp_node->height = tmp_node->right->height + 1;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
					else
					{
						left_rotation( tmp_node->left );
						right_rotation( tmp_node );
						tmp_height = tmp_node->left->left->height; 
						tmp_node->left->height  = tmp_height + 1;
						tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
						tmp_node->right->height = tmp_height + 1;
						tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
						tmp_node->height = tmp_height + 2;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
				}
				else if ( tmp_node->left->height - tmp_node->right->height == -2 )
				{
					if( tmp_node->right->right->height - tmp_node->left->height == 1 )
					{
						left_rotation( tmp_node ); 
						tmp_node->left->height = tmp_node->left->right->height + 1;
						tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
						tmp_node->height = tmp_node->left->height + 1;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
					else
					{
						right_rotation( tmp_node->right );
						left_rotation( tmp_node );
						tmp_height = tmp_node->right->right->height; 
						tmp_node->left->height  = tmp_height + 1;
						tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
						tmp_node->right->height = tmp_height + 1;
						tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
						tmp_node->height = tmp_height + 2;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
				}
				else /* update height even if there was no rotation */ 
				{
					if( tmp_node->left->height > tmp_node->right->height )
					{
						tmp_node->height = tmp_node->left->height + 1;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
					else
					{
						tmp_node->height = tmp_node->right->height + 1;
						tmp_node->key = tmp_node->left->key + tmp_node->right->key;
					}
				}
			}
			/*end rebalance*/
			return( deleted_object );
		}
	}
}

int main()
{  int i, tmp; text_t *txt1, *txt2, *txt3; char *c;
   printf("starting \n");
   txt1 = create_text();
   txt2 = create_text();
   txt3 = create_text();
   append_line(txt1, "line one" );
   if( (tmp = length_text(txt1)) != 1)
   {  printf("Test 1: length should be 1, is %d\n", tmp); exit(-1);
   }
   append_line(txt1, "line hundred" );
   insert_line(txt1, 2, "line ninetynine" );
   insert_line(txt1, 2, "line ninetyeight" );
   insert_line(txt1, 2, "line ninetyseven" );
   insert_line(txt1, 2, "line ninetysix" );
   insert_line(txt1, 2, "line ninetyfive" );
   for( i = 2; i <95; i++ )
     insert_line(txt1, 2, "some filler line between 1 and 95" );
   if( (tmp = length_text(txt1)) != 100)
   {  printf("Test 2: length should be 100, is %d\n", tmp); exit(-1);
   }
   printf("found at line 1:   %s\n",get_line(txt1,  1));
   printf("found at line 2:   %s\n",get_line(txt1,  2));
   printf("found at line 99:  %s\n",get_line(txt1, 99));
   printf("found at line 100: %s\n",get_line(txt1,100));
   for(i=1; i<=10000; i++)
   {  if( i%2==1 )
        append_line(txt2, "A");
      else 
        append_line(txt2, "B");
   }
   if( (tmp = length_text(txt2)) != 10000)
   {  printf("Test 3: length should be 10000, is %d\n", tmp); exit(-1);
   }
   c = get_line(txt2, 9876 );
   if( *c != 'B')
     {  printf("Test 4: line 9876 of txt2 should be B, found %s\n", c); exit(-1);
   }
   for( i= 10000; i > 1; i-=2 )
   {  c = delete_line(txt2, i);
      if( *c != 'B')
      {  printf("Test 5: line %d of txt2 should be B, found %s\n", i, c); exit(-1);
      }
      append_line( txt2, c );
   }
   for( i=1; i<= 5000; i++ )
   {  c = get_line(txt2, i);
      if( *c != 'A')
      {  printf("Test 6: line %d of txt2 should be A, found %s\n", i, c); exit(-1);
      }
   }
   for( i=1; i<= 5000; i++ )
     delete_line(txt2, 1 );
   for( i=1; i<= 5000; i++ )
   {  c = get_line(txt2, i);
      if( *c != 'B')
      {  printf("Test 7: line %d of txt2 should be B, found %s\n", i, c); exit(-1);
      }
   }
   set_line(txt1, 100, "the last line");
   for( i=99; i>=1; i-- )
     delete_line(txt1, i );
   printf("found at the last line:   %s\n",get_line(txt1,  1));
   for(i=0; i<1000000; i++)
     append_line(txt3, "line" );   
   if( (tmp = length_text(txt3)) != 1000000)
   {  printf("Test 8: length should be 1000000, is %d\n", tmp); exit(-1);
   }
   for(i=0; i<500000; i++)
     delete_line(txt3, 400000 );   
   if( (tmp = length_text(txt3)) != 500000)
   {  printf("Test 9: length should be 500000, is %d\n", tmp); exit(-1);
   }
   printf("End of tests\n");
}

