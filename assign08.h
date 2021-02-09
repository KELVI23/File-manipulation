#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#define APP_NAME "Assign8"
#define NAME_LENGTH 15

typedef enum { ACCOUNT, FIRST, LAST, BALANCE, PAYMENT } IndexKey;

typedef union
{
	int accnt;
	char last_N[NAME_LENGTH];
	char first_N[NAME_LENGTH];
	double bal;
	double payment;
} KeyType;

typedef struct
{
	int acctNum;
	char lastName[NAME_LENGTH];
	char firstName[NAME_LENGTH];
	double balance;
	double lastPaymentAmount;
} ClientData;

typedef struct
{
	KeyType		key;
	long		filePos;
} IndexRecord;

typedef struct
{
	IndexKey	idxKey;
	char		appName[20];
	int			recCount;
} IndexHeader;

IndexKey indexKey;