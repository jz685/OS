/* Information:
 * CSci 4061 Spring 2015 Assignment 3
 * Name1 = Zixiao Wang
 * Name2 = Jia Zhang
 * StudentID1 = 5107014
 * StudentID2 = 4388046
 * Commentary = description of the program, problems encountered while coding, etc.
 * 			Assumption: 1) offsets will be aligned at 512 bytes
 * 						2) file name will not be larger than 15 characters.
 * 						3) input files exist and are readable.
 * 						4) none of the input file names will have any spaces in the name.
 * 						5) total file count won’t exceed 128 or the total size would not exceed the size of the mini file system.
 * 						6) There are no soft / hard links in the directory tree. There are only files.
 */

#include <stdio.h>
#include "mini_filesystem.h"
#include <dirent.h>
#include "restart.h"
//#include <cstdio.h>

//------------------------------------------helper function------------------------------------

bool isJPG(char *filename)
{
	int len = strlen(filename);
	int i = 0;
	char suffixOfFile[4];
	for (i = 0; i < 4; i++)
	{
		suffixOfFile[i] = filename[len - 4 + i];				// get last 4 character(suffix of file)
	}
	fprintf(stderr,"The suffix of file is: %s\n", suffixOfFile);
	return !(strncmp(suffixOfFile, ".jpg", 4));
}

int writefileback (int inode_number, char *outputdir)
{
	char* filenameTemp;
	int len, index;
	pid_t childpid;
	
	filenameTemp = Directory_Structure[inode_number].Filename;
	fprintf (stderr, "filenameTemp is %s\n", filenameTemp);
	
	len = Inode_List[inode_number].File_Size;
	fprintf (stderr, "len is %d\n", len);
	
	char* to_read = (char*) malloc(len );	
	//Read_File(inode_number, 0, length, to_read);
	FILE *fw;
	
	index = Open_File(filenameTemp);
	
	char writefilepath[100];
	char newpath[100];
	sprintf(writefilepath, "%s/%s", outputdir, filenameTemp);
	sprintf(newpath, "%s%s", writefilepath, "encd");
	fprintf (stderr, "writefilepath is %s\n", writefilepath);
	
	if (Read_File( inode_number, 0, len, to_read) > 0)
	{
		//fprintf(stderr,"%s\t", to_read);
		fw = fopen(newpath, "w");
		if(fw != NULL)
		{
			int i = 0;
			while(i < len)
			{
				fputc(to_read[i], fw);
				i++;
			}
		}else{
			fprintf(stderr,"Oops, can't open\n");
		}
		fclose(fw);
	}
	
	
	childpid = fork();	
	//error handling
	if (childpid == -1) 
	{
		perror("Failed to fork");
		return 1;
	}
	//child code
	if (childpid == 0) 
	{
		fprintf(stderr, "--------------I am the child----------------\n");
		//char filepath[100];
		//char newpath[100];
		//sprintf(filepath, "%s/%s", dir, d_name);
		//sprintf(newpath, "%s%s", filepath, "encd");
		fprintf(stderr,"writefilepath is %s \t newpath is %s\t\n", writefilepath, newpath);
		char cmd[200];
		strcpy(cmd, "base64");
		strcat(cmd, " -d ");
		strcat(cmd, newpath);
		strcat(cmd, " > ");
		strcat(cmd, writefilepath);
		execlp("/bin/bash", "bash", "-c", cmd, NULL);
		//execl("/usr/bin/base64", "base64", filepath, ">", newpath);
	}else{
		//wait
		while(r_wait(NULL) > 0)
		{
			fprintf(stderr, "Parent Wait\n");
		}
		fprintf(stderr, "--------------I am the parent----------------\n");
	}

	return 0;
}

//Read all files in the input directory and copy all files to mini file system
int ReadDir(const char *dir)
{
	int file_count = 0;
	int temp = 0;
	struct dirent* ptr;
	DIR* srcdir = opendir(dir);
	pid_t childpid;

	if (srcdir == NULL)
	{
		perror("opendir");
		return -1;
	}

	while((ptr = readdir(srcdir)) != NULL)										// traverse done
	{
		char* d_name;
		d_name = ptr -> d_name;

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
			printf("\n/////////////////////////START//////////////////////////\n%s/%s\n", dir, d_name);
			if (isJPG(d_name)){
				//---------------Jia------------------
				//fork
				
				childpid = fork();
				
				//error handling
				if (childpid == -1) 
				{
					perror("Failed to fork");
					return 1;
				}
				//child code
				if (childpid == 0) 
				{
					fprintf(stderr, "--------------I am the child----------------\n");
					char filepath[100];
					char newpath[100];
					sprintf(filepath, "%s/%s", dir, d_name);
					sprintf(newpath, "%s%s", filepath, "encd");
					fprintf(stderr,"filepath is %s \t newpath is %s\t\n", filepath, newpath);
					char cmd[200];
					strcpy(cmd, "base64 ");
					strcat(cmd, filepath);
					strcat(cmd, " > ");
					strcat(cmd, newpath);
					execlp("/bin/bash", "bash", "-c", cmd, NULL);
					//execl("/usr/bin/base64", "base64", filepath, ">", newpath);
				}else{
					//wait
					while(r_wait(NULL) > 0)
					{
						fprintf(stderr, "Parent Wait\n");
					}
					fprintf(stderr, "--------------I am the parent----------------\n");
					//---------------end jia---------------
					//--------------------Create---------------------------
					int f = Create_File(d_name);
					//printInode(Inode_List[Superblock.next_free_inode-1]);
					if(f == -3)
					{
						printf("File could not be added to Directory.\n");
						continue;
					}else if (f == -2)
					{
						printf("File could not be written to the new inode.\n");
						continue;
					}else if (f == -1)
					{
						printf("File already existed in directory.\n");
						continue;
					}else{
						//printf("ptr->d_name: %s\n", d_name);			// #################
						file_count++;
						//------------------------------Copy image to mini file system-------------------------------------------
						FILE *fp;
						int ch;
						//fprintf(stderr, "d_name is: %s\n", d_name);
						char filepath[100];
						sprintf(filepath, "%s/%s", dir, d_name);
						//--------------jia--------------------
						char newpath[100];
						sprintf(newpath, "%s%s", filepath, "encd");
						fprintf(stderr,"filepath is %s \t newpath is %s\t\n", filepath, newpath);
						fp = fopen(newpath, "r");
						///--------------end jia----------------
						
						//fp = fopen(filepath, "r");
						if (fp == NULL) {
							fprintf(stderr, "Cannot open %s/%s .....\n", dir, d_name);
							break;														// what we should do if could not read file!!!!!!!!!!!!!!
						}else{
							//------------------------------Copy file to mini_filesystem---------------------------------------
							fseek(fp, 0L, SEEK_END);
							int sz = ftell(fp);
							fseek(fp, 0L, SEEK_SET);
							char* to_write = (char*) malloc(sizeof(char)*sz);
							int i = 0;
							while( (ch = fgetc(fp)) != EOF )
							{
								to_write[i] = ch;
								//fprintf(stderr, "%c\t", ch);
								i++;
							}
							//fprintf(stderr, "\nto_write is: %s\n", to_write);
							Write_File(Search_Directory(d_name), 0, to_write, sz);
							fprintf(stderr,"---------------\n");
							printInode(Inode_List[Superblock.next_free_inode-1]);
							fprintf(stderr,"---------------\n/////////////////////////END///////////////////////////\n");
						}
						
					}
				}
			}else{
				printf("The file is not JPG file, skipped it.\n");
			}
			
		}
		
	}
	closedir(srcdir);
	return file_count;
}


//------------------------------------------main function--------------------------------------
int main(int argc, char **argv)
{
	//--------------------------Usage Test--------------------------------------------
	if (argc != 4) 
	{
		fprintf (stderr, "Usage: ./test output_dir input_dir logfile\n");
		fprintf (stderr, "For example: $ ./test output_dir input_dir logfile\n");
		return 1;
	}
	
	//--------------------------initiating variables--------------------------------------------
	/* argc = 3, 
	 * argv[0] = test: name of final executable file
	 * argv[1] = Input_directory
	 * argv[2] = Output_directory
	 * argv[3] = Log_filename 
	 */
	char inputdir[100];
	char outputdir[100];
	char log_filename[100]; 
	//pid_t childpid;
	 
	strcpy(inputdir, argv[1]);
	strcpy(outputdir, argv[2]);
	strcpy(log_filename, argv[3]);
	//printf("inputdir name: %s\n", inputdir);							// #################
	//printf("outputdir name: %s\n", outputdir);						// #################
	//printf("Log_filename, : %s\n\n", log_filename);					// #################
	

	
	/* 1) Your program will take as input the name of the input_directory, an output_directory and a log_filename.
	 * 		a) Intput_directory – Check if it exists, if it doesn’t, you should exit with appropriate error message.
	 * 		b) Output_directory – Check if it exists, if it does, use that, if it doesn’t, create one.
	 * 		c) Log_filename – Check if it exists, if it does, remove and create a new one. 
	 */
	
	//---------------------------------------Initilize filesystem----------------------------------------
	
	if (Initialize_Filesystem(log_filename))						// something wrong with initialization
	{
		return -1;
	}
	
	//---------------------------------------Output directory----------------------------------------
	
	DIR * output = NULL;
	printf("\n---------------create output dir-----------------\n");
	if ((output = opendir(outputdir)) == NULL){
		char out[200];
		sprintf(out, "%s/%s", getcwd(NULL, 0), outputdir);
		mkdir(out, 0700);
		printf("Outputdir: %s\n\n", out);									// #################
	}
	closedir(output);
	
	
	//---------------------------------------Input directory reading and Creating----------------------------------------
	
	int numberOfFile = ReadDir(inputdir), i = 0;
	printf("\nThe number of file in inputdir: %d\n\n---------------------------------\n", numberOfFile);		// #################
	for (i = 0; i < numberOfFile; i++)
	{
		printInode(Inode_List[i]);
		printf("\n");
		
	}

	//---------------------------------------Open file copy to output dir-------------------------------------------

	for (i = 0; i < Superblock.next_free_inode ; i++)
	{
		fprintf(stderr, "--------------START--------------\n");
		writefileback (i, outputdir);
		
		fprintf(stderr, "---------------END---------------\n");
	}
	
	//----------------------------------Delete extra generated file--------------------------------------
	if (!(deleteENCD(outputdir)))
	{
		//add success deleting all encd file information here
	}else{
		//add error information here
	}
	
	//----------------------------------Create thumbnails------------------------------------------
	genThumbs(outputdir);
	
	//---------------------------------------Create html-------------------------------------------
	createHTML(outputdir);
	
	
	
	
	
	
	
	
	
	
	
	
	//---------------------------------------------test every function------------------------------------------
	

	/* --------------search_Directory pass test, should print 0------------
	char *b = "test_search";
	strcpy(Directory_Structure[0].Filename,b);
	printf("Directory_Structure[0].Filename: %s\n", Directory_Structure[0].Filename);
	int a2 = Search_Directory(b);
	printf("Search_Directory return value: %d\n", a2);*/
	
	/* --------------Add_to_Directory pass test----------------------------
	char *b = "nihaoma";
	int k = 15;
	Add_to_Directory(b, k);
	*/
	
	/* --------------Inode_Read pass test----------------------------------
	Inode_List[12].Inode_Number = 15;
	Inode_List[12].User_Id = 15;
	Inode_List[12].Group_Id = 15;
	Inode_List[12].File_Size = 15;
	Inode_List[12].Start_Block = 15;
	Inode_List[12].End_Block = 15;
	Inode_List[12].Flag = 15;
	Inode test = Inode_Read(12);
	printInode(test);
	
	Inode test2 = Inode_Read(128);
	printInode(test2);*/
	
	/* --------------Inode_write pass test-----------------------------------
	Inode b = {
		.Inode_Number = 15,
		.User_Id = 15,
		.Group_Id = 15,
		.File_Size = 15,
		.Start_Block = 15,
		.End_Block = 15,
		.Flag = 15};
	
	Inode_Write(5,b);
	printInode(5);
	printf("Inode does not exist situation return: %d\n", Inode_Write(129,b));*/
	
	/* --------------Block_Read pass test------- still have problem unsolved
	Disk_Blocks[5] = (char *)calloc(1, 512);
	strcpy(Disk_Blocks[5], "test for read block!!");
	printBlock(Disk_Blocks[5]);
	char nihao[22];
	printf("nihao : %d\n", sizeof(nihao));
	int k = Block_Read(5 , 22, nihao);
	printf("Number of bytes read: %d\n", k);
	printf("information in the test char array: %s\n", nihao);*/
	
	/* --------------Block_Write pass test--------------------------------------
	Disk_Blocks[5] = (char *)calloc(1, 512);
	char *test = "test for Block_Write!";
	int k = Block_Write(5, test);
	printf("Bytes written: %d\n", k);
	printBlock(Disk_Blocks[5]);*/
	
	/* --------------SuperBlock_Write pass test--------------------------------------
	Super_block new1 = 
	{
		.next_free_block = 12,
		.next_free_inode = 12
	};
	int k1 = Superblock_Write(new1);
	printf("Status of SuperBlock wirte k1: %d\n", k1);
	printSuperBlock();
	
	Super_block new2 = 
	{
		.next_free_block = -1,
		.next_free_inode = 12
	};
	int k2 = Superblock_Write(new2);
	printf("Status of SuperBlock wirte k1: %d\n", k2);
	printSuperBlock();*/
	
	/* --------------Create_File pass test--------------------------------------
	char *testname = "test_filename";
	printSuperBlock();
	int k = Create_File(testname);
	printf("After creating file......\n");
	printSuperBlock();
	printf("\n");
	printDirectory(Directory_Structure[0]);
	printf("\n");
	printInode(Inode_List[0]);*/
	
	/* --------------Open_File pass test--------------------------------------
	char *testname = "test_filename";
	Create_File(testname);
	int k =  Open_File(testname);
	printf("Opened File inode number is: %d\n", k);
	printInode(Inode_List[0]);*/
	
	/* --------------Read_File not pass test--------------------------------------*/
	
	/* --------------Write_File ----------------------------------------------------
	Inode testInode = {
		.Inode_Number = 15,
		.User_Id = 0,
		.Group_Id = 0,
		.File_Size = 512,
		.Start_Block = 0,
		.End_Block = 0,
		.Flag = 0
	};
	Inode_Write(15, testInode);
	printInode(Inode_List[15]);
	printf("\n");
	
	char *testwrite = "liyangyunshishabi,liyangyunshishabi,liyangyunshishabi,liyangyunshishabi,liyangyunshishabi,liyangyunshishabi,liyangyunshishabi,liyangyunshishabi,liyangyunshishabi,liyangyunshishabiliyangyunshishabiliyangyunshishabiliyangyunshishabi,liyangyunshishabiliyangyunshishabiliyangyunshishabi,liyangyunshishabiliyangyunshishabiliyangyunshishabiliyangyunshishabiliyangyunshishabiliyangyunshishabi,liyangyunshishabiliyangyunshishabiliyangyunshishabiliyangyunshishabi,liyangyunshishabi,liyangyunshishabi,liyangyunshishabi----\0";
	printf("Length of test write array: %d\n\n", strlen(testwrite));
	
	Write_File(15, 0, testwrite);
	printInode(Inode_List[15]);

	char * testwrite2 = "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------test write2---------------------------------------------------------------------------";
	printf("Length of test write2 array: %d\n\n", strlen(testwrite2));
	
	Write_File(15, 0, testwrite2);
	printInode(Inode_List[15]);*/

	fprintf(stderr, "\nCount: %d\n", Count);
	fcloseall(); //close all
	return 0;
}
