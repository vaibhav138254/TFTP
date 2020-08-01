/*********************************************************************
 * HEADER NAME : header.h
 * DESCRIPTION : this is a header file which includes all the predefined 
 * 				 header files which will be required by the server to work.
 *
 * DATE 		AUTHOR			REFERENCE 			REASON
 * 04-02-2020										PROJECT
 * *******************************************************************/
#ifndef __HEADER_H_
#define __HEADER_H_

/********************************************************************
 * 				PREDEFINED HEADER FILES
 *******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/*********************************************************************
 *								MACROS
 *********************************************************************/								
#define MYPORT 4950
#define BUFFERSIZE 512
#define MAXBUFLEN 1024
#define MAX_FILENAME_LEN 100
#define MAX_READ_LEN 512
#define MAX_TRIES 3
#define MAX_PACKETS 99
#define SUCCESS 1
#define FAILURE 0

#endif

