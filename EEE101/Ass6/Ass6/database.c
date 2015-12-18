#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

/********************************************************************************
=================================================================================
=================================================================================
==============================Strcture Declaration===============================
=================================================================================
=================================================================================
********************************************************************************/

typedef struct Database {
	struct Costomer* cosList;
	struct Account* accList;
	int SO_State; /*0 operation idle, 1 under operation, 2 main thread call to end, 3 thread terminated*/
} Db;

typedef struct Costomer {
	struct AccountRecord* accList;
	char name[30];
	char address[100];
	char telephone[20];
	char ID[20];
	struct Costomer* CosLast;
	struct Costomer* CosNext;
} Cos;

typedef struct Account{
	float balance;
	char ownerID[20];
	long accountNumber;
	int PIN;
	int state;/*1 when active, 0 when freezed*/
	struct StandOrderRecord* SORecList;
	struct OperationRecord* OPRecList;
	struct Account* AccLast;
	struct Account* AccNext;
} Acc;

/*For internal use*/
typedef struct StandOrderRecord {
	float amount;
	time_t timeAssigned;
	time_t timeLastOperation;
	long interval;
	time_t timeEnd;
	long dest;
	struct StandOrderRecord* SORecLast;
	struct StandOrderRecord* SORecNext;
} SORec;

/*For internal use*/
typedef struct OperationRecord {
	/*
	* record information:
	* 0 when withdraw
	* 1 when deposit
	* 2 when stand order out
	* 3 when stand order in
	* 4 when stand order out failed due to no enough balance
	* 5 when stand order failed due to destination account is freezed
	* 6 freeze account
	* 7 unfreeze account
	* 8 when stand order failed due to account is freezed
	*/
	int type;
	time_t time;
	float amount;
	char info[20];
	struct OperationRecord* OPRecLast;
	struct OperationRecord* OPRecNext;
} OPRec;

typedef struct AccountRecord {
	long acc;
	struct AccountRecord* AccRecLast;
	struct AccountRecord* AccRecNext;
} AccRec;

/********************************************************************************
=================================================================================
=================================================================================
==============================Function Declaration===============================
=================================================================================
=================================================================================
********************************************************************************/


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
- Db* db: the destination database
- Cos* cos: the destination costomer
- int PIN: the PIN number
return:
- The generated account
function:
- Add one account to the exist costomer
********************************************************************************/
Acc* db_AddAcc(Db* db, int PIN, Cos* cos);

/********************************************************************************
input:
- Acc* acc: the destination account
- long interval: the interval between each transfer
- long duration: the time this function will operate
- float amount: the amount to move
function:
- Add one stand order record to exsiting account
********************************************************************************/
void acc_AddSORec(Acc* acc, long interval, long duration, float amount, long dest);

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
void acc_Withdraw(Acc* acc, float amount);

/********************************************************************************
input:
- Cos* cos: the destination costomer
- Db* db: the database
return:
- float: the avarage balance of the costomer
function:
- Calculate and return the avarage balance of the costomer
********************************************************************************/
float cos_GetAvg(Cos* cos, Db* db);

/********************************************************************************
input:
- Db* db: database to print
- int mode: 1 to print all costomers
function:
- Print information in the database, for debug use
********************************************************************************/
void db_Print(Db* db, int mode);

/********************************************************************************
input:
- Cos* cos: costomer to print
- int mode: 
- - 1 to print all costomer information
- - 2 to print all costomer with account
function:
- Print information of the costomer
remarks:
- This is used as a debug tool
********************************************************************************/
void cos_Print(Cos* cos, int mode, Db* db);

/********************************************************************************
=================================================================================
=================================================================================
=============================Function Declaration================================
================================(Internal use)===================================
=================================================================================
=================================================================================
********************************************************************************/

/********************************************************************************
input:
- Db* db: database to put all the infomation
function:
- Read the database
********************************************************************************/
void db_Fread(Db* db);

/********************************************************************************
input:
- Db* db: database to link the costomer
- Cos* cos: the costomer to be added
function:
- Link one existing costomer to the database
********************************************************************************/
void db_LinkCos(Db* dest, Cos* cos);

/********************************************************************************
input:
- Db* db: database to write
function:
- Write all the content in the database into files
********************************************************************************/
void db_Fwrite(Db* db);

/********************************************************************************
input:
- Cos* cos: the costomer to write
- FILE* stream: the file to write costomer information to
- Db* db: the database
function:
- Write all the content of the costomer into the filestream
- Write the accounts linked to the costomer to individual files
********************************************************************************/
void cos_Fwrite(Cos* cos, FILE* stream, Db* db);

/********************************************************************************
input:
- Cos* cos: the costomer to read
-FILE* stream: the file to read costomer information from
- Db* db: the database
function:
- Read all the content of the costomer from the filestream
- Read the accounts linked to the costomer form other files
********************************************************************************/
void cos_Fread(Cos* cos, FILE* stream, Db* db);

/********************************************************************************
input:
- char str[]: the string to check
function:
- Delete all the enters (\n) in the string
********************************************************************************/
void utl_CheckString(char str[]);

/********************************************************************************
input:
- Acc* acc: the account to print to the interface
- int mode: 
- - 0 to print the account information
- - 1 to print the account information and operation record, stand order record
function:
- Print information of the account
********************************************************************************/
void acc_Print(Acc* acc, int mode);

/********************************************************************************
input:
- Db* dest: the database to link the account
- Acc* acc: the account to be linked
function:
- Link one existing account to the database
********************************************************************************/
void db_LinkAcc(Db* dest, Acc* acc);

/********************************************************************************
input:
- Acc* acc: the account to write
function:
- Write the account to the related file
********************************************************************************/
void acc_Fwrite(Acc* acc);

/********************************************************************************
input:
- Cos* cos: the costomer to be add the account
- long accNum: the account number of the account
function:
- Add a account record the an existing costomer
********************************************************************************/
void cos_AddAcc(Cos* cos, long accNum);

/********************************************************************************
input:
- Db* db: the database to add the account
- long accNum: account number of the account to be read
function:
- Read a account and add it to the database
********************************************************************************/
void acc_Fread(Db* db, long accNum);

/********************************************************************************
input:
- Acc* dest: the account to link the stand order record
- SORec* buffer: the stand order record to be added
function:
- Link one existing stand order record to the account
********************************************************************************/
void acc_LinkSORec(Acc* dest, SORec* buffer);

/********************************************************************************
input:
- Acc* dest: the account to link the operation record
- OPRec* buffer: the operation record to be added
function:
- Link one existing operation record to the account
********************************************************************************/
void acc_LinkOPRec(Acc* dest, OPRec* opRec);

/********************************************************************************
input:
- Acc* dest: the account to add the operation record
- int type: 
- - 0 withdraw
- - 1 deposit
- - 2 stand order out
- - 3 stand order in
- - 4 stand order out failed due to not enough balance
- - 5 when stand order failed due to destination account is freezed
- - 6 freeze account
- - 7 unfreeze account
- - 8 stand order failed due to account is freezed
- Others: other information of the record
function:
- Add one operation record to existing account
********************************************************************************/
void acc_AddOPRec(Acc* dest, float amount, char info[], int type);

/********************************************************************************
input:
- SORec* soRec: the stand order record to print
function:
- Print one stand order record to the interface
********************************************************************************/
void soRec_Print(SORec* soRec);

/********************************************************************************
input:
- SORec* soRec: the operation record to print
function:
- Print one operation record to the interface
********************************************************************************/
void opRec_Print(OPRec* opRec);

/********************************************************************************
input:
- char buffer[]: the buffer to contain the result, LOMGER THAN 26
- time_t time: the time variable to print
function:
- Change the content of the time variable into string and store into the buffer
remarks:
- the buffer is better to be one empty string
- the buffer is needed because it is not useful to return address of local variable
- so this function needs one string as buffer and return it
********************************************************************************/
char* utl_timeSprintf(char buffer[], time_t time);

/********************************************************************************
input:
- Acc* acc: the account to free
function:
- Free all the allocated memory linked to the account
********************************************************************************/
void acc_Free(Acc* acc);

/********************************************************************************
input:
- Cos* cos: the costomer to free
function:
- Free all the allocated memory linked to the costomer
********************************************************************************/
void cos_Free(Cos* cos);

/********************************************************************************
input:
- Acc* acc: the account to check the record
- Db* db: the databse
function:
- Check all the stand order record and operate
********************************************************************************/
void acc_CheckSO(Acc* acc, Db* db);

/********************************************************************************
input:
- SORec* soRec: the stand order record to check
- Acc* dest: the account containing the record
- Db* db: the databse
function:
- Check one stand order record and operate
********************************************************************************/
void soRec_CheckSO(SORec* soRec, Acc* dest, Db* db);

/********************************************************************************
input:
- Acc* acc: the account to take money from
- SORec* soRec: the record
- Db* db: the database
function:
- Operate one stand order record
********************************************************************************/
void acc_OperateSO(Acc* acc, SORec* soRec, Db* db);

/********************************************************************************
input:
- Db* db: the databse
function:
- Check all the stand order record in the databse and operate
********************************************************************************/
void db_CheckSO(Db* db);

/********************************************************************************
input:
- void* _db: the pointer to the database
function:
- Do all the initialization of stand order check
- Operate stand order check every second
********************************************************************************/
void db_SOCheckInit(void* _db);

/********************************************************************************
input:
- Acc* acc: account to freeze
function:
- Freeze the account
********************************************************************************/
void acc_Freeze(Acc* acc);

/********************************************************************************
input:
- Acc* acc: account to unfreeze
function:
- Unfreeze the account
********************************************************************************/
void acc_unFreeze(Acc* acc);

/********************************************************************************
=================================================================================
=================================================================================
==================================Sample code====================================
=================================================================================
=================================================================================
********************************************************************************/

int main() {
	Db* db = db_Load();
	Cos* cos;
	Acc* acc;
	cos = db_AddCos(db, "Suzhou", "320105199509260000", "Juntong Liu", "18661206723");
	acc = db_AddAcc(db, 123456, cos);
	acc_Deposit(acc, 200);
	acc_Freeze(acc);
	acc_AddSORec(acc, 1, 10, 1, acc->accountNumber);
	system("pause");
	printf("%s", cos->address);
	acc_Deposit(acc, 123);
	db_Print(db, 3);
	printf("\navg balance is %f", cos_GetAvg(cos, db));
	db_Free(db);
	system("pause");
}


/********************************************************************************
=================================================================================
=================================================================================
=================================Function code===================================
=================================================================================
=================================================================================
********************************************************************************/

Db* db_Load() {
	Db* db = malloc(sizeof(Db));
	db->cosList = NULL;
	/*declare an empty costomer as the first element in the list*/
	Cos* cos = malloc(sizeof(Cos));
	cos->accList = NULL;
	cos->CosLast = NULL;
	cos->CosNext = NULL;
	strcpy(cos->address, "");
	strcpy(cos->ID, "");
	strcpy(cos->name, "");
	strcpy(cos->telephone, "");
	db->cosList = cos;
	/*declare an empty account as the first element in the list*/
	Acc* acc = malloc(sizeof(Acc));
	acc->balance = 0;
	acc->AccLast = NULL;
	acc->AccNext = NULL;
	acc->accountNumber = 0;
	acc->OPRecList = NULL;
	acc->PIN = 0;
	acc->SORecList = NULL;
	acc->state = 0;
	strcpy(acc->ownerID, "");
	db->accList = acc;
	/*check if the file exists*/
	FILE* file = fopen(".\\data\\information.db", "r");
	/*if doesn't exist*/
	if (file == NULL) {
		/*create the database file*/
		system("mkdir data");
		file = fopen(".\\data\\information.db", "w+");
		fclose(file);
		db->SO_State = 0;
		/*start SO check service*/
		_beginthread(db_SOCheckInit, 0, (void*)db);
		return db;
	}
	/*if exist*/
	else {
		fclose(file);
		db_Fread(db);
		db->SO_State = 0;
		/*start SO check service*/
		_beginthread(db_SOCheckInit, 0, (void*)db);
		return db;
	}
}

void db_Fread(Db* db) {
	FILE* file = fopen(".\\data\\information.db", "r+");
	Cos* temp = malloc(sizeof(Cos));
	int i = 1;
	int p;
	/*check if there is content in the file*/
	if (fgetc(file) == EOF) {
		free(temp);
		return;
	}
	/*if there is record(s)*/
	/*check if the file is in use by other thread*/
	fseek(file, (-2)*sizeof(char), SEEK_END);
	fscanf(file, "%d", &p);
	while (p != -1) {
		Sleep(10);
		fseek(file, (-2)*sizeof(char), SEEK_END);
		fscanf(file, "%d", &p);
	}
	rewind(file);
	/*read the content*/
	/*read the first empty element*/
	fread(temp, sizeof(Cos), 1, file);
	free(temp);
	/*read other elements*/
	do {
		Cos* buffer = malloc(sizeof(Cos));
		cos_Fread(buffer, file, db);
		/*check if the program reaches the last empty element*/
		if (strcmp(buffer->ID, "") != 0) {
			db_LinkCos(db, buffer);
		}
		else {
			i = 0;
			free(buffer);
		}
	} while (i);
}

void db_LinkCos(Db* dest, Cos* cos) {
	Cos* buffer = dest->cosList;
	while (buffer->CosNext != NULL) {
		buffer = buffer->CosNext;
	}
	buffer->CosNext = cos;
	cos->CosLast = buffer;
}

void cos_LinkAccRec(Cos* dest, AccRec* accRec) {
	AccRec* buffer = dest->accList;
	while (buffer->AccRecNext != NULL) {
		buffer = buffer->AccRecNext;
	}
	buffer->AccRecNext = accRec;
	accRec->AccRecLast = buffer;
}

Cos* db_AddCos(Db* db, char address[], char ID[], char name[], char telephone[]) {
	Cos* buffer = malloc(sizeof(Cos));
	utl_CheckString(address);
	utl_CheckString(ID);
	utl_CheckString(name);
	utl_CheckString(telephone);
	/*set the first element in the account list to be empty*/
	AccRec* accRec = malloc(sizeof(AccRec));
	accRec->acc = 0;
	accRec->AccRecLast = NULL;
	accRec->AccRecNext = NULL;
	buffer->accList = accRec;
	/*assign input value*/
	strcpy(buffer->address, address);
	strcpy(buffer->name, name);
	strcpy(buffer->telephone, telephone);
	strcpy(buffer->ID, ID);
	buffer->CosLast = NULL;
	buffer->CosNext = NULL;
	db_LinkCos(db, buffer);
	/*update file content*/
	db_Fwrite(db);
	return buffer;
}

void cos_Fread(Cos* cos, FILE* stream, Db* db) {
	Cos* temp = malloc(sizeof(Cos));
	/*set the first account to be an empty account*/
	AccRec* accRec = malloc(sizeof(AccRec));
	accRec->acc = 0;
	accRec->AccRecLast = NULL;
	accRec->AccRecNext = NULL;
	cos->accList = accRec;
	/*read the information and assign*/
	fread(temp, sizeof(Cos), 1, stream);
	cos->CosLast = NULL;
	cos->CosNext = NULL;
	strcpy(cos->ID, temp->ID);
	strcpy(cos->name, temp->name);
	strcpy(cos->address, temp->address);
	strcpy(cos->telephone, temp->telephone);
	/*check it is the last empty customer record in the file*/
	if (strcmp(temp->ID, "") == 0) {
		cos->accList = NULL;
		free(accRec);
	}
	else {
		int i = 1;
		/*read the account record*/
		AccRec* temp = malloc(sizeof(AccRec));
		fread(temp, sizeof(AccRec), 1, stream);
		free(temp);
		while (i) {
			AccRec* buffer = malloc(sizeof(AccRec));
			fread(buffer, sizeof(AccRec), 1, stream);
			if (buffer->acc == 0) {
				i = 0;
				free(buffer);
			}
			else {
				buffer->AccRecLast = NULL;
				buffer->AccRecNext = NULL;
				cos_LinkAccRec(cos, buffer);
				acc_Fread(db, buffer->acc);
			}
		}
	}
}

void cos_Print(Cos* cos, int mode, Db* db) {
	if (mode < 0) {
		return;
	}
	printf("Name: %s\n", cos->name);
	printf("ID  : %s\n", cos->ID);
	printf("Tel : %s\n", cos->telephone);
	printf("Add : %s\n", cos->address);
	/*mode > 0 means more information about accounts should be presented*/
	if (mode > 0) {
		AccRec* buffer = cos->accList;
		Acc* acc;
		if (buffer->AccRecNext == NULL) {
			puts("No account.");
		}
		else {
			while (buffer->AccRecNext != NULL) {
				buffer = buffer->AccRecNext;
				acc = db_GetAcc(db, buffer->acc);
				acc_Print(acc, mode - 1);
			}
		}
	}
}

void db_Print(Db* db, int mode) {
	Cos* buffer = db->cosList;
	if (buffer->CosNext == NULL) {
		puts("No record");
	}
	else {
		while (buffer->CosNext != NULL) {
			buffer = buffer->CosNext;
			cos_Print(buffer, mode - 1, db);
			puts("");
		}
		puts("Print finished");
	}
}

void db_Free(Db* db) {
	Cos* bufferC = db->cosList;
	Acc* bufferA = db->accList;
	/*send information to ask the thread to terminate*/
	db->SO_State = 2;
	/*check if it has finised work*/
	while (db->SO_State != 3) {
		Sleep(10);
	}
	/*get to the last element*/
	while (bufferC->CosNext != NULL) {
		bufferC = bufferC->CosNext;
	}
	/*free all costomers*/
	while (bufferC->CosLast != NULL) {
		bufferC = bufferC->CosLast;
		cos_Free(bufferC->CosNext);
	}
	/*free the last element*/
	free(bufferC);
	/*get to the last element*/
	while (bufferA->AccNext != NULL) {
		bufferA = bufferA->AccNext;
	}
	/*free all costomers*/
	while (bufferA->AccLast != NULL) {
		bufferA = bufferA->AccLast;
		acc_Free(bufferA->AccNext);
	}
	/*free the last element*/
	free(bufferA);
}

void db_Fwrite(Db* db) {
	Cos* buffer = db->cosList;
	Cos empty = { NULL, "", NULL, NULL, "", "", "" };
	FILE* file = fopen(".\\data\\information.db", "r");
	char ch;
	/*check if the file is being used by other thread*/
	ch = fgetc(file);
	if (ch != EOF) {
		int p;
		fseek(file, (-2)*sizeof(char), SEEK_END);
		fscanf(file, "%d", &p);
		while (p != -1) {
			Sleep(100);
			fseek(file, (-2)*sizeof(char), SEEK_END);
			fscanf(file, "%d", &p);
		}
	}
	fclose(file);
	/*clear the file and write information*/
	file = fopen(".\\data\\information.db", "w+");
	cos_Fwrite(buffer, file, db);
	while (buffer->CosNext != NULL) {
		buffer = buffer->CosNext;
		cos_Fwrite(buffer, file, db);
	}
	cos_Fwrite(&empty, file, db);
	/*print the final mark for other thread to check*/
	fprintf(file, "%d", -1);
	fclose(file);
}

void cos_Fwrite(Cos* cos, FILE* stream, Db* db) {
	/*check if it is an empty customer*/
	if (cos->accList == NULL) {
		fwrite(cos, sizeof(Cos), 1, stream);
	}
	else {
		AccRec* index = cos->accList;
		AccRec empty = { 0, NULL, NULL };
		Acc* acc;
		fwrite(cos, sizeof(Cos), 1, stream);
		/*write all the account records*/
		fwrite(index, sizeof(AccRec), 1, stream);
		while (index->AccRecNext != NULL) {
			index = index->AccRecNext;
			fwrite(index, sizeof(AccRec), 1, stream);
			acc = db_GetAcc(db, index->acc);
			acc_Fwrite(acc);
		}
		/*print an empty record*/
		fwrite(&empty, sizeof(AccRec), 1, stream);
	}
}

Cos* db_GetCos(Db* db, char ID[]) {
	Cos* index = db->cosList;
	while (index->CosNext != NULL) {
		index = index->CosNext;
		if (strcmp(index->ID, ID) == 0) {
			return index;
		}
	}
	return NULL;
}

void utl_CheckString(char str[]) {
	int l = strlen(str);
	int i = 0;
	for (i = 0; i < l; i++) {
		if (str[i] == '\n') {
			str[i] = '\0';
		}
	}
}

Acc* db_AddAcc(Db* db, int PIN, Cos* cos) {
	long temp;
	Acc* test;
	Acc* buffer = malloc(sizeof(Acc));
	/*set the first element in the account & stand order list to be empty*/
	SORec* soRec = malloc(sizeof(SORec));
	OPRec* opRec = malloc(sizeof(OPRec));
	soRec->interval = 0;
	soRec->amount = 0;
	soRec->timeAssigned = 0;
	soRec->timeEnd = 0;
	soRec->timeLastOperation = 0;
	soRec->SORecLast = NULL;
	soRec->SORecNext = NULL;
	soRec->dest = 0;
	buffer->SORecList = soRec;
	opRec->amount = 0;
	opRec->OPRecLast = NULL;
	opRec->OPRecNext = NULL;
	opRec->type = -1;
	opRec->time = 0;
	strcpy(opRec->info, "");
	buffer->OPRecList = opRec;
	/*assign input value*/
	buffer->PIN = PIN;
	buffer->state = 1;
	buffer->AccLast = NULL;
	buffer->AccNext = NULL;
	temp = time(0);
	/*check if the account number duclicate with another account*/
	while (db_GetAcc(db, temp) != 0) {
		test = db_GetAcc(db, temp);
		temp++;
	}
	buffer->accountNumber = temp;
	buffer->balance = 0;
	strcpy(buffer->ownerID, "");
	strcpy(buffer->ownerID, cos->ID);
	db_LinkAcc(db, buffer);
	cos_AddAcc(cos, buffer->accountNumber);
	/*update file content*/
	db_Fwrite(db);
	acc_Fwrite(buffer);
	
	return buffer;
}

void db_LinkAcc(Db* dest, Acc* acc) {
	Acc* buffer = dest->accList;
	while (buffer->AccNext != NULL) {
		buffer = buffer->AccNext;
	}
	buffer->AccNext = acc;
	acc->AccLast = buffer;
}

void acc_Fwrite(Acc* acc) {
	char filename[25] = ".\\data\\";
	char accNumC[25];
	FILE* file;
	SORec* indexS = acc->SORecList;
	SORec emptyS = { 0, NULL, NULL, 0, 0, 0 };
	OPRec* indexO = acc->OPRecList;
	OPRec emptyO = { -1, "", 0, "", NULL, NULL };
	sprintf(accNumC, "%ld.db", acc->accountNumber);
	strcat(filename, accNumC);
	file = fopen(filename, "r");
	/*check if the file already exists*/
	if (file != NULL) {
		/*check if there are already records*/
		if (fgetc(file) != EOF) {
			int p;
			/*check if the file is occupied by other threads*/
			fseek(file, (-2)*sizeof(char), SEEK_END);
			fscanf(file, "%d", &p);
			while (p != -1) {
				Sleep(100);
				fseek(file, (-2)*sizeof(char), SEEK_END);
				fscanf(file, "%d", &p);
			}
		}
		fclose(file);
	}
	file = fopen(filename, "w+");
	fwrite(acc, sizeof(Acc), 1, file);
	/*print stand order record*/
	fwrite(indexS, sizeof(SORec), 1, file);
	while (indexS->SORecNext != NULL) {
		indexS = indexS->SORecNext;
		fwrite(indexS, sizeof(SORec), 1, file);
	}
	fwrite(&emptyS, sizeof(SORec), 1, file);
	/*print operation record*/
	fwrite(indexO, sizeof(OPRec), 1, file);
	while (indexO->OPRecNext != NULL) {
		indexO = indexO->OPRecNext;
		fwrite(indexO, sizeof(OPRec), 1, file);
	}
	fwrite(&emptyO, sizeof(OPRec), 1, file);
	fprintf(file, "%d", -1);
	fclose(file);
}

void cos_AddAcc(Cos* dest, long accNum) {
	AccRec* accRec = malloc(sizeof(AccRec));
	AccRec* buffer = dest->accList;
	/*initialize account record*/
	accRec->acc = accNum;
	accRec->AccRecLast = NULL;
	accRec->AccRecNext = NULL;
	/*get to the last account record*/
	while (buffer->AccRecNext != NULL) {
		buffer = buffer->AccRecNext;
	}
	/*link the record*/
	buffer->AccRecNext = accRec;
	accRec->AccRecLast = buffer;
}

void acc_Print(Acc* acc, int mode) {
	if (mode < 0) {
		return;
	}
	printf("Acc Num : %ld\n", acc->accountNumber);
	printf("Balance : %f\n", acc->balance);
	printf("Costomer: %s\n", acc->ownerID);
	printf("PIN     : %06d\n", acc->PIN);
	printf("State   : %d\n", acc->state);
	/*check if further information of operation and stand order record is needed*/
	if (mode == 1) {
		OPRec* index1 = acc->OPRecList;
		SORec* index2 = acc->SORecList;
		/*print operation records*/
		if (index1->OPRecNext != NULL) {
			while (index1->OPRecNext != NULL) {
				index1 = index1->OPRecNext;
				opRec_Print(index1);
			}
		}
		else {
			puts("No operation record");
		}
		/*print stand order records*/
		if (index2->SORecNext != NULL) {
			while (index2->SORecNext != NULL) {
				index2 = index2->SORecNext;
				soRec_Print(index2);
			}
		}
		else {
			puts("No stand order record");
		}
	}
}

Acc* db_GetAcc(Db* db, long accNum) {
	Acc* index = db->accList;
	while (index->AccNext != NULL) {
		index = index->AccNext;
		if (index->accountNumber == accNum) {
			return index;
		}
	}
	return NULL;
}

void acc_Fread(Db* db, long accNum) {
	char filename[25] = ".\\data\\";
	char accNumC[25];
	Acc* buffer = malloc(sizeof(Acc));
	SORec* temp1 = malloc(sizeof(SORec));
	OPRec* temp2 = malloc(sizeof(OPRec));
	SORec* temp3 = malloc(sizeof(SORec));
	OPRec* temp4 = malloc(sizeof(OPRec));
	FILE* file;
	int i = 1;
	int p;
	/*get filename linked to the account*/
	sprintf(accNumC, "%ld.db", accNum);
	strcat(filename, accNumC);
	/*check if it is used by other thread*/
	file = fopen(filename, "r+");
	fseek(file, (-2)*sizeof(char), SEEK_END);
	fscanf(file, "%d", &p);
	while (p != -1) {
		Sleep(100);
		fseek(file, (-2)*sizeof(char), SEEK_END);
		fscanf(file, "%d", &p);
	}
	rewind(file);
	/*initialize allocated memory*/
	fread(buffer, sizeof(Acc), 1, file);
	buffer->AccLast = NULL;
	buffer->AccNext = NULL;
	temp3->interval = 0;
	temp3->SORecLast = NULL;
	temp3->SORecNext = NULL;
	temp3->timeAssigned = 0;
	temp3->timeEnd = 0;
	temp3->timeLastOperation = 0;
	buffer->SORecList = temp3;
	temp4->amount = 0;
	temp4->OPRecLast = NULL;
	temp4->OPRecNext = NULL;
	temp4->type = -1;
	strcpy(temp4->info, "");
	temp4->time = 0;
	buffer->OPRecList = temp4;
	/*read stand order records*/
	fread(temp1, sizeof(SORec), 1, file);
	free(temp1);
	while (i) {
		SORec* buffer1 = malloc(sizeof(SORec));
		fread(buffer1, sizeof(SORec), 1, file);
		if (buffer1->timeAssigned == 0) {
			i = 0;
			free(buffer1);
		}
		else {
			buffer1->SORecLast = NULL;
			buffer1->SORecNext = NULL;
			acc_LinkSORec(buffer, buffer1);
		}
	}
	/*read operation records*/
	fread(temp2, sizeof(OPRec), 1, file);
	free(temp2);
	i = 1;
	while (i) {
		OPRec* buffer2 = malloc(sizeof(OPRec));
		fread(buffer2, sizeof(OPRec), 1, file);
		if (buffer2->type == -1) {
			i = 0;
			free(buffer2);
		}
		else {
			buffer2->OPRecLast = NULL;
			buffer2->OPRecNext = NULL;
			acc_LinkOPRec(buffer, buffer2);
		}
	}
	/*link the account to database*/
	db_LinkAcc(db, buffer);
}

void acc_LinkSORec(Acc* dest, SORec* soRec) {
	SORec* buffer = dest->SORecList;
	while (buffer->SORecNext != NULL) {
		buffer = buffer->SORecNext;
	}
	buffer->SORecNext = soRec;
	soRec->SORecLast = buffer;
}

void acc_LinkOPRec(Acc* dest, OPRec* opRec) {
	OPRec* buffer = dest->OPRecList;
	while (buffer->OPRecNext != NULL) {
		buffer = buffer->OPRecNext;
	}
	buffer->OPRecNext = opRec;
	opRec->OPRecLast = buffer;
}

void acc_AddOPRec(Acc* dest, float amount, char info[], int type) {	
	OPRec* buffer = malloc(sizeof(OPRec));
	time( &(buffer->time) );
	buffer->amount = amount;
	buffer->type = type;
	strcpy(buffer->info, info);
	buffer->OPRecLast = NULL;
	buffer->OPRecNext = NULL;
	acc_LinkOPRec(dest, buffer);
	acc_Fwrite(dest);
}

void acc_Deposit(Acc* acc, float amount) {
	acc->balance += amount;
	acc_AddOPRec(acc, amount, "", 1);
}

void acc_Withdraw(Acc* acc, float amount) {
	acc->balance -= amount;
	acc_AddOPRec(acc, amount, "", 0);
}

void soRec_Print(SORec* soRec) {
	char buffer[30];
	printf("interval   : %ld\n", soRec->interval);
	printf("start time : %s\n", utl_timeSprintf(buffer, soRec->timeAssigned));
	printf("end time   : %s\n", utl_timeSprintf(buffer, soRec->timeEnd));
	printf("last oprate: %s\n", utl_timeSprintf(buffer, soRec->timeLastOperation));
}

void opRec_Print(OPRec* opRec) {
	char buffer[30];
	printf("Record: ");
	/*check operation type and print*/
	switch (opRec->type)
	{
	case 0:
		puts("dithdraw");
		printf("Amount: %f\n", opRec->amount);
		break;
	case 1:
		puts("deposit");
		printf("Amount: %f\n", opRec->amount);
		break;
	case 2:
		puts("stand order out");
		printf("To    : %s\n", opRec->info);
		printf("Amount: %f\n", opRec->amount);
		break;
	case 3:
		puts("stand order in");
		printf("From  : %s", opRec->info);
		printf("Amount: %f\n", opRec->amount);
		break;
	case 4:
		puts("stand order failed due to not enough balance");
		printf("To    : %s\n", opRec->info);
		break;
	case 5:
		puts("stand order failed due to destination account freezed");
		printf("To    : %s\n", opRec->info);
		break;
	case 6:
		puts("Freeze");
		break;
	case 7:
		puts("Unfreeze");
		break;
	case 8:
		puts("stand order failed due to account freezed");
		break;
	default:
		puts("unknown record type");
		printf("To    : %s\n", opRec->info);
		break;
	}
	/*other information*/
	printf("Time  : %s\n", utl_timeSprintf(buffer, opRec->time));
}

char* utl_timeSprintf(char buffer[], time_t time) {
	sprintf(buffer, "%s", asctime(localtime(&time)));
	utl_CheckString(buffer);
	return buffer;
}

float cos_GetAvg(Cos* cos, Db* db) {
	int i = 0;
	float total = 0;
	float tempf;
	AccRec* index = cos->accList;
	Acc* temp;
	/*check if there have records*/
	if (index->AccRecNext == NULL) {
		return 0;
	}
	else {
		/*calculate average balance*/
		while (index->AccRecNext != NULL) {
			index = index->AccRecNext;
			temp = db_GetAcc(db, index->acc);
			tempf = temp->balance;
			total = total + tempf;
			i++;
		}
	}
	return total / i;
}

void acc_Free(Acc* acc) {
	SORec* bufferS = acc->SORecList;
	OPRec* bufferO = acc->OPRecList;
	/*move to the last element*/
	while (bufferS->SORecNext != NULL) {
		bufferS = bufferS->SORecNext;
	}
	/*free the stand order list*/
	while (bufferS->SORecLast != NULL) {
		bufferS = bufferS->SORecLast;
		free(bufferS->SORecNext);
	}
	/*free the last element*/
	free(bufferS);
	/*move to the last element*/
	while (bufferO->OPRecNext != NULL) {
		bufferO = bufferO->OPRecNext;
	}
	/*free the stand order list*/
	while (bufferO->OPRecLast != NULL) {
		bufferO = bufferO->OPRecLast;
		free(bufferO->OPRecNext);
	}
	/*free the last element*/
	free(bufferO);
	/*free the account*/
	free(acc);
}

void cos_Free(Cos* cos) {
	AccRec* buffer = cos->accList;
	/*move to the last element*/
	while (buffer->AccRecNext != NULL) {
		buffer = buffer->AccRecNext;
	}
	/*free the stand order list*/
	while (buffer->AccRecLast != NULL) {
		buffer = buffer->AccRecLast;
		free(buffer->AccRecNext);
	}
	/*free the last element*/
	free(buffer);
	/*free the customer*/
	free(cos);
}

void acc_AddSORec(Acc* acc, long interval, long duration, float amount, long dest) {
	SORec* buffer = malloc(sizeof(SORec));
	time(&(buffer->timeAssigned));
	buffer->timeLastOperation = buffer->timeAssigned;
	buffer->timeEnd = buffer->timeAssigned + duration;
	buffer->interval = interval;
	buffer->amount = amount;
	buffer->SORecLast = NULL;
	buffer->SORecNext = NULL;
	buffer->dest = dest;
	acc_LinkSORec(acc, buffer);
	/*refresh file content*/
	acc_Fwrite(acc);
}

void db_CheckSO(Db* db) {
	Acc* index = db->accList;
	while (index->AccNext != 0) {
		index = index->AccNext;
		acc_CheckSO(index, db);
	}
}

void acc_CheckSO(Acc* acc, Db* db) {
	SORec* index = acc->SORecList;
	while (index->SORecNext != 0) {
		index = index->SORecNext;
		soRec_CheckSO(index, acc, db);
	}
}

void soRec_CheckSO(SORec* soRec, Acc* dest, Db* db) {
	time_t tm;
	int i;
	/*if no operation left to do*/
	if ((soRec->timeEnd) - (soRec->timeLastOperation) <= soRec->interval) {
		return;
	}
	else {
		time(&tm);
		/*if current time exceeds the endtime and there are still operations left*/
		if (tm > soRec->timeEnd) {
			i = (soRec->timeEnd - soRec->timeLastOperation) / soRec->interval;
		}
		/*if current time deesn't exceed the endtime*/
		else {
			i = (tm - (soRec->timeLastOperation)) / soRec->interval;
		}
		/*operate the transfer for i times*/
		/*these calculation is not usefu if the system checks records every second*/
		/*but it is reserved for low frequency check, in order to do the skipped operations*/
		for (i; i > 0; i--) {
			acc_OperateSO(dest, soRec, db);
		}
		/*refresh last operation record*/
		soRec->timeLastOperation = tm;
	}
}

void acc_OperateSO(Acc* acc, SORec* soRec, Db* db) {
	/*if balance enough*/
	if (acc->balance > soRec->amount) {
		Acc* dest = db_GetAcc(db, soRec->dest);
		if (acc->state == 0) {
			char buffer[15];
			/*add operation records*/
			sprintf(buffer, "%ld", soRec->dest);
			acc_AddOPRec(acc, 0, buffer, 8);
		}
		else if (dest->state == 1) {
			char buffer[15];
			/*transfer between accounts and add operation records*/
			acc->balance -= soRec->amount;
			sprintf(buffer, "%ld", soRec->dest);
			acc_AddOPRec(acc, soRec->amount, buffer, 2);
			dest->balance += soRec->amount;
			acc_AddOPRec(dest, soRec->amount, buffer, 3);
		}
		else {
			char buffer[15];
			/*add operation records*/
			sprintf(buffer, "%ld", soRec->dest);
			acc_AddOPRec(acc, 0, buffer, 5);
		}	
	}
	else {
		char buffer[15];
		/*add operation records*/
		sprintf(buffer, "%ld", soRec->dest);
		acc_AddOPRec(acc, 0, buffer, 4);
	}
}

void db_SOCheckInit(void* _db) {
	Db* db = (Db*)_db;
	/*if the main thread doesn't ask to terminate, check every seconds*/
	while (db->SO_State == 0) {
		Sleep(1000);
		/*check records*/
		db_CheckSO(db);
		/*refresh file content*/
		db_Fwrite(db);
	}
	/*privide information to main thread before termination*/
	db->SO_State = 3;
	return;
}

void acc_Freeze(Acc* acc) {
	acc->state = 0;
	acc_AddOPRec(acc, 0, "", 6);
}

void acc_unFreeze(Acc* acc) {
	acc->state = 1;
	acc_AddOPRec(acc, 0, "", 7);
}