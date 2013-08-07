#include "IntegerTable.h"

/* Constructs a new node which is allocated
 * using vzalloc and should be freed with 
 * vfree
 */
struct u64Table_Node* _p_u64TableNewNode( struct u64Table_Node* parent,
	int parent_index ) {
	struct u64Table_Node* child;

	/*
	 * Allocate the new child and set everything to
	 * NULL
	 */
	child = vzalloc( sizeof( struct u64Table_Node ) );

	/* Set the parent and the parent index */
	child->_m_parent = parent;
	child->_m_parent_index = parent_index;

	return child;
}


/*
 * Checks to see if all the children of a node are set
 * to NULL.
 */
int _p_u64Table_IsEmpty( struct u64Table_Node* node ) {
	int i;

	/* Iterate through all the children */
	for( i = 0 ; i < U64_TABLE_NCHILDREN; ++ i ) {
		/* If one isn't null, return false */
		if( node->_m_children[i] != NULL ) {
			return 0;
		}
	}

	/* Otherwise return true. */
	return 1;
}

/* Returns the leaf where the mapping for a given index should
 * be stored, or NULL if no such node exists. If create!=0, then
 * new nodes will be created for that node
 */
struct u64Table_Node* _p_u64Table_GetLeaf( struct u64Table_Node* node,
	__u64 index, int create, int* tmpidx_ptr ) {
	int count;
	int tmpidx;

	struct u64Table_Node* child;

	/* Iterate through all the levels */
	for( count = 0; count < U64_TABLE_LEVELS - 1; ++ count ) {
		/* Grab the last N bits of the index. This
		 * will give us the index into the next table */
		tmpidx = index & U64_TABLE_MASK;

		/* Shift the index by N bits */
		index >>= U64_TABLE_NBITS;

		/* Grab the child */
		child = node->_m_children[ tmpidx ];
		
		/* If the child is NULL, we either create it or
		 * return NULL right here */
		if( child == NULL ) {
			if( create ) {
				child = _p_u64TableNewNode( node, tmpidx );
				node->_m_children[ tmpidx ] = child;
			} else {
				return NULL;
			}
		}

		/* Continue with child as our updated node */
		node = child;
	}

	/* set tmpidx to the index of the void* to return */
	*tmpidx_ptr = index & U64_TABLE_MASK;

	return node;
}

/*
 * Returns the void* mapped to `index`
 */
void* u64Table_Get( u64Table* table, __u64 index ) {
	int tmpidx;
	
	/*
	 * Get the leaf node where this mapping should exist
	 */
	table = _p_u64Table_GetLeaf( table, index, 0, &tmpidx );

	if( table == NULL ) {
		/*
		 * No such element
		 */
		return NULL;
	}

	/*
	 * Grab the element at tmpidx
	 */
	return table -> _m_children[tmpidx];
}

/*
 * Inserts an element into the table. If there already
 * exists an element at that mapping, it is replaced by the
 * new value and returned
 */
void* u64Table_Insert( u64Table* table, __u64 index, void* value ) {
	int tmpidx;
	void* ret;

	/*
	 * Get the leaf where it should exist
	 */
	table = _p_u64Table_GetLeaf( table, index, 1, &tmpidx );

	if( ! table ) {
		/*
		 * Should not get to this point because create
		 * mode is set to 1, so noisily continue
		 */
		printk( KPRINT_STR "Gremlins! -- _p_u64Table_GetLeaf returned NULL with create=1" );
		return NULL;
	}

	/*
	 * Get a handle to what already exists
	 */
	ret = table->_m_children[tmpidx];

	/*
	 * Overwrite what was there
	 */
	table->_m_children[tmpidx] = value;

	/*
	 * Return the old value
	 */
	return ret;
}

/*
 * Removes and returns an element from
 * a table
 */
void* u64Table_Erase( u64Table* table, __u64 index ) {
	int tmpidx;
	void* ret;

	/*
	 * Get where this entry should be
	 */
	table = _p_u64Table_GetLeaf( table, index, 0, &tmpidx );

	if( table == NULL ) {
		/*
		 * The entry doesn't exist, let's move
		 * on
		 */
		return NULL;
	}

	/*
	 * Get a handle to the old value
	 */
	ret = table -> _m_children[ tmpidx ];

	/*
	 * Overwrite with NULL
	 */
	table -> _m_children[ tmpidx ] = NULL;

	/*
	 * Delete nodes if they are empty. Start with a child
	 * and move up. Make sure we do not delete the root
	 * node
	 */
	while( _p_u64Table_IsEmpty( table ) &&
		table->_m_parent != NULL ) {
		/*
		 * Who am i to my parent
		 */
		tmpidx = table->_m_parent_index;

		/*
		 * Move to parent
		 */
		table = table->_m_parent;

		/*
		 * Free that child
		 */
		vfree( table -> _m_children[ tmpidx ] );

		/*
		 * Set that child to NULL
		 */
		table -> _m_children[ tmpidx ] = NULL;
	}

	return ret;
}
