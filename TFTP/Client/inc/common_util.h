/*********************************************************************
 * HEADER NAME : header.h
 * DESCRIPTION : this is a header file which includes all the predefined 
 * 				 header files which will be required by the server to work.
 *
 * DATE 		AUTHOR			REFERENCE 			REASON
 * 04-02-2020										PROJECT
 * *******************************************************************/
#ifndef __COMMON_UTIL_H_
#define __COMMON_UTIL_H_
/*********************************************************************
 *					COMMON FUNCTION PROTOTYPE
 *********************************************************************/								
extern int check_timeout(int sockfd, char *buf, struct sockaddr_storage their_addr, socklen_t addr_len);
extern void *get_in_addr(struct sockaddr *sa);
extern char *create_read_packet(char *filename);
extern char* create_write_packet(char *filename);
extern char* create_data_packet(int block, char *data);
extern char* create_ack_packet(char* block);
extern char* create_error_packet(char *errcode, char* errmsg);
extern int open_file(FILE** arg_file, char* arg_name, char* arg_mode);
extern int close_file(FILE** arg_file);

#endif

