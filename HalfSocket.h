#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/string.h>

#include <linux/kernel.h>
#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/sched.h>

#include <asm/types.h>

#include "IntegerTable.h"


#define HALF_SOCKET_SERVER_READ 0x00
#define HALF_SOCKET_SERVER_WRITE 0x01
#define HALF_SOCKET_SERVER_INIT 0x02

#define HALF_SOCKET_MAJOR 0x137

typedef struct HalfSocket_Node {
	/* Input written by a client. Waiting to be read
	 * by ioctl( HALF_SOCKET_SERVER_READ )
	 */
	DECLARE_KFIFO_PTR( _m_ClientInput, unsigned char );

	/*
	 * Output written byte the server awaiting a
	 * read from the clients
	 */
	DECLARE_KFIFO_PTR( _m_ServerOutput, unsigned char );
	
	/*
	 * Clients waiting for data to be written by
	 * the server
	 */
	wait_queue_head_t _m_ClientWaitingForRead;

	/*
	 * Server threads waiting for data to be written by
	 * the client
	 */
	wait_queue_head_t _m_ServerWaitingForRead;

} HalfSocket;

typedef struct {
	int len;
	unsigned char* data;
} HalfSocket_Buffer;

/*
 * Global table of inodes to
 * Half sockets
 */
DECLARE_U64TABLE( _g_HalfSocketsTable );


/* Opens a half socket */
int HalfSocket_open( struct inode *inode, struct file *filep );

int HalfSocket_release( struct inode *inode, struct file *filep );

int HalfSocket_read( struct file *filep, char* buf, size_t count, loff_t *offp );

int HalfSocket_write( struct file *filep, const char* buf, size_t count, loff_t offp );

int HalfSocket_ioctl( struct inode *inode, struct file *file,
	uint ictl_num, ulong param );

/* These are specific ioctl calls */
int HalfSocket_ServerInit( struct inode *inode, struct file* file  );

int HalfSocket_ServerWrite( struct inode *inode, struct file* file, const HalfSocket_Buffer* buffer );

int HalfSocket_ServerRead( struct inode *inode, struct file* file, const HalfSocket_Buffer* buffer );


