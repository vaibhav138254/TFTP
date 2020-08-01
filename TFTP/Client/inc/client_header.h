#ifndef __CLIENT_HEADER_H__
#define __CLIENT_HEADER_H__


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX 10
#define MAX_READ_LEN 512
#define VAL   3
#define FAILURE 0
#define SUCCESS 1
#define MAX_SIZE 1024
#define MAX_TRIES 3
#define TIME_OUT 5
#define MAX_PACKETS 99
#define MAXBUFLEN 1024



extern int DownloadFromServer(int sockfd, struct sockaddr_storage address, socklen_t addr_len, struct addrinfo *p);
extern int WriteInFile(char *buff, int c_write, FILE *fp, char *last_recv_pkt);
extern int SendAck(int sockfd, char *buff, struct sockaddr_storage address, socklen_t addr_len);

extern int WaitingForAck(int sockfd ,char* buff, char* last_message, struct addrinfo *p, socklen_t addr_len, struct sockaddr_storage address);
extern int FileHandling(int sockfd, struct addrinfo *p, FILE *fp, char* last_message, struct sockaddr_storage address,  socklen_t addr_len);
extern int UploadToServer(int sockfd, struct sockaddr_storage address, struct addrinfo *p, socklen_t addr_len);

extern char* create_read_packet(char* filename);
extern char* create_write_packet(char* filename);
extern char* create_data_packet(int block, char* send_data);
extern char* create_ack_packet(char* block);
extern char* create_error_packet(char* error_code, char* error_msg);
extern int check_timeout(int sockfd, char *buff,  struct sockaddr_storage address, socklen_t addr_len);

#endif
