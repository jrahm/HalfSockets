/*
 * A small, quick library desgined to map a 64-bit integer to
 * a void* of some kind. The table is set up as a simple heirarchy
 * of bitmap, similar to how a cache is implemented, using the bits of
 * the number to narrow the search.
 */

#ifndef INTEGER_TABLE_H_
#define INTEGER_TABLE_H_

#include <linux/kernel.h>
#include <linux/vmalloc.h>

/* A dynamically expanding table
 * of values. Ues the bits to find the
 * correct values
 *
 * Uses blocks of size 4 bits
 */

#define U64_TABLE_NBITS     (4)
#define U64_TABLE_NCHILDREN (1 << U64_TABLE_NBITS)
#define U64_TABLE_LEVELS    (64 / U64_TABLE_NBITS) // 16
#define U64_TABLE_MASK      ((1 << U64_TABLE_NBITS) - 1) // 0x0F

#define KPRINT_STR "[u64Table] - "

#define DECLARE_U64TABLE( name )\
	struct u64Table_Node name = \
		{ NULL, 0 }

struct u64Table_Node {
	/* A pointer to the parent node */
	struct u64Table_Node* _m_parent;

	/* The index of this child as seen from the parent */
	int _m_parent_index;

	/* Pointers to either sub-tables or
	 * pointers to values */
	void* _m_children[U64_TABLE_NCHILDREN];
};

typedef struct u64Table_Node u64Table;

/*
 * Creates a new node with the specified parent node and
 * the index of the child from the view of the parent
 */
struct u64Table_Node* _p_u64TableNewNode( struct u64Table_Node* parent,
	int parent_index ) ;

/*
 * Checks to see if a node is empty. That is if all the children
 * of the node are NULL
 */
int _p_u64Table_IsEmpty( struct u64Table_Node* node ) ;

/*
 * Returns a pointer to the leaf node that will contain the address
 * being searched for. If the create option is set to a non-zero value,
 * nodes will be created on the fly to hold the address.
 *
 * tmpidx_ptr is set to the index of the child for the index
 */
struct u64Table_Node* _p_u64Table_GetLeaf( struct u64Table_Node* node,
	__u64 index, int create, int* tmpidx_ptr ) ;

/*
 * Returns the void* mapped to the index `index`
 */
void* u64Table_Get( u64Table* table, __u64 index ) ;

/*
 * Inserts a value into the table.
 * returns the previous value held at that
 * index.
 */
void* u64Table_Insert( u64Table* table, __u64 index, void* value ) ;

/*
 * Sets the index specified to NULL and returns
 * the previous value.
 */
void* u64Table_Erase( u64Table* table, __u64 index ) ;

#endif
