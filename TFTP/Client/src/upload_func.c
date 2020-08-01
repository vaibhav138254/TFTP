


/************************************************************************
				INCLUDING USER DEFINED HEADER FILE
**************************************************************************/

#include <clientutil.h>

/************************************************************************
			Function name     :  file_size

			Description       :

			Parameters        :

			Return value      :
************************************************************************/
int FileHandling(int sockfd, struct addrinfo *p, FILE *fp, char* last_message, struct sockaddr_storage address,  socklen_t addr_len)
{
	//calculating of size of file
	char buff[MAX_SIZE] = {0},
		temp[MAX_READ_LEN] = {0};
	char *trans_pkt = NULL;
	int block = 1,
		numbytes = -1,
		check_ack = -1;
	fseek(fp, 0, SEEK_END);
	int total = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int remaining = total;
	if(remaining == 0)
	{
		++remaining;
	
	}
	else if(remaining%MAX_READ_LEN == 0)
	{
		--remaining;
	}
	while(remaining>0)
	{
		//READING FILE - DATA PACKET
		if(remaining>MAX_READ_LEN)
		{
			fread(temp, MAX_READ_LEN, sizeof(char), fp);
			temp[MAX_READ_LEN] = '\0';
			remaining -= (MAX_READ_LEN);
		}
		else
		{
			fread(temp, remaining, sizeof(char), fp);
			temp[remaining] = '\0';
			remaining = 0;
		}

		//SENDING FILE - DATA PACKET
		trans_pkt = (char*)malloc(sizeof(char)* MAX_SIZE);
		trans_pkt = create_data_packet(block, temp);
		numbytes = sendto(sockfd, trans_pkt, strlen(trans_pkt), 0, p->ai_addr, p->ai_addrlen);
		if(-1 == numbytes)
		{
			perror("CLIENT: sendto");
			exit(FAILURE);
		}
		printf("CLIENT: sent %d bytes to \n", numbytes);
		last_message = trans_pkt;
				
		//WAITING FOR ACKNOWLEDGEMENT - DATA PACKET
		addr_len = sizeof address;

		check_ack = WaitingForAck(sockfd, buff, last_message, p, addr_len, address);
		if(FAILURE == check_ack)
		{
			printf(" ");
		}
		printf("CLIENT: packet is %d bytes long\n", numbytes);
		buff[numbytes] = '\0';
		printf("CLIENT: packet contains %s\n", buff);
		
		if(buff[0]=='0' && buff[1]=='5')
		{//if error packet received
			fprintf(stderr, "CLIENT: got error packet: %s\n", buff);
			exit(1);
		}
		++block;
		if(block > MAX_PACKETS)
		{
			block = 1;
		}
		fclose(fp);
		free(trans_pkt);
		trans_pkt = NULL;
	}
	return SUCCESS;
}


/************************************************************************
			Function name     :  file_size

			Description       :

			Parameters        :

			Return value      :
************************************************************************/
 
int WaitingForAck(int sockfd ,char* buff, char* last_message, struct addrinfo *p, socklen_t addr_len, struct sockaddr_storage address)
{
	int time,
		numbytes = -1,
		temp_bytes = -1;
	for(time = 0; time <= MAX_TRIES; ++time)
	{
		if(time == MAX_TRIES)
		{
			printf("CLIENT: MAX NUMBER OF TRIES REACHED\n");
			exit(1);
		}
	
	
		numbytes = check_timeout(sockfd, buff, address, addr_len);
		if(-1 == numbytes)
		{
			perror("CLIENT: problem in recvfrom");
			exit(FAILURE);
		}
		else if(FAILURE == numbytes)
		{
			printf("CLIENT: try no. %d\n", time+1);

			temp_bytes = sendto(sockfd, last_message, strlen(last_message), 0, p->ai_addr, p->ai_addrlen);
			if(-1 == temp_bytes)
			{
				perror("CLIENT ACK: sendto");
				exit(1);
			}	
			printf("CLIENT: sent %d bytes again\n", temp_bytes);
			continue;
		}
		else
		{
			break;
		}
	}
	return SUCCESS;
}


/************************************************************************
			Function name     :  file_size

			Description       :

			Parameters        :

			Return value      :
************************************************************************/
int UploadToServer(int sockfd, struct sockaddr_storage address, struct addrinfo *p, socklen_t addr_len)
{
	FILE *fp = NULL;
	int numbytes = -1,
	//	wait_ack = -1,
		check_ack = -1,
		file = -1;
	char file[MAX_SIZE] = {0};
	char* message = NULL;
	char buff[MAX_SIZE] = {0};
	char* last_message = NULL;
	
	printf("Enter the file name want to upload.\n");
	scanf("%s", file);
	message = create_write_packet(file);
	numbytes = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen);
	if(-1 == numbytes)
	{
		perror("CLIENT: sendto");
		exit(1);
	}
	printf("CLIENT: sent %d bytes to the server\n", numbytes);
	
	last_message = (char*)malloc(sizeof(char)*strlen(message)+1);
	memset(last_message, '\0', sizeof(strlen(message)+1));
	strcpy(last_message, message);
	
	//WAITING FOR ACK - WRQ
	addr_len = sizeof address;
	check_ack = WaitingForAck(sockfd, buff, last_message, p, addr_len, address);
	if(FAILURE == check_ack)
	{
		printf("\nACKNOWLEDGE FAILED: ");
	}
	printf("CLIENT: packet is %d bytes long\n", numbytes);
	buff[numbytes] = '\0';
	printf("CLIENT: packet contains %s\n", buff);
	
	if((buff[0] == '0') && (buff[1] == '4'))
	{
		fp = fopen(file, "rb");
		if((NULL == fp) || (access(file, F_OK )== -1))
		{
			fprintf(fp,"CLIENT: file not exist\n") ;
			exit(1);
		}
		// CALCULATING FILE SIZE
		file = FileHandling(sockfd, p, fp, last_message, address, addr_len);
		if(FAILURE == file)
		{
			printf(" ");
		}
	}	
	else 
	{
		fprintf(stderr,"CLIENT ACK: expecting but got: %s\n", buff);
		exit(1);
	}
	
	return SUCCESS;
}



