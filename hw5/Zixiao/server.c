#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "md5sum.h"
#include "restart.h"

//Zixiao Wang and Jia Zhang

// error exit sub-routing
void error(char *msg)
{
    perror(msg);
    exit(1);
}


// main function
int main(int argc, char **argv)
{
	 /*
	  * argv[1]: <port: port on which server is listening for connections>
	  * argv[2]: <input directory>
	  */
	 
	 
    // int i;
    // unsigned char sum[MD5_DIGEST_LENGTH];
    char inputdir[200];
    
    // Copy the input agrument to be the input directory
    strcpy(inputdir, argv[2]);
    fprintf(stderr, "Input is: %s\n", inputdir);
    
    // initialing catalog path
    char catalog_path[200];

    //----------------------network initilization-------------------------- 
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
	// limit 3 inputs
    if (argc != 3) {
        fprintf(stderr, "Incorrect arguments..\n");
        fprintf(stderr, "Usage: %s <PortNumber> <InputDirectory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //-----------------Search the directory and compute MD5, generate csv---------------
    FILE* fp;
    sprintf(catalog_path, "%s/%s", getcwd(NULL, 0), "catalog.csv");
    // Open catalog
	fp = fopen(catalog_path, "w");
    fprintf(fp, "fileName,size,checksum\n");
    fprintf(stderr, "Writing FileName,Size,Checksum\n");    // ##########################
    // Close the fp
	fclose(fp);    
    // Read Dir and retur the number of files
    int numberOfFile = ReadDir(inputdir, catalog_path);
    fprintf(stderr, "\nthe total number of file is: %d \n", numberOfFile);
    
    //-----------------Load csv information into structure-------------------
	 struct entry FileInfo[numberOfFile];
	 LoadCatalog(catalog_path, numberOfFile, FileInfo);



     // Network communication set up
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
     if (listen(sockfd,5) == -1){
	  	   printf("listen fail!");    
     }
     clilen = sizeof(cli_addr);
     if((newsockfd = accept(sockfd, (struct sockaddr *)NULL, NULL)) < 0)
         error("ERROR on accept");	  
	  
	  //-----------------------transfer catalog----------------------
	  // open catalog.csv
	  fp = fopen(catalog_path, "r");
	// initialize the transfer flag
	  bool End_of_File = false;
	  // Transfering catalog file loop 
     while (!End_of_File) {
         bzero(buffer,256);
         
         n = fread(buffer+1, 1, 255, fp);
		   
			fprintf(stderr, "n: %d\n", n);		   
			fprintf(stderr, "buffer: %s\n", buffer+1);		   
		   
         if (n == 255)
         {
         	buffer[0] = '0';
         }else if(n < 255)
         {
				buffer[0] = '1';
				End_of_File = true;
				printf("reach to the end of file\n");         
         }
         
         n = write(newsockfd, buffer, 256);
         if (n < 0) error("ERROR writing to socket");
         printf("Here is the message: %s\n",buffer+1);
     }
     // close the fp after transfering
     fclose(fp);
     
     
     
     //-----------------------transfer image----------------------
	  char c[1];
	  // initialize the transfer flag
     bool Finish_Transfer = false;
     int index = -1;		// index for file to transfer
     while(!Finish_Transfer)
     {
			bzero(buffer,256);
			// wait for reading insteructions
     		n = read(newsockfd,buffer,256);
			fprintf(stderr, "n: %d\n", n);     		
     		// reading the file by index
     		if (n < 0)
     		{	
     			error("ERROR reading from socket");
     		}else {
     			// invalid input buffer will be handled in client
				index = atoi(buffer);     		
     		}
     		
     		fprintf(stderr, "index: %d\n", index);
     		
     		
     		if (index == 0)
     		{
				 // finish transfer
				 printf("finish transfer\n");
				 Finish_Transfer = true;
     		}else{
     			
     			 char imagepath[512] = {};
				// Search for the file, get path
     			 Find_imagepath(inputdir, FileInfo, index, imagepath);
     			 fprintf(stderr, "ImagePath: %s\n\n", imagepath);
     			 // read file
     			 fp = fopen(imagepath, "r");
     			 // transfer loop
     			 int i = 0;
				for (i = 0; i < FileInfo[index-1].size; i++)
     			{
     			 	 
     			 	 bzero(c,1);
        			 n = fread(c, 1, 1, fp);

         		 int k;
    			    k = write(newsockfd, c, 1);
    			    if (k < 0) error("ERROR writing to socket");

     			 }
     			 // close file
     			 fclose(fp);
     			 
     		}
     
     
     }
     // finish the socket
     close(newsockfd);
     // return 0;

    exit(EXIT_SUCCESS);
    
    
}









