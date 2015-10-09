/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

typedef struct 
{
	char item1[50];
	char item2[50];
	char item3[50];
} replyItems;


//----------------------Helpful Functions Here----------------------

int ReadDir(const char *dir)
{
	int file_count = 0;
	int temp = 0;
	struct dirent* ptr;
	DIR* srcdir = opendir(dir);

	if (srcdir == NULL)
	{
		perror("opendir");
		return -1;
	}

	while((ptr = readdir(srcdir)) != NULL)							// traverse done
	{
		char* d_name;
		d_name = ptr -> d_name;
		printf("\n/////////////////////////START SCANNING//////////////////////////\n");

		// Open catlog.csv file
		FILE* fp;
		char html_path[100] = {};
		sprintf(html_path, "%s/%s/%s", getcwd(NULL, 0), inputdir, "catlog.csv");
		fp = fopen(html_path, "w");

		// Open directory
		if (ptr -> d_type & DT_DIR)									// check whether it is a directory
		{
			if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
			{
				char path[100];
				sprintf(path, "%s/%s", dir, d_name);
				temp = ReadDir(path);
				file_count += temp;
			}
		}else{
			// Check if the type matchs
			if (isPNG(d_name) || isGIF(d_name) || isTIFF(d_name)){
				
				fseek(fp, 0L, SEEK_END);
				int sz = ftell(fp);
				fseek(fp, 0L, SEEK_SET);
				fprintf(fp, 


			}else{
				printf("The file is not png/gif/tiff file, skipped it.\n");
			}
			
		}
		
	}
	closedir(srcdir);
	return file_count;
}

bool isPNG(char *filename)
{
	int len = strlen(filename);
	int i = 0;
	char suffixOfFile[4];
	for (i = 0; i < 4; i++)
	{
		suffixOfFile[i] = filename[len - 4 + i];				// get last 4 character(suffix of file)
	}
	fprintf(stderr,"The suffix of file is: %s\n", suffixOfFile);
	return !(strncmp(suffixOfFile, ".png", 4));
}

bool isGIF(char *filename)
{
	int len = strlen(filename);
	int i = 0;
	char suffixOfFile[4];
	for (i = 0; i < 4; i++)
	{
		suffixOfFile[i] = filename[len - 4 + i];				// get last 4 character(suffix of file)
	}
	fprintf(stderr,"The suffix of file is: %s\n", suffixOfFile);
	return !(strncmp(suffixOfFile, ".gif", 4));
}

bool isTIFF(char *filename)
{
	int len = strlen(filename);
	int i = 0;
	char suffixOfFile[5];
	for (i = 0; i < 5; i++)
	{
		suffixOfFile[i] = filename[len - 5 + i];				// get last 5 character(suffix of file)
	}
	fprintf(stderr,"The suffix of file is: %s\n", suffixOfFile);
	return !(strncmp(suffixOfFile, ".tiff", 5));
}

//----------------------Main Functions Here----------------------


int main(int argc, char *argv[])
{
	//----------------------Initiating Variables----------------------

     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
	 char reply[256];
     struct sockaddr_in serv_addr, cli_addr;
	 replyItems replies;
	 strcpy(replies.item1, "sirius");
	 strcpy(replies.item2, "procyon");
	 strcpy(replies.item3, "persei");
     int n, itemRequestNum;

    // Additional Variables
    char inputdir[100];	 
	strcpy(inputdir, argv[2]);

	//----------------------Code Begins Here----------------------
    // Check for inputs
     if (argc < 2) 
     {
         fprintf(stderr,"ERROR, no port provided\n");
         fprintf(stderr, "Usage: %s\tport\tdirectory\n", argv[0]);
         exit(1);
     }

     // Read Directory
     int numberOfFile = ReadDir(inputdir),

     //	Create the socket, ADDRESS FAMILY, TCP
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
				 &clilen);
	 if (newsockfd < 0) 
		 error("ERROR on accept");

	 while (1) 
	 {
		 bzero(buffer,256);
		 n = read(newsockfd,buffer,255);
		 if (n < 0) error("ERROR reading from socket");
		 printf("Here is the message: %s\n",buffer);
		 if (strstr(buffer, "disconnect")) 
		 {
			 printf("Recieved disconnect command, closing socket\n");
			 close(sockfd);
			 return 0;
		 }
		 itemRequestNum = atoi(buffer);
		 if (itemRequestNum < 1 || itemRequestNum > 3) 
		 {
			 strcpy(reply, "Please Enter a number between 1 and 3");
		 }
		 else if (itemRequestNum == 1) 
		 {
			 strcpy(reply, replies.item1);
		 }
		 else if (itemRequestNum == 2) 
		 {
			 strcpy(reply, replies.item2);
		 }
		 else 
		 {
			 strcpy(reply, replies.item3);
		 }
		 n = write(newsockfd, reply, strlen(reply));
		 if (n < 0) error("ERROR writing to socket");
	 }
	 close(sockfd);
	 return 0; 
}
