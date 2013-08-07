#include "buffer.h"

#include <memory.h>
#include <stdlib.h>

static unsigned buffer_block_append_max( struct buffer_block* block,
	const char* data, unsigned len ) {
	unsigned to_write = BLOCK_SIZE - block->endptr;
	to_write = len < to_write ? len : to_write;

	unsigned off = block->endptr;
	unsigned i;
	for( i = 0 ; i < to_write; ++ i ) {
		block->buf[i + off] = data[i];
	}
	block->endptr += to_write;

	return to_write;
}

static struct buffer_block* new_buffer_block( ) {
	struct buffer_block* ret = malloc( sizeof( struct buffer_block) );
	memset( ret, 0, sizeof( struct buffer_block ) );
	return ret;
}

/*
 * Adds characters to a buffer block. Adds new
 * blocks on the the linked list of this block
 * if the amount of writing overflows.
 *
 * Returns a pointer to the new end of the list
 */
static struct buffer_block* buffer_block_append( struct buffer_block* block,
	const char* data, unsigned len ) {
	unsigned written;

	while( len > 0 ) {
		written = buffer_block_append_max( block, data, len );
		data += written;
		len -= written;
		if( len > 0 ) {
			block->next = new_buffer_block();
			block = block->next;
		}
	}

	return block;
}

static unsigned buffer_block_read_max( struct buffer_block* block,
	char* data, unsigned len ) {
	unsigned toread = block->endptr - block->startptr;
	toread = len < toread ? len : toread;

	memcpy( data, block->buf, toread );
	block->startptr += toread;

	return toread;
}

static struct buffer_block* buffer_block_read( struct buffer_block** blockptr,
	char* data, unsigned len, unsigned* bytes_read_ptr ) {
	struct buffer_block* block = *blockptr;
	struct buffer_block* tmp;
	unsigned bytes_read = 0;
	unsigned read;

	while( len > 0 ) {
		read = buffer_block_read_max( block, data, len );
		bytes_read += read;
		data += read;
		len -= read;
		if( len > 0 ) {
			if( (tmp = block->next) != NULL ) {
				free(block);
				if( block == *blockptr ) {
					*blockptr = NULL;
				}
				block = tmp;
			} else {
				break;
			}
		}
	}

	*bytes_read_ptr = bytes_read;
	return block;
}

void read_buffer_put( struct read_buffer* buf, const char* chars, unsigned len ) {
	buf->end_block = buffer_block_append( buf->end_block, chars, len );
}

unsigned read_buffer_get( struct read_buffer* buf, char* chars, unsigned len ) {
	unsigned read;
	buf->start_block = buffer_block_read( &buf->start_block, chars, len, &read );
	return read;
}

struct read_buffer* new_read_buffer( ) {
	struct read_buffer* ret = malloc( sizeof(struct read_buffer) );
	ret->start_block = ret->end_block = new_buffer_block();
	return ret;
}
