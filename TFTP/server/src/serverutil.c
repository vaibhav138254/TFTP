/****************************************************************************
				FILE NAME     : 

				DESCRIPTION   :

				AUTHOR      DATE       REASON    COPYRIGHT

*****************************************************************************/

/****************************************************************************
						USER DEFINED HEADER FILES
****************************************************************************/

#include <serverutil.h>

/***********************************************************************************************
	FUNCTION NAME : Read_request
	
	DESCRIPTION   : 
	
	ARGUMENTS     :
	
	RETURNS       :
************************************************************************************************/

int read_request(int sockfd, char *buf, struct sockaddr_storage their_addr, struct addrinfo *result,socklen_t addr_len, char *str)
{
		
	int block = 1,
		remaining = 0,
		total = 0,
		ret_value = 0,
		times = 0,
		numbytes = -1,
		temp_bytes = -1;
			
	char filename[MAX_FILENAME_LEN] ={0},
		temp[MAX_READ_LEN]= {0};

    char *error_message = NULL,
		*transfer_message =NULL;
			 
	FILE *fp = NULL;
	strcpy(filename, buf+2);

	ret_value = open_file(&fp, filename, "rb");
		
	if(ret_value == FAILURE || access(filename, F_OK) == -1)
	{
		//SENDING ERROR PACKET - FILE NOT FOUND
		fprintf(stderr,"SERVER: file '%s' does not exist, sending error packet\n", filename);
		error_message = create_error_packet("01", "ERROR_FILE_NOT_FOUND");
		printf("%s\n", error_message);
		sendto(sockfd, error_message, strlen(error_message), 0, (struct sockaddr *)&their_addr, addr_len);
		exit(FAILURE);
	}

	//STARTING TO SEND FILE	
	
	//it will take the pointer to the end of the file
	fseek(fp, 0, SEEK_END);
	
	//ftell gives the current value of the pointer
	total = ftell(fp);
	
	//it will take the pointer to the starting of the file
	rewind(fp);
	
	remaining = total;
		
	if(remaining == 0)
	{
		remaining = remaining + 1;
	}
	else if(remaining % MAX_READ_LEN == 0)
	{
		remaining = remaining - 1;
	}

	while(remaining > 0)
	{
		//READING FILE
		if(remaining > MAX_READ_LEN)
		{
			fread(temp, MAX_READ_LEN, sizeof(char), fp);
			temp[MAX_READ_LEN] = '\0';
			remaining = remaining - (MAX_READ_LEN);
		} 
		else 
		{
			fread(temp, remaining, sizeof(char), fp);
			temp[remaining] = '\0';
			remaining = 0;
		}
	
		//SENDING - DATA PACKET
		transfer_message = create_data_packet(block, temp);
		numbytes = sendto(sockfd, transfer_message, strlen(transfer_message), 0, (struct sockaddr *)&their_addr, addr_len);
			
		if(-1 == numbytes)
		{
			if(EAGAIN == errno)
			{
				perror("resources unavailable\n");
			}
			else
			{
				perror("SERVER ACK: sendto");
				exit(FAILURE);
			}
		}
		printf("SERVER: sent %d bytes\n", numbytes);
	
		//WAITING FOR ACKNOWLEDGEMENT - DATA PACKET
		for(times = 0; times <= MAX_TRIES; ++times)
		{
			if(times == MAX_TRIES)
			{
				printf("SERVER: MAX NUMBER OF TRIES REACHED\n");
				exit(FAILURE);
			}
	
			numbytes = check_timeout(sockfd, buf, their_addr, addr_len);
			if(numbytes == -1)
			{
				perror("SERVER: Recieved Error");
				exit(FAILURE);
			} 
			else if(FAILURE == numbytes)
			{
				//timeout
				printf("SERVER: try no. %d\n", times);
				temp_bytes = sendto(sockfd, transfer_message, strlen(transfer_message), 0, result->ai_addr, result->ai_addrlen);
				if(-1 == temp_bytes)
				{
					perror("SERVER: ACK: sendto");
					exit(FAILURE);
				}
				printf("SERVER: sent %d bytes AGAIN\n", temp_bytes);
				continue;
			} 
			else 
			{ 
				//valid
				break;
			}
		}
		printf("SERVER: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), str, sizeof(str)));
		printf("SERVER: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';
		printf("SERVER: packet contains %s\n", buf);				
			
		++block;
		if( block > MAX_PACKETS )
		{
			block = 1;
		}
	}
		ret_value = close_file(&fp);

		if(FAILURE == ret_value)
		{
			printf("unable to close the file\n");
			exit(FAILURE);
		}
		return EXIT_SUCCESS;
}

/***********************************************************************************************
		FUNCTION NAME      : write_request
	
		DESCRIPTION        : 
	
		ARGUMENTS          :
	
		RETURNS            : 
************************************************************************************************/
int write_request(int sockfd, char *buf, struct sockaddr_storage their_addr, struct addrinfo *result,socklen_t addr_len, char *str)
{
	char *message = NULL,
		*error_message = NULL,
		*transfer_message = NULL;
		
	char last_recv_message[MAXBUFLEN] = {0},
		 last_sent_ack[BUF_LENGTH] = {0},
		 filename[MAX_FILENAME_LEN] = {0},
		 block[BLOCK_SIZE] ={0};
			
	int numbytes = -1,
		c_written = -1,
		ret_value = -1;
		
	FILE *fp = NULL;
			
	message= create_ack_packet("00");
	
	if(	NULL == message)
	{
		printf("creation of acknowledgement failed\n");
	}		
		
	strcpy(last_recv_message, buf);
	strcpy(last_sent_ack, message);
		
	numbytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&their_addr, addr_len);
	if(	-1 == numbytes)
	{
		perror("SERVER ACK: send failed\n");
		exit(FAILURE);
	}
	printf("SERVER: sent %d bytes\n", numbytes);
	strcpy(filename, buf+2);
	
	if(	(access(filename, F_OK)) != -1)
	{ 
		//SENDING ERROR PACKET - DUPLICATE FILE
		fprintf(stderr,"SERVER: file %s already exists, sending error packet\n", filename);
		error_message = create_error_packet("06", "ERROR_FILE_ALREADY_EXISTS");
		sendto(sockfd, error_message, strlen(error_message), 0, (struct sockaddr *)&their_addr, addr_len);
		exit(FAILURE);
	}
	
	ret_value = open_file(&fp, filename, "wb");
		
	if(FAILURE == ret_value || access(filename, W_OK) == -1)
	{ 
		//SENDING ERROR PACKET - ACCESS DENIED
		fprintf(stderr,"SERVER: file %s access denied, sending error packet\n", filename);
		error_message = create_error_packet("05", "ERROR_ACCESS_DENIED");
		sendto(sockfd, error_message, strlen(error_message), 0, (struct sockaddr *)&their_addr, addr_len);
		exit(FAILURE);
	}
		
	do
	{
		//RECEIVING FILE - PACKET DATA
		numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len); 
		if(numbytes == -1)
		{
			perror("SERVER: recvfrom");
			exit(1);
		}
			
		printf("SERVER: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), str, sizeof(str)));
		printf("SERVER: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';
		printf("SERVER: packet contains \"%s\"\n", buf);
					
		//SENDING LAST ACK AGAIN - AS IT HAS NOT REACHED
		if(!(strcmp(buf, last_recv_message)))
		{
			sendto(sockfd, last_sent_ack, strlen(last_sent_ack), 0, (struct sockaddr *)&their_addr, addr_len);
			continue;
		}
	
		//WRITING FILE
		c_written = strlen(buf+4);
		fwrite(buf+4, sizeof(char), c_written, fp);
		strcpy(last_recv_message, buf);
			
		//SENDING ACKNOWLEDGEMENT - PACKET DATA
			
		strncpy(block, buf+2, 2);
		block[2] = '\0';
		transfer_message = create_ack_packet(block);
		numbytes = sendto(sockfd, transfer_message, strlen(transfer_message), 0, (struct sockaddr *)&their_addr, addr_len);
		if(-1 == numbytes) 
		{
			perror("SERVER ACK: sendto");
			exit(FAILURE);
		}
		printf("SERVER: sent %d bytes\n", numbytes);
		strcpy(last_sent_ack, transfer_message);
	}
		
		while(c_written == MAX_READ_LEN);
		printf("NEW FILE: %s SUCCESSFULLY MADE\n", filename);
		
		ret_value = close_file(&fp);
		if(FAILURE == ret_value)
		{
			printf("unable to close the file\n");
			exit(FAILURE);
		}
		
		return SUCCESS;
}

/***********************************************************************************************
	FUNCTION NAME : check_dir
	
	DESCRIPTION   : this function is used to check the directory or
	
	ARGUMENTS     :
	
	RETURNS       :
************************************************************************************************/


int check_dir(int sockfd,char *buf, struct sockaddr_storage their_addr, socklen_t addr_len)
{
	DIR *dir = NULL;

	char *directory = "./../data/server/",
		*uniqueId = NULL;

    int ret = -1,
	     ret_val = -1,
	     numbytes = -1,
		 buflen = 0;

	chdir(directory);

	numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len); 
	if(-1 == numbytes)
	{
		perror("SERVER: UniqueID not recieved");
		exit(1);
	}
	
	buflen = strlen(buf);
    uniqueId = (char*)malloc(sizeof(char)*buflen);
   
	memset(uniqueId,'\0',sizeof(char)*buflen);
	strcpy(uniqueId,buf);
        
    dir = opendir(uniqueId);
    if(dir == 0)
    {
   	  // printf("Directory exist\n");
      ret = chdir(uniqueId);
      if(ret == 0)
      {
      	//  printf("Directory Entered\n");
		return SUCCESS;
      }
      else
      {
      	printf("change directory failed\n");
		closedir(dir);
      }
	return FAILURE;
   }
	//ENOENT is for no such file or directory
   else if(ENOENT == errno)
   {
   	  printf("Directory does not exist\n");
      printf("creating directory\n");

		//read, write and execute permission to the user(S_IRWXU)
      ret = mkdir(uniqueId, S_IRWXU);
      if(ret == 0)
      {
      	//printf("directory creation successful\n");
      	ret_val=chdir(uniqueId);
       	if(ret_val == 0)
        {
         	printf("Ready for transmission\n");
			return SUCCESS;
        }
        else
        {
         	printf("directory entered failed after creation\n");
			return FAILURE;
        }
      }
      else
      {
       	printf("directory creation failed\n");
	  }
   }
   else
   {
		printf("permission denied to access\n");
   }
   return FAILURE;
}

