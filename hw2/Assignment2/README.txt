Group information
Member1:
Name: Zixiao Wang
ID: 5107014

Member2:
Name: Jia Zhang
ID:

The CSElab machine for testing code:

Syntax and usage pointers for program:

Special test cases handled:
1.

Special test cases not handled:
1.





/* Read files in inputdir*/
	char *files[100];
	int flag[100];
	char thumbnails[100][100];
	int i = 0;
	DIR *dir;
	struct dirent *ptr;
	if ((dir = opendir(inputdir)) == NULL){
		perror("Error happend when open directory!");
		return 1;
	}
	
	ptr = readdir(dir);
	while(ptr != NULL){
        files[i] = ptr->d_name;
        flag[i] = 1;				// 1 represent has not been converted.
        ptr = readdir(dir);
        i++;
    }
    closedir(dir);


// create html
int createHTML(char* outputdir){
	FILE *fp = fopen(outputdir, "w");
	fprintf(fp , "<html><head><title>thumb</title><meta http‐equiv=\"refresh\" content=\"1; URL=file:///");
	
	fprintf(fp, outputdir);
	fprintf(fp, "/");
	fprintf(fp, thumbnails[i+1]);
	fprint(fp, "></head><body><img src=\"");
	fprintf(fp, outputdir);
	fprintf(fp, "/");
	fprintf(fp, thumbnails[i]);
	fprintf(fp, "\"/></a></body></html>");
	fclose(fp);
}

// store thumbnail name in array
snprintf(thumbnails[index], 100, "%s", getpid());
