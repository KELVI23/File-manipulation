// display.c

#include "assign08.h"



long fileSize(FILE* input); //computes file size
void instructions(void);
void balanceDescendingOrder(FILE* fpData);
void naturalOrder(FILE* fpData);
void checkArgs(int);
void checkInputFile(char* File);
void checkOutputFile(char* File);





int main(int argc, char* argv[])
{

	FILE* fpData;



	void(*pfunc[2]) (FILE * fpData);//defines a function pointer

// assign a function name to each of the function pointers in the array of function pointers
	pfunc[0] = naturalOrder;
	pfunc[1] = balanceDescendingOrder;

		instructions(); // display the menu
		unsigned int choice; // user's choice

		// loop while user does not choose 3

		int timeToExit = 0;  // false to begin with
		do {
			printf("%s", "? ");
			scanf("%u", &choice);

			switch (choice) {
			case 1: //natural order
				(*pfunc[0]) (&fpData);
				break;

			case 2: //balance descending order
				//balanceDescendingOrder(&fpData);
				(*pfunc[1]) (&fpData);
				break;

			case 3: // time to go home
				timeToExit = 1; // now set to true
				break;

			default:
				puts("Invalid choice.\n");
				instructions();
				break;
			} // end switch

		} while (!timeToExit);

		puts("End of run.");
		_getch();
		return 0;
}



// display program instructions to user
void instructions(void)

{
	puts("Enter your choice:\n"
		"   1 to display in Natural order.\n"
		"   2 to display  Account Balance Descending order.\n"
		"   3 to exit.");

}


void balanceDescendingOrder(FILE* fpData)
{
	FILE* fpIndexBalance;
	int		recCount;
	int		readCount;

	ClientData client = { 0, "", "", 0.0, 0.0 }; //create client data with default 

	IndexHeader idxHeader;
	IndexRecord idxTemp;

	// open data file for reading
	if ((fpData = fopen("accounts.dat", "rb")) == NULL)
	{
		printf("ERROR - can not open data file.\n");
		return 1;
	}


	// open index file for writing
	if ((fpIndexBalance = fopen("accounts.idx", "rb")) == NULL)
	{
		printf("ERROR - can not open/create index file.\n");
		return 1;
	}

	// calculate record count in data file
	recCount = fileSize(fpData) / sizeof(ClientData);

	// read header record in index file
	readCount = fread(&idxHeader, sizeof(IndexHeader), 1, fpIndexBalance);

	// check that application versions match
	if (strcmp(idxHeader.appName, APP_NAME) != 0)
	{
		printf("ERROR - Data and Index Files Created by Different Application Versions\n");
		fclose(fpData);
		fclose(fpIndexBalance);
		return 1;
	}

	// check that record count of data file matches index header
	if (idxHeader.recCount != recCount)
	{
		printf("ERROR - Data and Index Files Have a Record Count Mismatch\n");
		fclose(fpData);
		fclose(fpIndexBalance);
		return 1;
	}

	printf("Data File Read in Indexed Order (Balance)\n\n");
	printf("%-20s%-20s%-20s%-20s%-20s\n", "Account number", "First Name", "Last Name", "Balance", "Payment Amount");
	printf("---------------------------------------------------------------------------------------------------\n");

	// read first index record
	readCount = fread(&idxTemp, sizeof(IndexRecord), 1, fpIndexBalance);


	// continue processing all index records
	while ((!feof(fpIndexBalance)) && (readCount == 1))
	{
		// seek record in data file based on file position
		//  stored in previously read index record
		if (fseek(fpData, idxTemp.filePos, SEEK_SET) != 0)
		{
			printf("Seek Error\n");
			fclose(fpData);
			fclose(fpIndexBalance);
			return 1;
		}

		// read data record and output
		fread(&client, sizeof(ClientData), 1, fpData);
		printf("%-20d%-20s%-20s%-20.2f%-20.2f\n", client.acctNum, client.firstName, client.lastName, client.balance, client.lastPaymentAmount);

		// read next index record
		readCount = fread(&idxTemp, sizeof(IndexRecord), 1, fpIndexBalance);
	}

	// close files and go home
	fclose(fpData);
	fclose(fpIndexBalance);

}

long fileSize(FILE* input)
{
	long orgPos;
	long startPos;
	long endPos;

	orgPos = ftell(input);          // save orig file position
	rewind(input);
	startPos = ftell(input);
	fseek(input, 0, SEEK_END);
	endPos = ftell(input);
	fseek(input, orgPos, SEEK_SET); // restore orig position

	return(endPos - startPos);
}


void naturalOrder(FILE* fpData)
{
	ClientData client = { 0, "", "", 0.0, 0.0 }; //create client data with default 


	if ((fpData = fopen("accounts.dat", "rb")) == NULL)
	{
		printf("File could not be opened.\n");
		return 1;
	}

	printf("Data File Read in Natural Order:\n\n");
	printf("%-20s%-20s%-20s%-20s%-20s\n", "Account number", "First Name", "Last Name", "Balance", "Payment Amount");
	printf("---------------------------------------------------------------------------------------------------\n");

	fread(&client, sizeof(ClientData), 1, fpData);

	while (!feof(fpData))
	{
		printf("%-20d%-20s%-20s%-20.2f%-20.2f\n", client.acctNum, client.firstName, client.lastName, client.balance, client.lastPaymentAmount);
		fread(&client, sizeof(ClientData), 1, fpData);
	}

	fclose(fpData);
}
//Check number of command line arguments
void checkArgs(int iNumber)
{
	if (iNumber < 3)
	{
		printf("There are too few arguments.\n");
		exit(EXIT_FAILURE);
	}

	if (iNumber > 3)
	{
		printf("There are too many arguments.\n");
		exit(EXIT_FAILURE);
	}
}

// Check if input file exists
void checkInputFile(char* sFileName)
{
	if ((fInputPtr = fopen(sFileName, "rb")) == NULL)
	{
		printf("%s could not be opened.\n", sFileName);
		exit(EXIT_FAILURE);
	}
}

// Check if output file exists
void checkOutputFile(char* sFileName)
{
	if ((fIndexPtr = fopen(sFileName, "wb")) == NULL)
	{
		printf("%s could not be opened.\n", sFileName);
		exit(EXIT_FAILURE);
	}
}


// Write the index header to file
void createIndexHeader(int dataRecordCount)
{
	IndexHeader	 indexHeader;

	// populate index header record
	indexKey = BALANCE;//sort index file on balance
	indexHeader.idxKey = indexKey;
	strcpy(indexHeader.appName, APP_NAME);
	indexHeader.recCount = dataRecordCount;

	// write Index Header to file
	fwrite(&indexHeader, sizeof(IndexHeader), 1, fIndexPtr);
}
