#include "assign08.h" 

long fileSize(FILE* input); //compute filesize
int compare(const void* left, const void* right); //used by qsort
void checkArgs(int);
void checkInputFile(char* File);
void checkOutputFile(char* File);
void createIndexHeader(int iDataRecordCount);
void createIndexFile(int iDataRecordCount, long filePos);


int main(int argc, char* argv[])
{
	int readCount;
	int writeCount;
	int dataRecordCount;
	int indexRecordCount;
	long filePos;
	char indexName[NAME_LENGTH];
	char exeName[NAME_LENGTH];
	char datName[NAME_LENGTH];

	IndexHeader 	indexHeader;
	IndexRecord* indexRecords;
	ClientData client = { 0,  "",  "",  0.0, 0.0 }; //create client data with default data

	checkArgs(argc);
	checkInputFile(argv[1]);
	checkOutputFile(argv[2]);

	dataRecordCount = checkRecordCount();//check number of recors in file
	filePos = ftell( fInputPtr );
	createIndexHeader( dataRecordCount );
	createIndexFile( dataRecordCount, filePos );

	fclose( fInputPtr );
	fclose( fIndexPtr );

	return 0;
}

// calculates the size of a file
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

// function that qsort points to
int compare(const void* left, const void* right)
{
	// cast pointer parameters to IndexRecord
	IndexRecord* pLeft = (IndexRecord*)left;
	IndexRecord* pRight = (IndexRecord*)right;

	// would need to be updated if sorting on another key
	//return strcmp((pLeft->key.last_N), (pRight->key.last_N));

	return (pLeft->key.bal) - (pRight->key.bal);
}

// Check the number of records in the data file
int checkRecordCount(void)
{
	int iRecordCount = 0;
	iRecordCount = fileSize(fInputPtr) / sizeof(ClientData);
	rewind(fInputPtr);

	return iRecordCount;
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
void checkOutputFile( char* sFileName )
{
		if (( fIndexPtr = fopen(sFileName, "wb" )) == NULL)
		{
			printf("%s could not be opened.\n", sFileName);
			exit(EXIT_FAILURE);
		}
	}


// Write the index header to file
void createIndexHeader( int dataRecordCount )
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

//Write the indexes to file
void createIndexFile(int dataRecordCount, long filePos)
{
	int iCount;
	int iReadCount;
	int iWriteCount;
	int iIndexRecordCount = 0;

	ClientData client = { 0,  "",  "",  0.0, 0.0 }; //create client data with default data
	IndexRecord* indexRecords;

	// allocate an array to hold an equal number
	// of index records
	indexRecords = (IndexRecord*)calloc(dataRecordCount, sizeof(IndexRecord));

	// Read first data record
	iReadCount = fread(&client, sizeof(ClientData), 1, fInputPtr);

	// Process all data records
	while (!feof(fInputPtr) && (iReadCount == 1))
	{
		// populate index record from data record
		indexRecords[iIndexRecordCount].key.bal =  client.balance;
		indexRecords[iIndexRecordCount].filePos = filePos;

		// added a record - increment to next element
		iIndexRecordCount++;

		// store file pos before next read advances it
		filePos = ftell(fInputPtr);

		// read next record
		iReadCount = fread(&client, sizeof(ClientData), 1, fInputPtr);
	}

	// Sort array of index records in memory
	// based on return value of compare function
	qsort(indexRecords, iIndexRecordCount, sizeof(IndexRecord), compare);

	printf("Index Header Size: %d\nIndex Record Size: %d\n\n"
		, sizeof(IndexHeader), sizeof(IndexRecord));

	printf("Creating Index File\n\n");
	printf("Index sorted by account balance:\n\n");
	for (iCount = 0; iCount < iIndexRecordCount; iCount++)
	{
		printf("%s\n", &indexRecords[iCount].key.bal); 
	}

	// Write Index Records to file
	iWriteCount = fwrite(indexRecords, sizeof(IndexRecord), iIndexRecordCount, fIndexPtr);

	printf("\n%d Records Processed\n", iWriteCount);

	// release this memory and set it free
	free(indexRecords);
}