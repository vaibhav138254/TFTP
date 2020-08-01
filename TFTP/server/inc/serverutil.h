/*******************************************************************************




*********************************************************************************/

#ifndef __SERVERUTIL_H_
#define __SERVERUTIL_H_

/********************************************************************************
									HEADER FILE
********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

/******************************************************************************
									MACRO DEFINITION
******************************************************************************/
#define MYPORT "57371" // port to be opened on server
#define SERVERPORT 57371 // the port users will be connecting to
#define MAXBUFLEN 1024 // get sockaddr, IPv4 or IPv6:
#define MAX_READ_LEN 512 // maximum data size that can be sent on one packet
#define MAX_FILENAME_LEN 255 // maximum length of file name supported
#define MAX_PACKETS 99 // maximum number of file packets
#define MAX_TRIES 3 // maximum number of tries if packet times out
#define TIME_OUT 5 // in seconds
#define BUF_LENGTH 10
#define BLOCK_SIZE 3
#define SUCCESS 1
#define FAILURE 0


/*********************************************************************
*					Function Prototypes
**********************************************************************/

extern void *get_in_addr(struct sockaddr *sa);
extern char *create_read_packet(char *filename);
extern char* create_write_packet(char *filename);
extern char* create_data_packet(int block, char *data);
extern char* create_ack_packet(char* block);
extern char* create_error_packet(char *errcode, char* errmsg);
extern int open_file(FILE** arg_file, char* arg_name, char* arg_mode);
extern int close_file(FILE** arg_file);
extern int read_request(int sockfd, char *buf, struct sockaddr_storage their_addr, struct addrinfo *result,socklen_t addr_len, char *s);
extern int write_request(int sockfd, char *buf, struct sockaddr_storage their_addr, struct addrinfo *result,socklen_t addr_len, char *s);
extern int check_dir(int sockfd,char *buf, struct sockaddr_storage their_addr, socklen_t addr_len);
extern int check_timeout(int sockfd, char *buf, struct sockaddr_storage their_addr, socklen_t addr_len);


#endif

