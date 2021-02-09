// createindex.c
#include "assign08.h"




long fileSize(FILE* input); //computes file size
int compare(const void* left, const void* right); //used by qsort
//int strcicmp(char const* a, char const* b);//compare string names ignore case



int main(void)
{

	FILE* fpData;
	FILE* fpIndexBalance;

	int		readCount;
	int		writeCount;
	int		dataRecordCount;
	int		indexRecordCount;
	long	filePos;
	char indexName[NAME_LENGTH];
	char exeName[NAME_LENGTH];
	char datName[NAME_LENGTH];

	IndexHeader	indexHeader;
	IndexRecord* indexRecords;
	ClientData client = { 0, "", "", 0.0, 0.0 }; //create client data with default data

	indexKey = BALANCE;//sort index file on balance
	printf("%s", "Enter exename, dataFileName and IndexFileName respectively, seperate by space\n ? ");
	scanf("\n%s%s%s", exeName, datName, indexName);

	if (strcmp(exeName, APP_NAME) != 0 && strcmp(datName, "accounts.dat") != 0 && strcmp(indexName, "accounts.idx") != 0) {
		printf("Error opening the file : No such file or directory\n");
		return 1;
	}
	else {




		// open data file for reading
		if ((fpData = fopen("accounts.dat", "rb")) == NULL)
		{
			printf("ERROR - can not open data file.\n");
			return 1;
		}


		// open index file for writing
		if ((fpIndexBalance = fopen("accounts.idx", "wb")) == NULL)
		{
			printf("ERROR - can not open/create index file.\n");
			return 1;
		}

		// calculate record count in data file
		dataRecordCount = fileSize(fpData) / sizeof(ClientData);

		// allocate an array to hold an equal number
		//  of index records
		indexRecords = (IndexRecord*)calloc(dataRecordCount, sizeof(IndexRecord));

		// populate index header record
		indexHeader.idxKey = indexKey;
		strcpy(indexHeader.appName, APP_NAME);
		indexHeader.recCount = dataRecordCount;

		rewind(fpData);
		filePos = ftell(fpData);
		indexRecordCount = 0;

		// Read first data record
		readCount = fread(&client, sizeof(ClientData), 1, fpData);

		// Process all data records
		while (!feof(fpData) && (readCount == 1))
		{
			// populate index record from data record 
			indexRecords[indexRecordCount].key.bal = client.balance;
			indexRecords[indexRecordCount].filePos = filePos;

			// added a record - increment to next element
			indexRecordCount++;

			// store file pos before next read advances it
			filePos = ftell(fpData);

			// read next record
			readCount = fread(&client, sizeof(ClientData), 1, fpData);
		}

		// Sort array of index records in memory
	   //  based on return value of compare function
		qsort(indexRecords, indexRecordCount, sizeof(IndexRecord), compare);

		printf("Index Header Size: %d\nIndex Record Size: %d\n\n"
			, sizeof(IndexHeader), sizeof(IndexRecord));

		printf("Creating Index File\n");

		// write Index Header to file
		writeCount = fwrite(&indexHeader, sizeof(IndexHeader), 1, fpIndexBalance);

		// Write Index Records to file
		writeCount = fwrite(indexRecords, sizeof(IndexRecord), indexRecordCount, fpIndexBalance);

		printf("%d Records Processed\n", writeCount);

		printf("\nFileSize: %d\n\n", fileSize(fpIndexBalance));//print file size 

		// release this memory and set it free
		free(indexRecords);

		// close files and go home
		fclose(fpData);
		fclose(fpIndexBalance);
	}

	_getch();
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
	return (pLeft->key.bal > pRight->key.bal) - (pLeft->key.bal < pRight->key.bal);

}

/*
int strcicmp(char const* a, char const* b)
{
	for (;; a++, b++) {
		int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
		if (d != 0 || !*a)
			return d;
	}
}*/