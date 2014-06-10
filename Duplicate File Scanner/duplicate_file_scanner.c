/*
 * List duplicate files in a hard drive (or folder)
 * --------------------------------------------------------------------- 
 * This program scans recrusively, the folder you pass to it as an
 * argument from the command line.
 * 
 * Usage: ./a.out /home/username/folder1/folder2/../folderN/
 * 
 * The duplicate files are listed together and the user is given an option
 * to select the files to be deleted from a duplicate-file set.
 * 
 * This program has been tested upto the home folder i.e. /home/username/
 * It is capable of listing files in a folder, not across partitions yet.
 * 
 */
 
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 1024
#define STORE_MAX 1024

#define DEBUG 0

char root[PATH_MAX+1] = "";

struct dirfile {
	char dir[MAX];
	char filename[MAX];
} listing[STORE_MAX];

int listing_i = 0;
FILE * files_fptr = NULL;

void scanRecursive(DIR * dir, int level, char dirpath[]) {
	
	struct dirent * direntry;
	DIR * nextdir;
	char px[PATH_MAX+1];
	int i;
	
	while((direntry = readdir(dir)) != NULL) {
		if(direntry->d_type == DT_DIR) {
			if(DEBUG) for(i=0;i<level;i++) printf("    ");
			if(DEBUG) printf("Dir: %s%s\n", dirpath, direntry->d_name);
			if(strcmp(direntry->d_name, ".") == 0 || strcmp(direntry->d_name, "..") == 0) {
				;
			} else {
				strcpy(px, dirpath);
				strcat(px, direntry->d_name);
				strcat(px, "/");
				nextdir = opendir(px);
				if(nextdir != NULL) {
					scanRecursive(nextdir, level+1, px);
					closedir(nextdir);
				}
				else {
					if(DEBUG) printf("Could not open dir %s\n",px);
				}
			}
		} else {
			if(DEBUG) for(i=0;i<level;i++) printf("    ");
			if(DEBUG) printf("File: %s%s\n",dirpath,direntry->d_name);
			fprintf(files_fptr,"%s %s\n",dirpath,direntry->d_name);
			
		}
	}
}


void removeFiles()
{
	int i,j;
	FILE * dups_fptr = NULL;
	
	char *line = NULL;
	ssize_t read = 0;
	size_t len = 0;
	
	int del_index[MAX];
	char del_path[MAX];
	int del_index_i;
	int user_choice;
	int index, skipset;
	char temp;
	
	char remove_command[MAX];

	dups_fptr = fopen("dups","r");
	
	index=1;
	while(read = getline(&line, &len, dups_fptr) != -1) {
		line[strlen(line) - 1] = '\0';
		if(strlen(line) > 0) {
			sscanf(line, "%s%s", listing[listing_i].dir, listing[listing_i].filename);
			printf("%d: %s%s\n",index, listing[listing_i].dir, listing[listing_i].filename);
			index++;
			listing_i++;
		} else {
			
			skipset = 0;
			printf("Number of files to delete [0-%d]: ",index-1);
			scanf("%d",&user_choice);
			if(user_choice == 0) {
				index = 1;
				printf("\n");
				continue;
			}
			printf("Enter the path numbers of the files you would like to delete. [1-%d]\n",index-1);
			del_index_i = 0;
			for(i=0;i<user_choice;i++) {
				int val;
				scanf("%d",&val);
				if(val == 0) {
					skipset = 1;
				} else if(val >= 1 && val <= index-1) {
					del_index[del_index_i] = val;
					del_index_i++;
				}
			}
			
			if(skipset) {
				index = 1;
				printf("\n");
				continue;
			}
			
			for(j=0;j<del_index_i;j++) {
				sprintf(del_path, "%s%s", listing[listing_i - (index - 1) + del_index[j] - 1].dir, 
											listing[listing_i - (index - 1) + + del_index[j] - 1].filename);
				printf("Are you sure you want to delete %s? (y/n) ", del_path);
				scanf(" %c",&temp);
				if(temp == 'y' || temp == 'Y') {
					printf("Deleting %s..\n", del_path);
					strcpy(remove_command, "rm ");
					strcat(remove_command, del_path);
					system(remove_command);
				}
			}
			
			
			index = 1;
			printf("Done.\n\n");
		}	
	}
	
	free(line);
	fclose(dups_fptr);
}


int main(int argc, char * argv[]) {
	
	DIR * dir;
	int i,j;
	
	if(argc <= 1) {
		printf("Please specify a root path to search in.\n");
		exit(0);
	}
	
	strcpy(root, argv[1]);
	
	if(root[strlen(root)-1] != '/') {
		strcat(root, "/");
	}
	
	dir = opendir(root);
	
	if(dir == NULL) {
	
		printf("Could not open dir: %s",root);
		exit(0);
	}
	
	
	files_fptr = fopen("files","w");
	
	scanRecursive(dir,0,root);
	
	fclose(files_fptr);
	
	system("sort --key=2 files | uniq --skip-fields=1 --all-repeated=separate > dups");
	system("echo \"\" >> dups");

	removeFiles();
	
	closedir(dir);
	return 0;	
}
