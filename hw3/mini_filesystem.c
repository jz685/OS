#include <stdio.h>
#include <unistd.h>
#include "mini_filesystem.h"
#include <time.h>
//----------------
// The index for the next free Directory in Directory Structure
int next_free_directory;

int Search_Directory(char* Filename);
int Add_to_Directory(char * Filename, int inodeNumber);
Inode Inode_Read(int inodeNumber);
int Inode_Write(int inodeNumber, Inode inode);
int Block_Read(int Block_number, int numberOfBytes, char * name);
int Block_Write(int blocknumber, char *string, int len);
Super_block Superblock_Read();
int Superblock_Write(Super_block Superblock);


//---------------------------------------helper function-------------------------------------
int min(int a, int b)
{
	return a < b?a:b;
}

struct timespec gettime(void)
{
	struct timespec tim;
	clock_gettime(CLOCK_REALTIME, &tim);
	return tim;
}

int name_compare(const char *a, const char *b)
{
	int j = 0;
	int sizea = strlen(a);
	int sizeb = strlen(b);
	//printf("a:%d b:%d\n", sizea ,sizeb);
	if (sizea == 0 || sizeb == 0 || sizea != sizeb)
	{
		return 1;
	}else{
		for(j = 0; j < min(sizea, sizeb); j++){
			//printf("a[%d]: %c\n", j, a[j]);
			//printf("b[%d]: %c\n", j, b[j]);
			if (a[j] != b[j]){				
				//printf("a[%d]: %c???\n", j, a[j]);
				//printf("b[%d]: %c???\n", j, b[j]);
				return 1;					// not equal return 1
			}
		}
	}
	return 0;
}

void printDirectory(Directory d)
{
	printf("Filename: %s\n", d.Filename);
	printf("Inode_Number: %d\n", d.Inode_Number);
}

void printInode(Inode a)
{
	printf("Inode_Number: %d\n",a.Inode_Number);
	printf("User_Id: %d\n", a.User_Id );
	printf("Group_Id: %d\n", a.Group_Id);
	printf("File_Size: %d\n", a.File_Size);
	printf("Start_Block: %d\n", a.Start_Block);
	printf("End_Block: %d\n", a.End_Block);
	printf("Flag: %d\n", a.Flag);
}

void printBlock(char *b)
{
	printf("Information in the block: %s\n", b);
}

void printSuperBlock()
{
	printf("next_free_block: %d\n", Superblock.next_free_block);
	printf("next_free_inode: %d\n", Superblock.next_free_inode);
}

//------------------------------------------Lower level functions------------------------------
// Implementation: This should search through the directory structure for the given file name.
int Search_Directory(char* Filename)
{
	Count++;
	fprintf(stderr, "Start Search_Directory ... \n");
	int inodenumber = -1;
	int i = 0, j = 0;
	for (i = 0; i < 128; i++)
	{
		if (!name_compare(Directory_Structure[i].Filename, Filename))				// all characters are equal
		{
			inodenumber = Directory_Structure[i].Inode_Number;						// Search from 0 to next_free_directory -1 ??
			break;
		}
	}
	fprintf(stderr, "inode number is: %d \n",  inodenumber);
	fprintf(stderr, "End Search_Directory .../ \n");
	//--
	struct timespec timenow = gettime();
	char str[200];
	FILE* fp = fopen(Log_Filename, "a");
	sprintf(str, "<%lld.%.9ld>\t<Directory>\t<Read>\n", timenow.tv_sec, timenow.tv_nsec);
	fputs(str, fp);
	fclose(fp);
	//--
	return inodenumber;
}

/* Implementation: This should add an entry to the directory structure with the Filename and
 * Inode number provided as input. It returns the status indicating if it was able to successfully
 * add the entry to the directory or not.
 */
int Add_to_Directory(char * Filename, int inodeNumber)
{
	Count++;
	fprintf(stderr, "Start Add_to_Directory ... \n");
	if (next_free_directory < 127){
		int f = next_free_directory, len = strlen(Filename), i = 0;
		next_free_directory++;
		for (i = 0; i < len; i++)
		{
			Directory_Structure[f].Filename[i] = Filename[i];
		}
		Directory_Structure[f].Inode_Number = inodeNumber;
		// printDirectory(Directory_Structure[f]);
		fprintf(stderr, "End Add_to_Directory .../ \n");
		//--
		struct timespec timenow = gettime();
		char str[200];
		FILE* fp = fopen(Log_Filename, "a");
		sprintf(str, "<%lld.%.9ld>\t<Directory>\t<Write>\n", timenow.tv_sec, timenow.tv_nsec);
		fputs(str, fp);
		fclose(fp);
		//--
		return 0;
	}else{
		printf("Diectory_structure is full!");
		return -1;
	}
}


/* Implementation: For the given inode number, if the file exists, this function should return
 * the Inode structure and NULL if the file doesn’t exist.
 */
Inode Inode_Read(int inodeNumber)
{
	Count++;
	fprintf(stderr, "Start Inode_Read ... \n");
	//Output Inode
	int i = 0;
	if (inodeNumber < 128 && inodeNumber >= 0)
	{
			//--
		struct timespec timenow = gettime();
		char str[200];
		FILE* fp = fopen(Log_Filename, "a");
		sprintf(str, "<%lld.%.9ld>\t<Inode>\t\t<Read>\n", timenow.tv_sec, timenow.tv_nsec);
		fputs(str, fp);
		fclose(fp);
		//--
		return Inode_List[inodeNumber];
	}else{
		Inode errornode = {
			.Inode_Number = -1,
			.User_Id = 0,
			.Group_Id = 0,
			.File_Size = 0,
			.Start_Block = 0,
			.End_Block = 0,
			.Flag = 0};
		fprintf(stderr, "End Inode_Read .../ \n");
		return errornode;
	}
}

/* Implementation: This function should copy the contents of Inode structure provided as
 * input to the Inode present at the Inode Number passed.
 */
int Inode_Write(int inodeNumber, Inode newnode)
{
	Count++;
	fprintf(stderr, "Start Inode_Write ... \n");
	// Output: (int) Status indicating successful write or not
	if (inodeNumber > 127 || inodeNumber < 0){
		return -1;							// Inode does not exist
	}else{
		Inode_List[inodeNumber].Inode_Number = newnode.Inode_Number;
		Inode_List[inodeNumber].User_Id = newnode.User_Id;
		Inode_List[inodeNumber].Group_Id = newnode.Group_Id;
		Inode_List[inodeNumber].File_Size = newnode.File_Size;
		Inode_List[inodeNumber].Start_Block = newnode.Start_Block;
		Inode_List[inodeNumber].End_Block = newnode.End_Block;
		Inode_List[inodeNumber].Flag = newnode.Flag;
		fprintf(stderr, "End Inode_Write .../ \n");
		//--
		struct timespec timenow = gettime();
		char str[200];
		FILE* fp = fopen(Log_Filename, "a");
		sprintf(str, "<%lld.%.9ld>\t<Inode>\t\t<Write>\n", timenow.tv_sec, timenow.tv_nsec);
		fputs(str, fp);
		fclose(fp);
		//--
		return 0;
	}
}

/* Implementation: This function should read the given number of bytes from the provided
 * block number and write it to character string provided. It should then return the number of
 * bytes read.
 */
int Block_Read(int Block_number, int numberOfBytes, char * name)
{
	Count++;
	//fprintf(stderr, "Start Block_Read ... \n");
	if (Block_number > 8192 || Block_number < 0)
	{
		return -1; 								// blocknumber beyond bound.
	}else{
		int count = 0, i = 0;
		//size_t len = strlen(name);                   what if the length of name is small than numberOfBytes
		//printf("len: %d numberOfBytes: %d\n", len, numberOfBytes);
		
		if (Disk_Blocks[Block_number] == NULL){
			return 0;
		}else{
			for (i = 0; i < numberOfBytes && i < 512; i++)
			{
				name[i] = Disk_Blocks[Block_number][i];
				count++;
			}
			// Output: (int) Number of bytes read
			//fprintf(stderr, "End Block_Read .../ \n");
			//--
			struct timespec timenow = gettime();
			char str[200];
			FILE* fp = fopen(Log_Filename, "a");
			sprintf(str, "<%lld.%.9ld>\t<Block>\t\t<Read>\n", timenow.tv_sec, timenow.tv_nsec);
			fputs(str, fp);
			fclose(fp);
			//--
			return count;
		}
	}
}

/* Implementation: Given the block number, write the contents of the string provided to the
 * block and return the number of bytes written.
 */ 
int Block_Write(int blocknumber, char *string, int len)
{
	Count++;
	//fprintf(stderr, "Start Block_Write ... \n");
	if (blocknumber > 8192 || blocknumber < 0)
	{
		return -1; 								// blocknumber beyond bound.
	}else{
		int count = 0, i = 0;
		//int len = strlen(string);
		//printf("length of string: %d\n", len);
		if (Disk_Blocks[blocknumber] == NULL){
			Disk_Blocks[blocknumber] = (char*)calloc(1, 512);
		}
		for (i = 0; i < len && i < 512; i++)
		{
			Disk_Blocks[blocknumber][i] = string[i];
			count++;
		}
		// Output: (int) Number of bytes written
		//printf("finish write...\n");
		//fprintf(stderr, "End Block_Write .../ \n");
		//--
		struct timespec timenow = gettime();
		char str[200];
		FILE* fp = fopen(Log_Filename, "a");
		sprintf(str, "<%lld.%.9ld>\t<Block>\t\t<Write>\n", timenow.tv_sec, timenow.tv_nsec);
		fputs(str, fp);
		fclose(fp);
		//--
		return count;
	}
}

/* Implementation: Return the superblock structure. */
Super_block Superblock_Read()
{
	Count++;
	//fprintf(stderr, "start Superblock_Read ... \n");
	// Output: (Superblock structure) superblock
	//--
	struct timespec timenow = gettime();
	char str[200];
	FILE* fp = fopen(Log_Filename, "a");
	sprintf(str, "<%lld.%.9ld>\t<SuperBlock>\t<Read>\n", timenow.tv_sec, timenow.tv_nsec);
	fputs(str, fp);
	fclose(fp);
	//--
	return Superblock;
}

/* Implementation: Copy the contents of the provided superblock structure to the superblock */
int Superblock_Write(Super_block newSuperblock)
{
	Count++;
	//fprintf(stderr, "start Superblock_Write ... \n");
	if (newSuperblock.next_free_block > 8191 || newSuperblock.next_free_block < 0 || newSuperblock.next_free_inode > 127 || newSuperblock.next_free_inode < 0)
	{
		return -1; 											// the value of newSuperBlock is not valid.
	}else{
		Superblock.next_free_block = newSuperblock.next_free_block;
		Superblock.next_free_inode = newSuperblock.next_free_inode;
		// Output: (int) Status whether superblock was successfully written
		//fprintf(stderr, "End Superblock_Write .../ \n");
		//--
		struct timespec timenow = gettime();
		char str[200];
		FILE* fp = fopen(Log_Filename, "a");
		sprintf(str, "<%lld.%.9ld>\t<SuperBlock>\t<Write>\n", timenow.tv_sec, timenow.tv_nsec);
		fputs(str, fp);
		fclose(fp);
		//--
		return 0;
	}
}


//------------------------------------------Higher level functions------------------------------
/* Implementation: Set the log file name as the filename provided as input. Set count to 0. Initialize
 * anything else if you think is needed for your file system to work, for example, set initial values for
 * superblock, allocate/initialize anything else that you may need etc. Return Success or Failure
 * appropriately.
 */
int Initialize_Filesystem(char* log_filename)
{
	fprintf(stderr, "start Initialize_Filesystem ... \n");
	Count = 0;
	Log_Filename = log_filename;
	FILE* fp = fopen(Log_Filename, "w");
	fclose(fp);
	
	Superblock.next_free_block = 0;										// Initialize the next_free_block of Superblock
	Superblock.next_free_inode = 0;										// Initialize the next_free_inode of Superblock
	
	next_free_directory = 0;
	
	int i = 0;
	for (i = 0; i < 128; i++)		// Initialize the Directory_Structure
	{
		memset(Directory_Structure[i].Filename, 0, 16);						// ?? No deleting files
		Directory_Structure[i].Inode_Number = -1;							// ?? if search for 00000... it matches
	}
	
	for (i = 0; i < 128; i++)					// Initialize the Inode_list
	{
		Inode_List[i].Inode_Number = 0;										// name them 0 is confusing ??
		Inode_List[i].User_Id = 0;
		Inode_List[i].Group_Id = 0;
		Inode_List[i].File_Size = 0;
		Inode_List[i].Start_Block = 0;
		Inode_List[i].End_Block = 0;
		Inode_List[i].Flag = 0;
	}
	
	for (i = 0; i < 8192; i++){
		Disk_Blocks[i] = NULL;												
	}
	
	return 0;
}

/* Implementation: This function should first check whether the file with the provided file name
 * already exists in the directory structure. If yes, return with appropriate error. If not, get the next
 * free inode number from the super block and using that create an entry for the file in the Inode_List.
 * Then, using the inode number returned and filename you have, add the entry to the directory
 * structure. Also, update the superblock since the next free inode index needs to be incremented.
 * Then return appropriately.
 */
int Create_File(char* filename)
{
	fprintf(stderr, "start Create_File ... \n");
	if (Search_Directory(filename) == -1)
	{
		uid_t myuid = getuid();
		uid_t mygid = getgid();
		//fprintf(stderr, "my uid = %d and my gid = %d \n", myuid, mygid);
		
		Inode newInode = 
		{
			.Inode_Number = Superblock.next_free_inode,
			.User_Id = myuid,
			.Group_Id = mygid,
			.File_Size = 0,
			.Start_Block = -1,
			.End_Block = -1,
			.Flag = 0
		};
		if(!Inode_Write(Superblock.next_free_inode, newInode))								// check whether calling of Inode_Write succeed
		{
			if(!Add_to_Directory(filename, Superblock.next_free_inode))				// check whether calling of Add_to_Directory succeed
			{
				// if success
				// update next_free_inode
				Superblock.next_free_inode++;
				fprintf(stderr, "End Create_File .../ \n");
				return 0;
			}else{
				return -3;							// could not add to directory
			}
		}else{
			return -2;								// could not write new node
		}
	}else{
		return -1;									// file already exists
	}
}

/* Implementation: Search the directory for the provided file name. If found, set the inode flag for it
 * to 1 and return the inode number. Else, return appropriately.
 */
int Open_File(char* filename)
{
	fprintf(stderr, "start Open_File ... \n");
	int index = Search_Directory(filename);
	if (index != -1)
	{
		Inode_List[index].Flag = 1;
		return index;
	}else{
		return -1;								// file does not exist
	}
}

/* Implementation: For the given inode number, first check if the provided offset and number of bytes
 * to be read is correct by comparing it with the size of the file. If correct, read the provided number
 * of bytes and store them in the provided character array. Return the number of bytes read. If
 * incorrect, return appropriate error. Hint: You need to make multiple calls to block_read to
 * implement this.
 */
int Read_File(int inode_number, int offset, int count, char* to_read)
{
	// what offset is !!!
	fprintf(stderr, "start Read_File ... \n");
	if (offset + count <= Inode_List[inode_number].File_Size){
		if (offset + count > 512)
		{
			int i = 0;								// counter
			int m = (offset + count) / 512;			// number of blocks used
			int r = (offset + count) % 512;			// left number of bytes
			int BlockIndex = Inode_List[inode_number].Start_Block + offset/512;
			for (i = 0; i < m; i++)
			{
				char temp[512];
				if (i == 0)
				{
					Block_Read(BlockIndex, 512, temp);
					strncpy(to_read, &(temp[offset%512]), 512);
				}else{
					Block_Read(BlockIndex, 512, temp);
					strncat(to_read, temp, 512);
				}
				BlockIndex++;
			}
			char rest[r];
			Block_Read(BlockIndex, r, rest);
			strcat(to_read, rest);
			//fprintf(stderr, "%s\t", rest);
			//fprintf(stderr, "%s\t", to_read);
			return count;
		}else{
			Block_Read(Inode_List[inode_number].Start_Block, count, to_read);
			return count;
		}
	}else{
		return -1;									// offset and number to read is larger than file size
	}
}

/* Implementation: For the given inode number, first check if the provided offset is correct by
 * comparing it with the size of the file.If correct, write the provided string to the file blocks, update the inode (since this
 * changes the file size, last block etc.) and superblock (As the next free disk block will change) with
 * the right information and return the number of bytes written. If incorrect, return appropriately.*/
int Write_File(int inode_number, int offset, char* to_write, int len)
{
	//fprintf(stderr, "start Write_File ... \n");
	//Inode InodeTemp = Inode_Read(inode_number);	
	//if (offset < Inode_List[inode_number].File_Size){
	if (offset % 512 == 0)								// assume offsets will be aligned at 512 bytes
	{
		//int len = strlen(to_write);
		printf("length of to_write: %d\n\n", len);
		if (Inode_List[inode_number].Start_Block == -1)
		{
			Inode_List[inode_number].Start_Block = Superblock.next_free_block;
			Inode_List[inode_number].End_Block = Superblock.next_free_block;
			Superblock.next_free_block++;
		}
		int start = Inode_List[inode_number].Start_Block;
		int end = Inode_List[inode_number].End_Block;
		//printf("start block: %d\n", start);
		//printf("end block: %d\n", start);

		int diff = (len + 512 - 1)/512 - (end - start + 1);				//the difference between new and old the number of blocks
		printf("Difference of blocks: %d\n\n", diff);
		
		Inode_List[inode_number].End_Block = diff + start;
		Superblock.next_free_block += diff;
		Inode_List[inode_number].File_Size = len;
		printSuperBlock();
		int i = 0;
		int m = len / 512;
		int r = len % 512;
		//printf("m: %d\n\n", m);
		//printf("r: %d\n\n", r);
		for (i = 0; i < m; i++)
		{
			//printf("\n New part of string.............\n");
			char temp[512];
			//printf("create temp....\n\n");
			strncpy(temp, &(to_write[i * 512]), 512);
			//printf("temp:\n%s\n", temp);
			//printf("Block write\n\n");
			Block_Write(start + i, temp, 512);
			//printBlock(Disk_Blocks[start+i]);
			//printf("\n");
		}
		//printf("\nRest part of string.............\n");
		char rest[r+1];
		//printf("\nstart copy.............\n");
		strncpy(rest, &(to_write[i * 512]), r);
		rest[r] = '\0';
		//printf("rest: %s\n", rest);
		Block_Write(Inode_List[inode_number].End_Block, rest, r); //!!!!!!!!!!!!	r+1
		//printBlock(Disk_Blocks[start+i]);
		return 0;
	}else{
		return -1;							// offset is beyond file's size
	}
}

int Close_file(int inode_number)
{
	Inode_List[inode_number].Flag = 0;
	return 0;
}
