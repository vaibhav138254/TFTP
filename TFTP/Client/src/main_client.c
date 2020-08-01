/*********************************************************************************
 *
*********************************************************************************/
/********************************************************************************
 *
*********************************************************************************/
#include <clientutil.h>

/*********************************************************************************
 *
*********************************************************************************/

int main(int argc, char* argv[])
{
	char* server = NULL;
	char uniqueid[10] = {0};
	char* port = NULL;
	int sockfd = -1,
		rv = -1,
		option = -1,
		numbytes = -1;
	struct addrinfo server_in, *servinfo;
	struct sockaddr_storage address;
	socklen_t addr_len = -1 ;
	if(argc != 4)
	{
		printf("enter the valid arguments.");
		exit(FAILURE);
	}
	else
	{
		if(NULL == argv[1])
		{
			printf("Please specify the server's IP Address\n");
			exit(FAILURE);
		}
		else
		{
			server = argv[1];
		}
		if(NULL == argv[2])
		{
			printf("Please specify the server's Port No \n");
			exit(FAILURE);
		}
		else
		{
			port = argv[2];
		}
		if(NULL == argv[3])
		{
			printf("Please specify the unique ID.\n");
			exit(FAILURE);
		}
		else
		{
			strcpy(uniqueid, argv[3]);
		}
		memset(&server_in, 0, sizeof(server_in));
		server_in.ai_family = AF_UNSPEC;
		server_in.ai_socktype = SOCK_DGRAM;
		rv = getaddrinfo(server,port, &server_in, &servinfo);
		if(0 != rv)
		{
			fprintf(stderr, "CLIENT: getaddrinfo: %s\n", gai_strerror(rv));
			exit(FAILURE);
		}
		else
		{
			
			sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
			if(sockfd == -1)
			{
				perror("socket error");
				close(sockfd);
				exit(FAILURE);
			}
			 
			printf("%s",uniqueid);
			numbytes = sendto(sockfd, uniqueid, strlen(uniqueid), 0, servinfo->ai_addr, servinfo->ai_addrlen );
			if(-1 == numbytes)
			{
				perror("CLIENT: error in send ID \n");
				exit(FAILURE);
			}
		}
		if(servinfo == NULL)
		{
			printf("bind error");
			close(sockfd);
			exit(FAILURE);
		}
		
		printf("*****************************************************\n");
		printf("*****************************************************\n");
		printf("WELCOME CLIENT: %s\n", uniqueid);
		printf("*****************************************************\n");
		printf("*****************************************************\n");

		while(1)
		{
			printf("press 1 to download a file.\n");
			printf("press 2 to upload a file.\n");
			printf("press 3 to exit.\n");
			scanf("%d",&option);
			switch(option)
			{
				case 1:
						DownloadFromServer(sockfd, address, addr_len, servinfo);
						break;
				case 2:
						UploadToServer(sockfd, address, servinfo, addr_len);
						break;
				case 3:
						close(sockfd);
						exit(FAILURE);
				default:
						printf("\n wrong choice entered.\n");
			}
		}
	}
	return 0;
}

