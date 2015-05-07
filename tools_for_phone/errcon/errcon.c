#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

typedef struct{
	int no;
	const char* str;
}err_items;

#define ITEM_DEF(X) {X,#X},

static err_items _err_items[]={
ITEM_DEF(EPERM)		//1	/* Operation not permitted */
ITEM_DEF(ENOENT)	//2	/* No such file or directory */
ITEM_DEF(ESRCH)		//3	/* No such process */
ITEM_DEF(EINTR)		//4	/* Interrupted system call */
ITEM_DEF(EIO) 	 	//5	/* I/O error */
ITEM_DEF(ENXIO)		//6	/* No such device or address */
ITEM_DEF(E2BIG)		//7	/* Argument list too long */
ITEM_DEF(ENOEXEC)	//8	/* Exec format error */
ITEM_DEF(EBADF)		//9	/* Bad file number */
ITEM_DEF(ECHILD	)	//10	/* No child processes */
ITEM_DEF(EAGAIN	)	//11	/* Try again */
ITEM_DEF(ENOMEM	)	//12	/* Out of memory */
ITEM_DEF(EACCES	)	//13	/* Permission denied */
ITEM_DEF(EFAULT	)	//14	/* Bad address */
ITEM_DEF(ENOTBLK)	//15	/* Block device required */
ITEM_DEF(EBUSY	)	//16	/* Device or resource busy */
ITEM_DEF(EEXIST	)	//17	/* File exists */
ITEM_DEF(EXDEV	)	//18	/* Cross-device link */
ITEM_DEF(ENODEV	)	//19	/* No such device */
ITEM_DEF(ENOTDIR)	//20	/* Not a directory */
ITEM_DEF(EISDIR	)	//21	/* Is a directory */
ITEM_DEF(EINVAL	)	//22	/* Invalid argument */
ITEM_DEF(ENFILE		)	//23	/* File table overflow */
ITEM_DEF(EMFILE		)	//24	/* Too many open files */
ITEM_DEF(ENOTTY		)	//25	/* Not a typewriter */
ITEM_DEF(ETXTBSY	)	//26	/* Text file busy */
ITEM_DEF(EFBIG		)	//27	/* File too large */
ITEM_DEF(ENOSPC		)	//28	/* No space left on device */
ITEM_DEF(ESPIPE		)	//29	/* Illegal seek */
ITEM_DEF(EROFS		)	//30	/* Read-only file system */
ITEM_DEF(EMLINK		)	//31	/* Too many links */
ITEM_DEF(EPIPE		)	//32	/* Broken pipe */
ITEM_DEF(EDOM		)	//33	/* Math argument out of domain of func */
ITEM_DEF(ERANGE		)	//34	/* Math result not representable */

ITEM_DEF(EDEADLK		)	//35	/* Resource deadlock would occur */
ITEM_DEF(ENAMETOOLONG	)	//36	/* File name too long */
ITEM_DEF(ENOLCK		)	//37	/* No record locks available */
ITEM_DEF(ENOSYS		)	//38	/* Function not implemented */
ITEM_DEF(ENOTEMPTY	)	//39	/* Directory not empty */
ITEM_DEF(ELOOP		)	//40	/* Too many symbolic links encountered */
ITEM_DEF(EWOULDBLOCK	)	//EAGAIN	/* Operation would block */
ITEM_DEF(ENOMSG		)	//42	/* No message of desired type */
ITEM_DEF(EIDRM		)	//43	/* Identifier removed */
ITEM_DEF(ECHRNG		)	//44	/* Channel number out of range */
ITEM_DEF(EL2NSYNC	)	//45	/* Level 2 not synchronized */
ITEM_DEF(EL3HLT		)	//46	/* Level 3 halted */
ITEM_DEF(EL3RST		)	//47	/* Level 3 reset */
ITEM_DEF(ELNRNG		)	//48	/* Link number out of range */
ITEM_DEF(EUNATCH		)	//49	/* Protocol driver not attached */
ITEM_DEF(ENOCSI		)	//50	/* No CSI structure available */
ITEM_DEF(EL2HLT		)	//51	/* Level 2 halted */
ITEM_DEF(EBADE		)	//52	/* Invalid exchange */
ITEM_DEF(EBADR		)	//53	/* Invalid request descriptor */
ITEM_DEF(EXFULL		)	//54	/* Exchange full */
ITEM_DEF(ENOANO		)	//55	/* No anode */
ITEM_DEF(EBADRQC		)	//56	/* Invalid request code */
ITEM_DEF(EBADSLT		)	//57	/* Invalid slot */

ITEM_DEF(EDEADLOCK	)	//EDEADLK

ITEM_DEF(EBFONT		)	//59	/* Bad font file format */
ITEM_DEF(ENOSTR		)	//60	/* Device not a stream */
ITEM_DEF(ENODATA	)	//	61	/* No data available */
ITEM_DEF(ETIME		)	//62	/* Timer expired */
ITEM_DEF(ENOSR		)	//63	/* Out of streams resources */
ITEM_DEF(ENONET		)	//64	/* Machine is not on the network */
ITEM_DEF(ENOPKG		)	//65	/* Package not installed */
ITEM_DEF(EREMOTE	)	//	66	/* Object is remote */
ITEM_DEF(ENOLINK	)	//	67	/* Link has been severed */
ITEM_DEF(EADV		)	//68	/* Advertise error */
ITEM_DEF(ESRMNT		)	//69	/* Srmount error */
ITEM_DEF(ECOMM		)	//70	/* Communication error on send */
ITEM_DEF(EPROTO		)	//71	/* Protocol error */
ITEM_DEF(EMULTIHOP	)	//72	/* Multihop attempted */
ITEM_DEF(EDOTDOT	)	//	73	/* RFS specific error */
ITEM_DEF(EBADMSG	)	//	74	/* Not a data message */
ITEM_DEF(EOVERFLOW	)	//75	/* Value too large for defined data type */
ITEM_DEF(ENOTUNIQ	)	//76	/* Name not unique on network */
ITEM_DEF(EBADFD		)	//77	/* File descriptor in bad state */
ITEM_DEF(EREMCHG	)	//	78	/* Remote address changed */
ITEM_DEF(ELIBACC	)	//	79	/* Can not access a needed shared library */
ITEM_DEF(ELIBBAD	)	//	80	/* Accessing a corrupted shared library */
ITEM_DEF(ELIBSCN	)	//	81	/* .lib section in a.out corrupted */
ITEM_DEF(ELIBMAX	)	//	82	/* Attempting to link in too many shared libraries */
ITEM_DEF(ELIBEXEC	)	//83	/* Cannot exec a shared library directly */
ITEM_DEF(EILSEQ		)	//84	/* Illegal byte sequence */
ITEM_DEF(ERESTART	)	//85	/* Interrupted system call should be restarted */
ITEM_DEF(ESTRPIPE	)	//86	/* Streams pipe error */
ITEM_DEF(EUSERS		)	//87	/* Too many users */
ITEM_DEF(ENOTSOCK	)	//88	/* Socket operation on non-socket */
ITEM_DEF(EDESTADDRREQ	)	//89	/* Destination address required */
ITEM_DEF(EMSGSIZE	)	//90	/* Message too long */
ITEM_DEF(EPROTOTYPE	)	//91	/* Protocol wrong type for socket */
ITEM_DEF(ENOPROTOOPT)	//92	/* Protocol not available */
ITEM_DEF(EPROTONOSUPPORT)	//93	/* Protocol not supported */
ITEM_DEF(ESOCKTNOSUPPORT)	//94	/* Socket type not supported */
ITEM_DEF(EOPNOTSUPP	)	//95	/* Operation not supported on transport endpoint */
ITEM_DEF(EPFNOSUPPORT)	//96	/* Protocol family not supported */
ITEM_DEF(EAFNOSUPPORT)	//97	/* Address family not supported by protocol */
ITEM_DEF(EADDRINUSE	)	//98	/* Address already in use */
ITEM_DEF(EADDRNOTAVAIL)	//99	/* Cannot assign requested address */
ITEM_DEF(ENETDOWN	)	//100	/* Network is down */
ITEM_DEF(ENETUNREACH)	//101	/* Network is unreachable */
ITEM_DEF(ENETRESET	)	//102	/* Network dropped connection because of reset */
ITEM_DEF(ECONNABORTED)	//103	/* Software caused connection abort */
ITEM_DEF(ECONNRESET	)	//104	/* Connection reset by peer */
ITEM_DEF(ENOBUFS	)	//105	/* No buffer space available */
ITEM_DEF(EISCONN	)	//106	/* Transport endpoint is already connected */
ITEM_DEF(ENOTCONN	)	//107	/* Transport endpoint is not connected */
ITEM_DEF(ESHUTDOWN	)	//108	/* Cannot send after transport endpoint shutdown */
ITEM_DEF(ETOOMANYREFS)	//109	/* Too many references: cannot splice */
ITEM_DEF(ETIMEDOUT	)	//110	/* Connection timed out */
ITEM_DEF(ECONNREFUSED)	//111	/* Connection refused */
ITEM_DEF(EHOSTDOWN	)	//112	/* Host is down */
ITEM_DEF(EHOSTUNREACH)	//113	/* No route to host */
ITEM_DEF(EALREADY	)	//114	/* Operation already in progress */
ITEM_DEF(EINPROGRESS)	//115	/* Operation now in progress */
ITEM_DEF(ESTALE		)	//116	/* Stale NFS file handle */
ITEM_DEF(EUCLEAN	)	//117	/* Structure needs cleaning */
ITEM_DEF(ENOTNAM	)	//118	/* Not a XENIX named type file */
ITEM_DEF(ENAVAIL	)	//119	/* No XENIX semaphores available */
ITEM_DEF(EISNAM		)	//120	/* Is a named type file */
ITEM_DEF(EREMOTEIO	)	//121	/* Remote I/O error */
ITEM_DEF(EDQUOT		)	//122	/* Quota exceeded */

ITEM_DEF(ENOMEDIUM	)	//123	/* No medium found */
ITEM_DEF(EMEDIUMTYPE)	//124	/* Wrong medium type */
ITEM_DEF(ECANCELED	)	//125	/* Operation Canceled */
ITEM_DEF(ENOKEY		)	//126	/* Required key not available */
ITEM_DEF(EKEYEXPIRED)	//127	/* Key has expired */
ITEM_DEF(EKEYREVOKED)	//128	/* Key has been revoked */
ITEM_DEF(EKEYREJECTED)	//129	/* Key was rejected by service */

/* for robust mutexes */
ITEM_DEF(EOWNERDEAD	)	//130	/* Owner died */
ITEM_DEF(ENOTRECOVERABLE)	//131	/* State not recoverable */

//ITEM_DEF(ERFKILL	)	//132	/* Operation not possible due to RF-kill */

{0,NULL}
};

static void list_all_errors()
{
	err_items* iter = _err_items;
	int i = 0;
	while(1){
		if(iter[i].no == 0)
			return;
		printf("%d\t%-20s %s\n",iter[i].no,iter[i].str,(const char*)strerror(iter[i].no));
		i++;
	}
}

int main(int argc, char *argv[ ])
{
    int c;
	extern char *optarg;
    extern int optind, optopt;

    while ((c = getopt(argc, argv, ":hln:o:")) != -1) {
        switch(c) {
        case 'h':
            {
				//printf usage
				printf(	"-h print this help\n"
						"-l list common errors\n"
						"-n errno print error message\n");
			}
			break;
		case 'l':
			{
				list_all_errors();
			}
			return 0;
            break;
        case 'n':
            if(atoi(optarg)){
				int no = atoi(optarg);
				if(no <0) no=-no;
				printf("error:%d\t %s\n",no,(const char*)strerror(no));
			}
			return 0;
            break;
		}
	}

	return 0;
}
