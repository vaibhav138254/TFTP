/************************************************************************
				FILE NAME     :  function.c

				DESCRIPTION   :  this file contains various functions
								 related to server and client

				AUTHOR          DATE      REASON       COPYRIGHT
			
			GR_Rossum_02   2-FEB-2020   PROJECT    ALTRAN COPYRIGHT
************************************************************************/

/***********************************************************************
			INCLUDING USER DEFINED HEADER FILE
************************************************************************/
#include <clientutil.h>
#include <common_util.h>

/*********************************************************************
			Function name    :   create_read_packet

			Description      :   this function is used to generate a
								 a read request packet

			Parameters       :   parameter is filename

			Return value     :   it will return the packet having file
								 with opcode 01

**********************************************************************/

char* create_read_packet(char* filename)
{
	char* read_packet = NULL;

	//memory allocation for read_packet
	read_packet = (char*)malloc(strlen(filename) + 2);
	

	//error checking for read_packet
	if(read_packet == NULL)
	{
		printf("MEMORY ALLOCATION FAILED FOR read_packet\n");
		exit(FAILURE);
	}

	//Initializing the read_packet with zero
	memset(read_packet, 0, sizeof(read_packet));

	//concating the opcode to read_packet(opcode for acknowledgement is 01)
	strcat(read_packet,"01");
	strcat(read_packet, filename);
	
	return read_packet;
}


/***********************************************************************
			Function name    :  create_write_packet

			Description      :  this function is used to generate a read
								request packet

			Parameters       :  parameter is file

			Return value     :  it will return the write_packet

************************************************************************/

char* create_write_packet(char* filename)
{
	char* write_packet = NULL;

	//memory allocation for write packet
	write_packet = (char*)malloc(strlen(filename) + 2);
	
	//error checking for write_packet
	if(write_packet == NULL)
	{
		printf("MEMORY ALLOCATION FAILED FOR WRITE_PACKET\n");
		exit(FAILURE);
	}

	//Initializing write_packet with zero
	memset(write_packet, 0, sizeof(write_packet));

	//concating the opcode to write_packet(opcode for acknowledgement is 02)
	strcat(write_packet, "02");
	strcat(write_packet, filename);
	
	return write_packet;
}

/*************************************************************************
			Function name    : create_data_packet

			Description      : this function is used to generate the data
							   packet

			Parameters       : parameters are block and data

			Return value     : It will return the data packet

***************************************************************************/

char* create_data_packet(int block, char* send_data)
{
	char* data_packet = NULL;
	
	//memory allocation for data packet
	data_packet = (char*)malloc(sizeof(char)*(strlen(send_data) + 2));
	
	//error checking for data_packet
	if(data_packet == NULL)
	{
		printf("MEMORY ALLOCATION FAILED FOR WRITE PACKET\n");
		exit(FAILURE);
	}

	//initializing data packet with zero
	memset(data_packet, '\0', strlen(data_packet)+1);

	//concating the opcode to data packet(opcode for data is 03)
	strcat(data_packet, "03");
	strcat(data_packet, send_data);
	
	return data_packet;
}

/***********************************************************************
			Function name    :  create_ack_packet

			Description      :  this function is used to generate a 
								acknowledgment request packet

			Parameters       :  parameter is block

			Return value     :  it will return the acknowledgment packet
************************************************************************/

char* create_ack_packet(char* block)
{
	char* ack_packet = NULL ;

	//allocating memory for  packet
	ack_packet = (char*)malloc(strlen(block) + 2);
	
	//error checking for packet
	if(ack_packet == NULL)
	{
		printf("MEMORY ALLOCATION FAILED FOR ACKNOWLEDGEMENT PACKET\n");
		exit(FAILURE);
	}

	//Initializing the packet with zero value
	memset(ack_packet, 0, sizeof(ack_packet));

	//concating the opcode to ack_packet(opcode for acknowledgement is 04)
	strcat(ack_packet, "04");
	strcat(ack_packet, block);
	
	return ack_packet;
}

/*************************************************************************
			Function name     :  create_error_packet

			Description       :  this function is used to generate a error
								 packet

			Parameters        : Parameters are error code and error message

			Return value      : it will return the error_packet
**************************************************************************/

char* create_error_packet(char* error_code, char* error_msg)
{
	char* err_packet = NULL;

	//allocating memory for packet
	err_packet = (char*)malloc(strlen(error_msg) + 4);
	
	//error checking for packet
	if(err_packet == NULL)
	{
		printf("MEMORY ALLOCATION FAILED FOR ERROR PACKET\n");
		exit(FAILURE);
	}

	//Initializing the packet with zero value
	memset(err_packet, 0, sizeof(err_packet));

	//concating the opcode to err_packet(opcode for error is 05)
	strcat(err_packet, "05");
	strcat(err_packet, error_code);
	strcat(err_packet, error_msg);

	return err_packet;
}

/***********************************************************************
			Function name     :  open_file

			Description       : this function is used to open a file

			Parameters        : parameters are address of file pointer,
								name of the file and mode of the file

			Return value      : it will return SUCCESS or FAILURE
***********************************************************************/


int open_file(
				FILE** arg_file,    //address of file pointer
				char* arg_name,   //name of the file
				char* arg_mode    //mode of the file
					)
{
	*arg_file = fopen(arg_name, arg_mode);

	if(*arg_file == NULL)
	{
		return FAILURE;
	}
	else
	{	
		return SUCCESS;
	}
}


/************************************************************************
			Function name      :   close_file

			Description        :   this function is used to close a 
								   file

			Parameters         :   parameter is address of file pointer

			Return value       :   it will return SUCCESS or FAILURE
************************************************************************/

int close_file(
					FILE** arg_file  //address of file pointer
					 )
{
	int ret_value = 0;

	ret_value = fclose(*arg_file);

	if(ret_value != 0)
	{
		return FAILURE;
	}
	
	return SUCCESS;
}



/***********************************************************************
			Function name    :  check_for_timeout

			Description      :  this function is used to check 

			Parameters       :  parameter is socketfd

			Return value     :  
************************************************************************/
int check_timeout(int sockfd, char* buf, struct sockaddr_storage their_addr, socklen_t addr_len)
{
	fd_set read_file_fds;
	int select_ret = 0;

	//Time structure involved
	struct timeval timeout;

	FD_ZERO(&read_file_fds); //clears the set
	FD_SET(sockfd, &read_file_fds); //ADD A FILE DESCRIPTOR FROM A SET

	//timeout is 5 sec
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	
	//wait until timeout or data recieved
	select_ret = select(sockfd, &read_file_fds, NULL, NULL, &timeout);

	//select return zero if the timeout expires before anything happens
	if(select_ret == 0)
	{
		printf("TIMEOUT\n");
		return FAILURE;
	}
	else
	{
		perror("SELECT ERROR");
		exit(FAILURE);
	}
	
	return recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr*)&their_addr, &addr_len);
	
}

/****************************************************************************
				Function name     : get_in_addr

				Description       : this function is used to determine if the internet
									family will be IPV4 OR IPV6

				Arguments         :

				Return value      :
*****************************************************************************/

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	else
	{
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}
}


