#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <crypt.h>
#include <ctype.h>
#include <string.h>

//Part 1 never state cannot use global variable.
FILE * wordList;
int MAX, MIN;
const char * salt = "$"; //salt of empty string
const char * hashType1 = "$1$";
const char * hashType2 = "$6$";

//Purpose: verify the command line arguments
//input parameters: int argc, char * wordListPath, char * min, char * max
//return value: an integer representing different type of error
int verifyArg(int argc, char * wordListPath, char * min, char * max){
	//verify main method read in total 4 arguments 
	if (argc != 4) return 2;

	//verify the file path
	wordList = fopen(wordListPath, "r");
	if (wordList == NULL) return 1; //fopen returns NULL if file cannot be opened or permission denied.

	//verify min and max are digit
	//loop through every char of min & max to check if it is digit
	for (int i = 0; min[i] != '\0'; i++){
		if(!isdigit(min[i])) return 2;
	}

	if (atoi(min) < 1) return 2; //min cannot be less than 1

	MIN = atoi(min);

	for (int i = 0; max[i] != '\0'; i++){
		if(!isdigit(max[i])) return 2;
	}

	if (atoi(max) < MIN) return 2; //max cannot be less than min

	MAX = atoi(max);

	//valid arguments
	return 0;
}
//Purpose: to display invalid file path error message
//input parameters: char * wordListPath; the passed in wordlist path
//return value: void
void usage1(char * wordListPath){
	printf("Fatal error! %s file not found\n",wordListPath );
	printf("%s\n", "Program halted, Please verify the file path and try again." );
}
//Purpose: to display invalid min or max value error message
//input parameters: char * filePath; the file path of Task 1 which is the argv[0[]
//return value: void
void usage2(char * filePath){
	printf("Usage : %s <wordlist> <min> <max>\n",filePath );
	printf("        <wordlist> : A file path/name in which contains the password dictionary.\n");
	printf("        <min> : An integer value greater than 1.\n");
	printf("                This value represents the minimum length of the password to be processed.\n");
	printf("        <max> : An integer value greater than or equal to <min>.\n");
	printf("                <max> represents the maximum length of the password to be processed.\n");
}
//Purpose: to display the current time when the program started/ended
//input parameters: char * state; the state of the program when this function is called, either "started" or "ended"
//return value: void
void displayTime(char * state){
	/*	Usage of <time.h> and its functions:
			Huss, E., 1997. time.h. [Online] 
			Available at: https://www-s.acm.illinois.edu/webmonkeys/book/c_guide/2.15.html#strftime
			[Accessed 8 12 2017].
	*/
	time_t totalSec = time(NULL);//total no of secs since EPOCH
	struct tm * currTime = localtime(&totalSec); //store local time in struct declared in time.h
	char * result = (char *) malloc(20 * sizeof(char)); //allocate memory for result as it is just a pointer with no memory pointing to.Compiler will not allocate memory for char * like in main. 
	//OR SIMPLY char result[20];
	strftime(result, 20, "%Y:%m:%d %H:%M:%S",currTime);//E.g. 2017:11:12 12:30:48 Max is 20 characters including \0
	printf("Program %s at %s\n", state, result );

	free(result); //free result as not needed anymore

}
//Purpose: to crypt the plain word according to the hashing scheme
//input parameters: const char * hashType, char * plain, char * encryptionScheme
//return value: char *; a char pointer pointing to the encrypted string with hashtype, salt and hash value stored inside.
char * hash(const char * hashType, char * plain, char * encryptionScheme){
	char * cipherText;
	strcpy(encryptionScheme, hashType);
	strcat(encryptionScheme, salt);
	cipherText = crypt(plain, encryptionScheme);
	return cipherText;
}
//Purpose: the main function; execution starts here
//input parameters: int argc, char * argv[]; argc states the number of arguments took in from command line. argv is an array of char pointers pointing to different part of command line input, separated by a space.
//return value: 0 means success execution of the program.
int main(int argc, char * argv[]){

	//verify the arguments
	int vResult = verifyArg(argc, argv[1], argv[2], argv[3]);
	if (vResult != 0) {
		vResult == 1 ? usage1(argv[1]) : usage2(argv[0]); //1 means invalid file path, 2 means invalid arguments
		return 0;
	}

	//display start timing
	displayTime("started");

	//load the file
	char word[47]; //longest english word is 45 chars + carriage return + \0

	char * cipherText;
	
	char encryptionScheme[5];//Hashing scheme length is fixed at 4 + 1 string terminator. E.g. $6$$

	int wordCount = 0; 

	FILE * outputFile = fopen("mytab2411.txt", "r"); //try opening output file with read to check if the file exists
	
	if (outputFile != NULL){ //output file exists
		char choice;
		do{
			printf("%s","The default output file \"mytab2411.txt\" already exists. Continue will overwrite the existing file. Do you want to continue? (Y/N) --> " );
			scanf(" %c%*[^\n]", &choice);
		}while(toupper(choice) != 'Y' && toupper(choice)!= 'N');
		
		if (toupper(choice) == 'N' ) { //does not want to overwrite
			displayTime("ended");
			return 0;
		}
		//overwrite the existing file
		else outputFile = fopen("mytab2411.txt", "w+");
	}
	//outputfile does not exist, reopen it for writing
	else outputFile = fopen("mytab2411.txt", "w");
	
	//read in word by word
	while (fgets(word, 47, wordList) != NULL){
		strtok(word, "\n"); //Remove carriage return.
		if (strcmp(word, "\n") == 0) continue; //If the word list contains an empty line, do not process it. 
		if ((strlen(word)) < MIN || (strlen(word)) > MAX) continue; //word not within range, skip the iteration
		// -1 to minus away the carriage return.
		 
		//hash word using md5 mode
		cipherText = hash(hashType1, word, encryptionScheme);
		//output to mytab2411.txt
		
		fprintf(outputFile, "%s:%s\n",word, cipherText );
		//hash word using sha 512
		cipherText = hash(hashType2, word, encryptionScheme);

		//output to mytab2411.txt
		fprintf(outputFile, "%s:%s\n",word, cipherText );

		//increase word count
		wordCount++;
	}
	//repeat iteration as long as not EOF
	//clost input wordList & outputFile
	fclose(wordList);
	fclose(outputFile);
	//display word count
	printf("Total number of word processed     =>%10d\n", wordCount );
	printf("Total number of generated entries  =>%10d\n", wordCount * 2 );
	//display end time
	displayTime("ended");
	return 0;
}
