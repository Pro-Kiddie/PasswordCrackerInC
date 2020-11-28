#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_SIZE 256 //The max number of characters for a line from the rainbow file and shadowfile is set to be 256 
#define MAX_USERID 51 //Maximum length of a user ID allowed is 50. Anything longer than that will cause memory overflow.

//struct for each entry of rainbow table
struct rainbowNode{
	//each entry has plain pass, hashType, hash
	char line[MAX_SIZE];
	char * plain;
	char * hashType;
	char * hash;

	//pointer to next node
	struct rainbowNode * next;
};
typedef struct rainbowNode rainbow; 
//Purpose: display usage of the function when wrong number of arguments were passed in
//input parameters: char * filePath; the char pointer pointing to the argv[0]
//return value: void
void usage1(char * filePath){
	printf("Usage : %s <shadow file> <rainbow table> \n",filePath );
	printf("        <shadow file> : A file path/name in which contains the hashed password for every user.\n");
	printf("        <rainbow table> : A file path/name in which contains the plain password and their respective hashed values.\n");
}
//Purpose: display invalid file path error message when either of shadow.txt or mytab2411.txt cannot be opened
//input parameters: char * filePath; the char pointer pointing to the either the shadow.txt command line argument or mytab2411.txt command line argument
//return value: void
void usage2(char * filePath){
	printf("Fatal error! %s file not found\n", filePath );
	printf("%s\n", "Program halted, Please verify the file path and try again." );
}
	
//Purpose: verify the command line arguments
//input parameters: int argc, char * filePath, char * shadowFileArg, char * rainbowTableArg
//return value: return either 0 or 1. 0 means all the arguments are valid. Return 1 means the arguments are invalid and main method should exit
int verifyArg (int argc, char * filePath, char * shadowFileArg, char * rainbowTableArg){
	//check if number of argument read in is 3 (part2 source code, shadow file and rainbow table file)
	if(argc != 3) {
		//if argc not equal to 3, show usage of the file
		usage1(filePath);
		return 1;
	}
	FILE * shadowFile;
	FILE * rainbowTable;
	//try to open file two files
	shadowFile = fopen(shadowFileArg,"r");
	rainbowTable = fopen(rainbowTableArg, "r");
	//if file cannot be opened, show error
	//if any file cannot be opened, show error.
	//if both cannot be opened, show error for both.
	if (shadowFile == NULL) {
		usage2(shadowFileArg);
	}
	if (rainbowTable == NULL) {
		usage2(rainbowTableArg);
	}
	if (shadowFile == NULL && rainbowTable == NULL){
		return 1;
	}
	else if (shadowFile == NULL && rainbowTable != NULL){
		fclose(rainbowTable);
		return 1;
	}

	else if (shadowFile != NULL && rainbowTable == NULL){
		fclose(shadowFile);
		return 1;
	}
	

	//both file are valid and argument verified
	fclose(shadowFile);
	fclose(rainbowTable);
	return 0;

}
//Purpose: Display the start/end time for the program
//Input: char pointer to the state of the program, either "started" or "ended"
//Return: void 
void displayTime(char * state){
	time_t totalSec = time(NULL);//total no of secs since EPOCH
	struct tm * currTime = localtime(&totalSec); //store local time in struct declared in time.h
	char * result = (char *) malloc(20 * sizeof(char)); //allocate memory for result as it is just a pointer with no memory pointing to.Compiler will not allocate memory for char * like in main. 
	//OR SIMPLY char result[20];
	strftime(result, 20, "%Y:%m:%d %H:%M:%S",currTime);//E.g. 2017:11:12 12:30:48 Max is 20 characters including \0
	printf("Program %s at %s\n", state, result );

	free(result); //free result as not needed anymore

}
//Purpose: initialize the linked list by creating a node for each line of the mytab2411.txt file.
//input parameters: No parameters
//return value: return the head of the initialized link list.
rainbow * initializeList(FILE * rainbowTable){
	//create the first node of the linked list
	rainbow * rainbowList = NULL;

	//Allocate memory from the heap for each line in rainbow file
	//char * line = (char *)malloc(sizeof(char)*MAX_SIZE);
	void * p;
	int count = 0;
	
	while(1){
		//create new node for the linked list.
		rainbow * new = (rainbow *)malloc(sizeof(rainbow));
		//read line by line until the end of rainbow file
		p = fgets(new -> line, MAX_SIZE, rainbowTable);
		if (p == NULL){//reached end of file. break out the loop.
			free(new); //free the node just created.
			break;
		}
		//remove carriage return for the line read
		strtok(new -> line, "\n");

		//process the line read and store them in new node
		new -> plain = strtok(new -> line, "$"); //get the plain password using $ as delimiter
		new -> hashType = strtok(NULL, "$");//get the hashType
		new -> hash = strtok(NULL, "$");// get the hash value
		//remove the : in plain password
		strtok(new -> plain, ":");
		//if the line is in the correct format, plain, hashType and hash should have being pointing to some string
		if (new -> plain == NULL || new -> hashType == NULL || new -> hash == NULL){//wrong format
			printf("%s %d.\n", "Wrong format of rainbow table at line", ++count );
			printf("%s\n", "Please make sure each line in rainbow table follows this format: Plain Password:$1/6$$Hash value." );
			continue;//skip this line from ranbow table. Do not include in linked list
			free(new);
		}

		//set the new node point to the current head
		new -> next = rainbowList;
		//set the list to point to the new node
		rainbowList = new;

		count ++;

		//create memory for the next line from rainbow file
		//line = (char *)malloc(sizeof(char)*MAX_SIZE);
	}

	//return the link list
	return rainbowList;

}
//Purpose: release the memory borrowed from the heap for each node of the link list
//input parameters: rainbow * rainbowList; take in the head of the link list as argument
//return value: void
void releaseMem(rainbow * rainbowList){
	//base case
	if (rainbowList == NULL){
		return;
	}
	//recursive step
	releaseMem(rainbowList -> next);
	free(rainbowList);

}
//Purpose: the main function; execution starts here
//input parameters: int argc, char * argv[]; argc states the number of arguments took in from command line. argv is an array of char pointers pointing to different part of command line input, separated by a space.
//return value: 0 means success execution of the program.
int main(int argc, char * argv[]){
	//verify input, make sure the shadow file and rainbow table can be opened
	FILE * shadowFile;
	FILE * rainbowTable;
	int verifyResult = verifyArg(argc, argv[0], argv[1], argv[2]);
	if (verifyResult == 1) return 0; //if invalid argument, exit the program.
	//arguments verified, open the files.
	shadowFile = fopen(argv[1],"r");
	rainbowTable = fopen(argv[2], "r");
	//display start time
	displayTime("started");
	printf("\n"); //house keeping
	//create the linked list for the rainbow table for lookup table
	rainbow * rainbowList = initializeList(rainbowTable);
	
	//get entry from shadow file
	char userID[MAX_USERID] = {0};
	int hashType;
	char hash[MAX_SIZE] = {0};
	int noOfItems;
	char line[MAX_SIZE];
	printf("\n%s\n", "Results:" );
	//get line by line from shadow file
	while(fgets(line, MAX_SIZE, shadowFile) != NULL){
		//validate entry in the correct format
		//correct format of an entry: userID:$1/6$$hash:
		/*	Usage of formatted string with scanf():
			Pollock, W., 2016. C Printf and Scanf Reference. [Online] 
			Available at: https://wpollock.com/CPlus/PrintfRef.htm
			[Accessed 08 12 2017].
		*/
		noOfItems = sscanf(line,"%[^:]:$%d$$%[^:]:%*[^\n]\n",userID, &hashType, hash); //%[^:]:$ means scan for any character except for : before ;$. 
		//demonstration of another concept of string mainuplating beside the one demonstrated in initializeList() which uses strtok()
		//better way as it specifies the format of the string to be taken in.
		if (noOfItems != 3 || (hashType != 1 && hashType != 6)){
			//if noOfItems not equal to 3, that means the line from shadow file is not in the correct format
			//if Hash type is not 1 or 6, then unsupported hashing scheme. 
			printf("%s\n", "Data Error: Invalid entry found in the shadow file. (skipped)" );
			//skip iteration;
			continue;
		}
		//compare with every node in link list
		int cmpResult;
		rainbow * workingRainbow = rainbowList;
		while (workingRainbow != NULL){
			cmpResult = strcmp(hash, workingRainbow -> hash);
			if(cmpResult == 0){
				printf("userID : %s - password found => %s \n", userID, workingRainbow -> plain );
				break; //found password, break out the while loop for next entry of shadow file
			}
			workingRainbow = workingRainbow -> next;
		}


		if (cmpResult != 0){ //password not found
			printf("userID : %s - password <NOT FOUND> \n", userID);
		}
	//display result
	//repeat for next entry of shadow file
	}
	//free the link list + the string inside every link list
	releaseMem(rainbowList);
	//display end time
	printf("\n");//house keeping
	displayTime("ended");
	//close opened files
	fclose(shadowFile);
	fclose(rainbowTable);
}