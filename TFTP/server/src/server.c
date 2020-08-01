/**********************************************************************
 * 			FILE NAME 	: TFTP SERVER
 * 
 * 			DESCRIPTION  : TFTP is a trivial file transfer Protocol which is to 
 * 				  				provide service to theMultiple client of upload and 
 * 				  				downloading the file 
 *
 * 			DATE 				AUTHOR		REFERENCE 			REASON
 *			
 *			04 -02-2020		GR_Rossum_02							PROJECT
 * *******************************************************************/

/*********************************************************************
 			 			USER-DEFINED HEADER FILE
 *********************************************************************/

#include <serverutil.h>

/*********************************************************************
  	FUNCTION NAME : MAIN
  	
  	DESCRIPTION   :	this is a driver function which broadcast the IP of
  					the server and which also work on confrigurable network
  					also
  	
  	ARGUMENT      : argv and argc are self explainatary
 	
 	RETURNS       : FAILURE AND SUCCESS
 *******************************************************************/

int main(void)
{
	int sockfd, 
	    get_return = -1,
	    numbytes = -1,
		ret_val = -1,
		bind_ret = -1,
		ret_value = -1,
		unique_id = -1;

	struct addrinfo server, 
			*servinfo = NULL, 
			*result = NULL;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;

	char buf[MAXBUFLEN] = {0},
		str[INET6_ADDRSTRLEN] = {0};
	
	//	char *PORT = (char*)MYPORT;
		//===========CONFIGURATION OF SERVER - START===========
	
	memset(&server, 0, sizeof(server));
	server.ai_family = AF_UNSPEC; // Address family unspecified(IPV4 OR IPV6)
	server.ai_socktype = SOCK_DGRAM; //preffered socktype is SOCK_DGRAM
	
	get_return = getaddrinfo(NULL, MYPORT, &server, &servinfo); 
	if(0 != get_return)
	{
		fprintf(stderr, "SERVER: getaddrinfo: %s\n", gai_strerror(get_return));
		return FAILURE;
	}

		sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, 0);
		if(-1 == sockfd)
		{
			perror("SERVER: socket");
			exit(FAILURE);
		}

	 	bind_ret = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
		if(-1 == bind_ret)
		{
			close(sockfd);
			perror("SERVER: bind");
		    exit(FAILURE);	
		}
	
	if(servinfo == NULL) 
	{
		fprintf(stderr, "SERVER: failed to bind socket\n");
		return FAILURE;
	}
	
	//freeaddrinfo(servinfo);
	
	printf("WELCOME TO THE SERVER......!!!!!\n");
	printf("SERVER: waiting to Client...\n");
	//===========CONFIGURATION OF SERVER - ENDS===========
	

	//===========MAIN IMPLEMENTATION - STARTS===========
	
	unique_id = check_dir(sockfd,buf, their_addr,addr_len);
	
	if(-1 == unique_id)
	{
		perror("direction check failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Welcome Client\n");
	printf("Ready for Transmission\n");
	
	//WAITING FOR FIRST REQUEST FROM CLIENT - RRQ/WRQ	
	addr_len = sizeof(their_addr);

	numbytes = recvfrom(sockfd, buf, MAXBUFLEN, 0, (struct sockaddr *)&their_addr, &addr_len);
	if(-1 == numbytes)
	{
		perror("SERVER: Not Recieved");
		exit(FAILURE);
	}
	
	printf("SERVER: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), str, sizeof(str)));
	printf("SERVER: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	printf("SERVER: packet contains \"%s\"\n", buf);

	//READ REQUEST FROM CLIENT
	if((buf[0] == '0') && (buf[1] == '1'))
	{
		ret_val = read_request(sockfd, buf, their_addr, result, addr_len, str);
		if(-1 == ret_val)
		{
			printf("Upload Request Failed\n");
			exit(FAILURE);
		}
	} 

	//WRITE REQUEST FROM CLIENT
	else if((buf[0] == '0') && (buf[1] == '2'))
	{
		ret_value = write_request(sockfd, buf, their_addr, result,addr_len, str);
		if(-1 == ret_value)
		{
			printf("Download Request Failed\n");
			exit(FAILURE);
		}
	} 
	else 
	{
		fprintf(stderr,"INVALID REQUEST\n");
		exit(FAILURE);
	}
	//===========MAIN IMPLEMENTATION - ENDS===========
	close(sockfd);
	return 0;
}


