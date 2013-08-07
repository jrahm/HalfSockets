#include "HalfSocket.h"

int HalfSocket_open( struct inode* inode, struct file *filep ) {
	/*
	 * Nothing is needed at this point
	 */
	return 0;
}

int HalfSocket_read( struct inode* inode, struct file *filep ) {

}

int HalfSocket_ioctl( struct inode* inode, struct file* filep, uint ioctlN, ulong param ) {
	switch( ioctlN ) {
	case HALF_SOCKET_SERVER_READ:
		HalfSocket_ServerRead( inode, filep, (HalfSocket_Buffer*)param );
		break;

	case HALF_SOCKET_SERVER_WRITE:
		HalfSocket_ServerWrite( inode, filep, (HalfSocket_Buffer*)param );
		break;

	case HALF_SOCKET_SERVER_INIT:
		HalfSocket_ServerInit( inode, filep );
		break;
	}
}
