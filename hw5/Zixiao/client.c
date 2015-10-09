#include "restart.h"

// Need to check 1. non-image file  2. sub-directory
void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	/*
	 * argv[1]: <IPv4 address: dotted-decimal format which indicate server machine>
	 * argv[2]: <port: port on which server is listening for connections>
	 * argv[3]: <optional extension> 
	 */
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	char buffer[256];
	// usage check
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

	//---------------------------pull out argv--------------------
	// initilize port number var			argv[2]
	portno = atoi(argv[2]);


	// create socket						
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");


	// get server
	server = gethostbyname(argv[1]);		//argv[1]
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	

	//---------------set the serv_addr to zero(initilization)-----------------
	bzero((char *) &serv_addr, sizeof(serv_addr));

	// set sin_family
	serv_addr.sin_family = AF_INET;			// AF_INET represents TCP/IP

	// set sin_addr:   copy server address to sin_addr
	bcopy((char *)server->h_addr, 
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);

	// sin_port:   convert portno to network byte order and copy to struct serv_addr
	serv_addr.sin_port = htons(portno);

	// connection with the socket
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	
	//###############################################################################
	//###############################################################################
	//---------------------------------------image directory----------------------------------------
	
	DIR * output = NULL;
	char outputdir[200] = {};
	sprintf(outputdir, "%s/images", getcwd(NULL, 0));
	//printf("%s\n", outputdir);

	//printf("\n---------------create output dir-----------------\n");
	if ((output = opendir(outputdir)) == NULL){
		mkdir(outputdir, 0700);
		//printf("Outputdir: %s\n\n", out);									// #################
	}
	closedir(output);
	
	
	//-------------------------create catalog.csv file----------------------------------
	FILE* fp;
	char catalog_path[200] = {};
	sprintf(catalog_path, "%s/%s", outputdir, "catalog.csv");
	fp = fopen(catalog_path, "w");
	
	bool End_of_File = false;
	
	//-------------------------download catalog.csv file------------------------------------
	
	while(!End_of_File)
	{
		/* buffer[0]: flag represents the whether reach the end of file 
		 * 
		 */
		bzero(buffer,256);
		n = read(sockfd,buffer,256);
		fprintf(stderr, "%s\n", buffer);
		if (n < 0) error("ERROR reading from socket");
		
		// write to the catalog.csv file
		if (buffer[0] == '1')			// reach to the end of catalog.csv
		{
			End_of_File = true;
			
		}else if (buffer[0] == '0')	// not reach to the end of catalog.csv
		{
			
		}
		
		// write buffer[1]-[255] to local catalog.csv
		char temp[255];
		bzero(temp, 255);
		strncpy(temp, buffer+1, 255);		// copy buffer[1]-[255] to temp
		fprintf(fp, "%s", temp);
	}
	fclose(fp);
		
		
	printf("===============================\nDumping contents of catalog.csv\n");
	// read downloaded catalog.csv and print it to screen
	int numFile = 0;			// ######  number of file 
	ReadCatalog(catalog_path, &numFile);
	
	struct entry FileInfo[numFile];
	
	LoadCatalog(catalog_path, numFile, FileInfo);

	//printf("numFile: %d\n", numFile);

	/*
	int k = 0;
	for (k = 0; k < numFile; k++)
	{
		printf("Filename: %s\n", FileInfo[k].filename);
		printf("File length: %d\n", FileInfo[k].namelength);
		printf("size: %d\n", FileInfo[k].size);
		printf("checksum: %s\n", FileInfo[k].checksum);
		
	}*/
	
	printf("===============================\n");
	
	
	//###############################################################################
	//###############################################################################
	// assign a flag to every file, indicating if transfered
	int File_Flag[numFile]; 
	int p = 0;
	for (p = 0; p < numFile; p++)
	{
		File_Flag[p] = 0;
	}
	
	int index = 0;		// index for file to download
	char c[1];
	
	if (argc == 3)					// Interactive mode
	{
		bool Finish_Download = false;
		
		// While loop for downloading
		while(!Finish_Download)
		{
			bzero(buffer,256);
			fprintf(stderr, "Enter ID to download (0 to quit): ");

				
				//------------------Interactive mode: write---------------------------
				bzero(buffer,256);
				fgets(buffer,256,stdin);
				// 
				index = atoi(buffer);			// select file to download
				
				// Check condition, exit
				if (index == 0 && strstr(buffer, "0"))
				{
					// write user input to sockfd
					n = write(sockfd,buffer,strlen(buffer));
					if (n < 0) 
						error("ERROR writing to socket\n");
					Finish_Download = true;
					// Check condition, n < 0
				}else if (index < 0 || index > numFile)
				{
					error("ERROR selecting file\n");
					// Check condition, n > number of files
				}else if (index > 0 && index <= numFile)
				{
					// write user input to sockfd
					n = write(sockfd,buffer,strlen(buffer));
					if (n < 0) 
						error("ERROR writing to socket\n");
					// Set corresponding index
					File_Flag[index-1] = 1;
					// reserve space for file path
					char file_path[512] = {};
					printf("index-1: %d\nnumFile: %d\n", index-1, numFile);
					sprintf(file_path, "%s/%s", outputdir, FileInfo[index-1].filename);
					//Open file to write
					fp = fopen(file_path, "w");
					
					int j = 0;
					for(j = 0; j < FileInfo[index-1].size; j++)
					{
						//-----------------Download image file---------------------------------
						bzero(c,1);
						n = read(sockfd,c,1);
						if (n < 0) error("ERROR reading from socket");
						
						// write to the image file
						fwrite(c, 1, 1, fp);						
						
					}
					// finish writing
					fclose(fp);
					// note the number of downloaded
					numDownload++;
					
					printf("Downloaded %s\n", FileInfo[index-1].filename);
				}

			End_of_File = false;
		}

		
	}else if(argc == 4)				// Passive Mode
	{		
		fprintf(stderr, "Running in passive mode..Downloading ‘%s’ files..\n", argv[3]);
		 
		bool Finish_Download = false;
		
		while(!Finish_Download) {
			//-----------------Download image file---------------------------------
			
			// check which type of file is required
			// ---------------------png----------------------
			if (!strcmp(argv[3], "png"))
			{
				int i = 0, j = 0;
				//choose all files that is png 
				for (i = 0; i < numFile; i++)
				{
					//if yes, request from the server
					if (isPNG(FileInfo[i].filename))
					{
						char temp[4] = {};
						sprintf(temp, "%d",i+1);
						n = write(sockfd,temp, 4);
						if (n < 0) 
							error("ERROR writing to socket\n");
							// set the file flag
						File_Flag[i] = 1;
						
						char file_path[512] = {};
						//printf("index-1: %d\nnumFile: %d\n", i, numFile);
						sprintf(file_path, "%s/%s", outputdir, FileInfo[i].filename);
						fp = fopen(file_path, "w");

						for(j = 0; j < FileInfo[i].size; j++)
						{
							bzero(c,1);
							n = read(sockfd,c,1);
							if (n < 0) error("ERROR reading from socket");
							
							// write to the image file
							fwrite(c, 1, 1, fp);
						}
						// close file, add download number
						fclose(fp);
						numDownload++;
						//print on screen
						printf("Downloaded %s\n", FileInfo[i].filename);
					}
				}
				
				char temp[4];
				bzero(temp, 4);
				sprintf(temp, "%d",0);
				n = write(sockfd,temp, 4);
				if (n < 0) 
					error("ERROR writing to socket\n");
				// end the transcration
				Finish_Download = true;
				
				
			// --------------------tiff--------------------
			}else if(!strcmp(argv[3] ,"tiff"))
			{
				int i = 0, j =0;
				//choose all files that is tiff
				for (i = 0; i < numFile; i++)
				{
					//if yes, request from the server
					if (isTIFF(FileInfo[i].filename))
					{
						
						char temp[4] = {};
						sprintf(temp, "%d",i+1);
						n = write(sockfd,temp, 4);
						if (n < 0) 
							error("ERROR writing to socket\n");
							// set the file flag
							File_Flag[i] = 1;
						
						char file_path[512] = {};
						//printf("index-1: %d\nnumFile: %d\n", i, numFile);
						sprintf(file_path, "%s/%s", outputdir, FileInfo[i].filename);
						fp = fopen(file_path, "w");

						for(j = 0; j < FileInfo[i].size; j++)
						{
							bzero(c,1);
							n = read(sockfd,c,1);
							if (n < 0) error("ERROR reading from socket");
							
							// write to the image file
							fwrite(c, 1, 1, fp);
						}
						// close file, add download number
						fclose(fp);
						numDownload++;
						//print on screen
						printf("Downloaded %s\n", FileInfo[i].filename);
					}
				}
				char temp[4];
				bzero(temp, 4);
				sprintf(temp, "%d",0);
				n = write(sockfd,temp, 4);
				if (n < 0) 
					error("ERROR writing to socket\n");
				
				// end the transcration
				Finish_Download = true;
			// ------------------------gif------------------------------
			}else if(!strcmp(argv[3], "gif"))
			{
				int i = 0, j =0;
				//choose all files that is gif
				for (i = 0; i < numFile; i++)
				{
					//if yes, request from the server
					if (isGIF(FileInfo[i].filename))
					{						
						char temp[4] = {};
						sprintf(temp, "%d",i+1);
						n = write(sockfd,temp, 4);
						if (n < 0) 
							error("ERROR writing to socket\n");
							// set the file flag
						File_Flag[i] = 1;
						
						char file_path[512] = {};
						//printf("index-1: %d\nnumFile: %d\n", i, numFile);
						sprintf(file_path, "%s/%s", outputdir, FileInfo[i].filename);
						fp = fopen(file_path, "w");

						for(j = 0; j < FileInfo[i].size; j++)
						{
							bzero(c,1);
							n = read(sockfd,c,1);
							if (n < 0) error("ERROR reading from socket");
							
							// write to the image file
							fwrite(c, 1, 1, fp);
						}
						// close file, add download number
						fclose(fp);
						numDownload++;
						//print on screen
						printf("Downloaded %s\n", FileInfo[i].filename);
					}
				}
				char temp[4];
				bzero(temp, 4);
				sprintf(temp, "%d",0);
				n = write(sockfd,temp, 4);
				if (n < 0) 
					error("ERROR writing to socket\n");
				// end the transcration
				Finish_Download = true;
			}
		}
				
	
	}

	close(sockfd);
	
	//fprintf(stderr, "number of downloaded file: %d\n", numDownload);

	printf("Computing checksums for downloaded files..\n");
	//-------------------------------Compute checksums--------------------------------	
	
	int i = 0, j = 0;
	unsigned char sum[numFile][MD5_DIGEST_LENGTH];
	char md5sum[numFile][MD5_DIGEST_LENGTH*2+1];
	bool result_check[numFile];
	
	checksum(outputdir, sum, numFile, FileInfo, File_Flag);

	for (i = 0; i < numFile; i++)
	{
		for (j = 0; j < MD5_DIGEST_LENGTH; j++)
		{
			char temp[2];
			sprintf(temp, "%02x", sum[i][j]);
			strncpy(md5sum[i]+2*j, temp, 2);
			md5sum[i][MD5_DIGEST_LENGTH*2] = '\0';
		}
		
		//printf("\n");
	}
	
	// !!!     compare md5       !!!!
	for (i = 0; i < numFile; i++)
	{
		if (File_Flag[i] != 0)
		{
			//---start compare---
			bool match = true;
			FileInfo[i].checksum[32] = 0;
			//fprintf(stderr, "md5sum[%d]: %s, length: %d\n", i, md5sum[i], strlen(md5sum[i]));
			//fprintf(stderr, "FileInfo[%d].checksum: %s, length: %d\n\n", i, FileInfo[i].checksum, strlen(FileInfo[i].checksum));
			if (strcmp(md5sum[i], FileInfo[i].checksum))
			{
				match = false;
			}
			
			//fprintf(stderr, "compare result[%s]: %d\n", FileInfo[i].filename, strcmp(md5sum[i], FileInfo[i].checksum));
			
			if (match)
			{
				result_check[i] = true;
			}else{
				result_check[i] = false;
			}
		}
	}
	
	
	//----------------------------------create html------------------------------------
	char html_path[200] = {};
	fprintf(stderr, "Generating HTML file..\n");
	sprintf(html_path, "%s/download.html", getcwd(NULL, 0));
	Write_to_html(html_path, result_check, numFile, outputdir, FileInfo, File_Flag);
	
	
	
	//----------------------------------exit-------------------------------------------
	fprintf(stderr, "Exiting..\n");






	
	return 0;
}
