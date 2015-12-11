#include <stdio.h>

typedef struct Database {
	FILE* dbFile;
	Cos* cosList;
} Db;

typedef struct Costomer {
	Acc* accList;
	char name[30];
	char address[100];
	char telephone[20];
	char ID[20];
	SORec* SORecList;
	OPRec* OPRecList;
	Cos* CosLast;
	Cos* CosNext;
} Cos;

typedef struct Account {
	long accountNumber;
	int PIN;
	int state;/*1 when active, 0 when freezed*/
	SORec* SORecList;
	OPRec* OPRecList;
	Acc* AccLast;
	Acc* AccNext;
} Acc;

/*For internal use*/
typedef struct StandOrderRecord {
	long timeAssigned;
	long timeLastOperation;
	long interval;
	SORec* SORecLast;
	SORec* SORecNext;
} SORec;

/*For internal use*/
typedef struct OperationRecord {
	/*0 when withdraw, 1 when deposit, 2 when stand order in, 3 when stand order out*/
	int type;
	char time[20];
	float amount;
	OPRec* OPRecLast;
	OPRec* OPRecNext;
} OPRec;


/********************************************************************************
return:
- Db*: the loaded database
function:
- Load the exist database, or creat when there isn't
remarks:
- This should be used once the program runs
********************************************************************************/
Db* db_Load();

/********************************************************************************
input:
- Db* db: pointer to the database to free
function:
- Free all the allocated memory to the database
remarks:
- Highly suggested to run before close the program
********************************************************************************/
void db_Free(Db* db);

/********************************************************************************
input:
- Db* db: pointer to the database
- char ID[]: string of ID
return:
- Cos*: the costomer found or NULL when not found
function:
- Search and return the costomer with corresponding ID in the database
- Return NULL when no such costomer found
********************************************************************************/
Cos* db_GetCos(Db* db, char ID[]);

/********************************************************************************
input:
- Db* db: pointer to the database
- long accNum: account number
return:
- Acc*: the account found or NULL when not found
function:
- Search and return the account with corresponding account number in the database
- Return NULL when no such costomer found
********************************************************************************/
Acc* db_GetAcc(Db* db, long accNum);

/********************************************************************************
input:
- Db* db: pointer to the database
- long accNum: account number
return:
- Acc*: the account found or NULL when not found
function:
- Search and return the account with corresponding account number in the costomer
- Return NULL when no such costomer found
********************************************************************************/
Acc* cos_GetAcc(Cos* cos, long accNum);

/********************************************************************************
input:
- Db* db: pointer to the database
- others: costomer information
return:
- Cos*: generated costomer
function:
- Add one costomer to the exist database
- Remenber to check if there is already a costomer with the ID. Use db_GetCos
********************************************************************************/
Cos* db_AddCos(Db* db, char address[], char ID[], char name[], char telephone[]);

/********************************************************************************
input:
- Cos* cos: the destination costomer
- int PIN: the PIN number
return:
- Cos*: generated account
function:
- Add one account to the exist costomer
********************************************************************************/
Acc* cos_AddAcc(Cos* cos, int PIN);

/********************************************************************************
input:
- Acc* acc: the destination account
- long interval: the interval between each transfer
- float amount: the amount to move
function:
- Add one stand order record to exsiting account
********************************************************************************/
void acc_AddSORec(Acc* acc, long interval, float amount);

/********************************************************************************
input:
- Acc* acc: the destination account
- float amount: the amount to deposit
function:
- Deposit money to exsiting account
remarks:
- This function will automatically add operation record
********************************************************************************/
void acc_Deposit(Acc* acc, float amount);

/********************************************************************************
input:
- Acc* acc: the destination account
- float amount: the amount to withdraw
function:
- Withdraw money to exsiting account
remarks:
- Remember to check if there is enough balance
- This function will automatically add operation record
********************************************************************************/
int acc_Withdraw(Acc* acc, float amount);

/********************************************************************************
input:
- Cos* cos: the destination costomer
return:
- float: the avarage balance of the costomer
function:
- Calculate and return the avarage balance of the costomer
********************************************************************************/
float cos_GetAvg(Cos* cos);