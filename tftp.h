#ifndef TFTP_H
#define TFTP_H

#define	SEGSIZE		512		/* data segment size */
#define PACKSIZE    (SEGSIZE + 2 + 2)
/*
 * Packet types.
 */
#define	RRQ 	01				/* read request */
#define	WRQ	    02				/* write request */
#define	DATA	03				/* data packet */
#define	ACK	    04				/* acknowledgement */
#define	ERROR	05				/* error code */


typedef struct {
    uint16_t th_opcode;
    union {
        uint16_t u_block;
        uint16_t u_code;
        char u_stuff[1];
    }th_u;
    char th_data[1];
}TFTPhdr_t;

#define  th_block    th_u.u_block
#define  th_code     th_u.u_code
#define  th_stuff    th_u.u_stuff

/*
 * Error codes.
 */
#define	EUNDEF		0		/* not defined */
#define	ENOTFOUND	1		/* file not found */
#define	EACCESS		2		/* access violation */
#define	ENOSPACE	3		/* disk full or allocation exceeded */
#define	EBADOP		4		/* illegal TFTP operation */
#define	EBADID		5		/* unknown transfer ID */
#define	EEXISTS		6		/* file already exists */
#define	ENOUSER		7		/* no such user */

#endif // TFTP_H
