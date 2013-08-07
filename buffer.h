#ifndef BUFFER_H_
#define BUFFER_H_

#define BLOCK_SIZE 512

struct buffer_block {
	char buf[ BLOCK_SIZE ];

	unsigned startptr;
	unsigned endptr;

	struct buffer_block* next;
};

struct read_buffer {
	struct buffer_block* start_block;
	struct buffer_block* end_block;
};

void read_buffer_put( struct read_buffer* buf, const char* chars, unsigned len ) ;

unsigned read_buffer_get( struct read_buffer* buf, char* chars, unsigned len ) ;

#endif
