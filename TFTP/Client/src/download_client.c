/************************************************************************
				INCLUDING USER DEFINED HEADER FILE
**************************************************************************/

#include<clientutil.h>

/************************************************************************
			Function name     :  file_size

			Description       :

			Parameters        :

			Return value      :
************************************************************************/
int WriteInFile(char *buff, int c_write, FILE *fp, char *last_recv_pkt)
{
	c_write = strlen(buff+4);
    fwrite(buff+4, sizeof(char), c_write, fp);
    strcpy(last_recv_pkt, buff);
    return SUCCESS;
}

/************************************************************************
			Function name     :  file_size

			Description       :

			Parameters        :

			Return value      :
************************************************************************/
int SendAck(int sockfd, char* buff, struct sockaddr_storage address, socklen_t addr_len)
{
	char block[MAX] = {0};
	char * m_ack = NULL;
    int numbytes = -1;
    strncpy(block, buff+2, 2);
    block[VAL-1] = '\0';
    m_ack = create_ack_packet(block);
    numbytes = sendto(sockfd, m_ack, strlen(m_ack), 0, (struct sockaddr*)&address, addr_len );
    if(-1 == numbytes)
    {
            perror("CLIENT ACK: sendto");
            exit(1);
    }
    return SUCCESS;
}


/************************************************************************
			Function name     :  file_size

			Description       :

			Parameters        :

			Return value      :
************************************************************************/
int DownloadFromServer(int sockfd, struct sockaddr_storage address, socklen_t addr_len, struct addrinfo *p)
{
    FILE *fp = NULL;
    char filename[50] = {0};
    int numbyte = -1,
		c_write = -1,
		f_write = -1;

    char* message = NULL;
    char buff[MAX_READ_LEN];
    char last_recv_pkt[MAX_READ_LEN];
    char last_sent_ack[MAX];


    printf("Enter the filename you want to download.\n");
   	scanf("\n%s", filename);
   	message = create_read_packet(filename);
    strcpy(last_sent_ack, message);
    numbyte = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen);
    if(-1 == numbyte)
    {
            perror("error in send request.");
            exit(FAILURE);
    }
    printf("File download request sent.\n");

    fp = fopen(filename, "wb");
    if(NULL == fp)
    {
            fprintf(stderr, "File can not open\n");
            exit(FAILURE);
    }

    do
    {
        addr_len = sizeof address;
        numbyte = recvfrom(sockfd, buff, MAX_READ_LEN, 0, (struct sockaddr *)&address, &addr_len);
        if(-1 == numbyte)
        {
                perror("error in receive request");
                exit(FAILURE);
        }
        printf("CLIENT: packet is %d bytes long\n",numbyte);
        printf("CLIENT: packet contains %s\n", buff);
        buff[numbyte] = '\0';

        if(buff[0] == '0' && buff[1] == '5')
        {
                fprintf(stderr, "CLIENT: got error packet: %s\n", buff);
                exit(FAILURE);
        }

        // Write in file
        f_write = WriteInFile(buff, c_write, fp, last_recv_pkt);
        if(-1 == f_write)
        {
                printf("file not written.\n");
                exit(FAILURE);
        }
        int ack_status = SendAck(sockfd, buff, address, addr_len);
        if(-1 == ack_status)
        {
                printf("Acknowledgment not received\n");     /////////////////////
        }

    }
    while(c_write == MAX_READ_LEN);

    printf("NEW FILE: %s SUCCESSFULLY MADE\n", filename);
    fclose(fp);

	
    return SUCCESS;
}




