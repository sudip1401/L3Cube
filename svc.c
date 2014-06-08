/*
 * A Simple Version Control System
 * ---------------------------------------------------------------------
 * This program is based on a few assumptions/restrictions given by
 * the problem statement:
 * 1. The file can have maximum 20 lines.
 * 2. Every line has maximum 10 chars (including \n).
 * 3. Only one operation: Append or delete is allowed at a time.
 * 4. Append: Add a line to the end of the file.
 * 5. Delete: Delete any line of the file.
 * ---------------------------------------------------------------------
 * This program uses forward deltas to keep track of the changes.
 * When a commit is made, only one change is recorded.
 * 
 * A file 'hist' stores the changes.
 * The format of the changes stored in 'hist' are:
 * 		v:N // Version N (0 based)
 * 		+:S // Append line S
 * 	OR	-:n	// Delete line n (0 based)
 * 		e:e // End of version N description
 * 
 * */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER 0
#define FILENAME 1
#define MAX_LINES 20
#define MAX_SIZE 12
#define TRUE 1
#define FALSE 0
#define APPEND '+'
#define DELETE '-'
#define VERSION 'v'
#define END 'e'

int DEBUG = FALSE;

int check(char arg[])
{
	int i = 0, type = FILENAME;
	for(i=0; arg[i]!='\0'; i++) 
	{
		if(arg[i] >= '0' && arg[i] <= '9')
			type = NUMBER;
		else
		{
			type = FILENAME;
			break;
		}
	}
	return type;
}

int strToInt(char arg[])
{
	int val=0, i=0;
	for(i=0; arg[i] != '\0'; i++)
		val = val*10 + (arg[i]-'0');
	return val;
}

int constructFile(int version, char file[MAX_LINES][MAX_SIZE], 
				  int getMostRecent, int * mostRecentVersionNo)
{
	FILE * historyFile = NULL;
	char * line = NULL;
	ssize_t read = 0;
	size_t len = 0;
	
	int v = 0;
	int file_i = 0;
	int i;
	int exitAfterEnd = FALSE;
	int exitLoop = FALSE;
		
	char info;
	char data[MAX_SIZE];
	
	historyFile = fopen("hist", "r");
	if(historyFile == NULL) 
	{
		return 0;
	}
	
	//clear file
	for(i=0; i<MAX_LINES; i++)
		file[i][0] = '\0';
	
	exitLoop = FALSE;
	while( (read = getline(&line, &len, historyFile)) != -1) 
	{
		
		line[strlen(line) - 1] = '\0';
		
		info = line[0];
		strcpy(data, line+2);
		
		switch(info)
		{
			case APPEND:
			{
				if(DEBUG) printf("Append line : %s\n",data);
				strcpy(file[file_i], data);
				file_i++;
				break;
			}
			case DELETE:
			{
				if(DEBUG) printf("delete line : %s\n",data);
				int lineNo = strToInt(data);
				for(i = lineNo; i<file_i; i++)
					strcpy(file[i], file[i+1]);
				file_i--;
				break;
			}
			case VERSION:
			{
				if(DEBUG) printf("version : %s\n",data);
				v = strToInt(data);
				if(v == version)
					exitAfterEnd = TRUE;
				break;
			}
			case END:
			{
				if(DEBUG) printf("End : %s\n",data);
				if(exitAfterEnd == TRUE && getMostRecent == FALSE)
				{
					exitLoop = TRUE;
					break;
				}
				break;
			}
		}
		
		if(exitLoop)
			break;
	}
	
	free(line);
	fclose(historyFile);
	*mostRecentVersionNo = v;
	return file_i;
}


int diffAndRecord(char prevfile[MAX_LINES][MAX_SIZE], int prevfileVersion, int lines, char filename[])
{
	FILE * currentFile = NULL;
	FILE * historyFile = NULL;
	
	int prevfileLineNo = 0;
	
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	
	char change[MAX_SIZE] = "\0";
	int changesFound = FALSE;
	
	currentFile = fopen(filename, "r");
	if(currentFile == NULL) 
	{
		printf("The file %s does not exist.\n", filename);
		exit(0);
	}
	
	historyFile = fopen("hist", "a");
	
	while( (read = getline(&line, &len, currentFile)) != -1 )
	{
		line[strlen(line) - 1] = '\0';
		if(prevfileLineNo < lines) 
		{
			if(strcmp(prevfile[prevfileLineNo], line) != 0) 
			{
				sprintf(change, "-:%d\n", prevfileLineNo);
				break; // since only one operation allowed: append or delete
			}
		}
		else 
		{
			sprintf(change, "+:%s\n", line);
		}
		prevfileLineNo++;
	}
	
	if(prevfileLineNo < lines) 
	{
		sprintf(change, "-:%d\n", prevfileLineNo);
	}
	
	if(strlen(change) > 0)
	{
		fprintf(historyFile, "v:%d\n", prevfileVersion + 1);
		if(DEBUG) printf("Change found as : %s\n",change);
		fprintf(historyFile, "%s", change);
		fprintf(historyFile, "e:e\n");
		changesFound = TRUE;
	}

	fclose(historyFile);
	fclose(currentFile);
	
	return changesFound;
}

int	main(int argc, char * argv[])
{
	int argType;
	char file[MAX_LINES][MAX_SIZE];
	int mostRecentVersionNo = -1;
	
	if (argc <= 1)
	{
		printf("Usage:\n");
		printf("%-20s %-40s\n", "svc FILENAME", "commit file");
		printf("%-20s %-40s\n", "svc N" , "display Nth version of the file");
		printf("svc will track the versions of one file for you.\n");
		exit(0);
	}
	
	if(argc == 3) {
		DEBUG = strToInt(argv[2]);
	}
	
	argType = check(argv[1]);
	
	if(argType == NUMBER) 
	{
		// Show Nth version
		if(DEBUG) printf("Arg type is number\n");
		int version = strToInt(argv[1]);
		int lines = constructFile(version, file, FALSE, &mostRecentVersionNo);
		int i;
		if(DEBUG) printf("-------Version %d-------\n",version);
		for(i=0; i<lines; i++)
			printf("%s\n",file[i]);
	}
	else
	{
		// Save current version
		if(DEBUG) printf("Arg type is filename	\n");
		int lines = constructFile(0, file, TRUE, &mostRecentVersionNo);
		int changes = diffAndRecord(file, mostRecentVersionNo, lines, argv[1]);
		if(changes) printf("File committed.\n");
		else printf("The file has already been committed. No changes found.\n");
	}
	
	
	return 0;
	
}
	
