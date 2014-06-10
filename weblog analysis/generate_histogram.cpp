#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <climits>

using namespace std;

#define MAX 1024

struct item {
	int count;
	char name[MAX];
};

vector<struct item> items;

int main(int argc, char * argv[]) {


	if(argc <= 1) {
		printf("Usage: ./a.out filename\nPlease specify a file.\n");
		exit(0);
	}
	
	FILE * fp = NULL;
	
	struct item temp;
	char *line = NULL;
	ssize_t read;
	size_t len;
	int mx = 0, mn = INT_MAX;
	
	fp = fopen(argv[1], "r");
	
	while((read = getline(&line, &len, fp)) != -1) {
		line[strlen(line) - 1] = '\0';
		sscanf(line, "%d%[^\n]s", &temp.count, temp.name);
		mn = min(mn, temp.count);
		mx = max(mx, temp.count);
		items.push_back(temp);
	}
	
	int width = 80;
	int scale = (mx-mn)/width + 1;
	
	printf("\n %28s:%d", "Scale",scale);
	printf("\n %30s", "0");
	for (int j = 0; j <= (mx/scale) ; j++) {
		printf(" ");
		if(j == (mx/scale)) {
			printf("%d",j);
		}
	}
		
	
	
	for (int i = 0; i < items.size(); ++i) {
		printf("\n%30s|", items[i].name);
		for (int j = 0; j <= (items[i].count/scale) ; j++)
			printf("#");
	}
	
	printf("\n");
	return 0;
}
