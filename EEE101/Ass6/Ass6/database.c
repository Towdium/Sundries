#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct Database {
	struct Costomer* cosList;
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
	long timeAssigned;
	long timeLastOperation;
	long interval;
	struct StandOrderRecord* SORecLast;
	struct StandOrderRecord* SORecNext;
} SORec;

/*For internal use*/
typedef struct OperationRecord {
	/*0 when withdraw, 1 when deposit*/
	/*2 when stand order in, 3 when stand order out*/
	int type;
	char time[20];
	float amount;
	struct OperationRecord* OPRecLast;
	struct OperationRecord* OPRecNext;
} OPRec;

typedef struct AccountRecord {
	long acc;
	struct AccountRecord* AccRecLast;
	struct AccountRecord* AccRecNext;
} AccRec;


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
Acc* cos_AddAcc(Cos* cos,int PIN);

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
- int mode: 0 to print all costomer information
function:
- Print information of the costomer
********************************************************************************/
void cos_Print(Cos* cos, int mode);


/*for internal use*/

void db_Fread(Db* db);
void db_LinkCos(Db* dest, Cos* cos);
void db_Fwrite(Db* db);
void cos_Fwrite(Cos* cos, FILE* stream);
void cos_Fread(Cos* cos, FILE* stream);
void checkString(char str[]);
void db_AddAcc(int PIN, long accNum);

/*sample code*/
int main() {
	Db* db = db_Load();
	db_AddCos(db, "Suzhou", "320105199509260000", "Juntong Liu", "18661206723");
	db_AddCos(db, "Nanjing", "320100000000000000", "Wole Gequ", "18318312345");
	db_Print(db, 1);
	db_Free(db);
	system("pause");
}


/*function code*/

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
	db->cosList = cos;
	/*check if the file exists*/
	FILE* file = fopen(".\\data\\information.db", "r");
	/*if doesn't exist*/
	if (file == NULL) {
		/*create the database file*/
		system("mkdir data");
		file = fopen(".\\data\\information.db", "w+");
		fclose(file);
		return db;
	}
	/*if exist*/
	else {
		fclose(file);
		db_Fread(db);
		return db;
	}
}

void db_Fread(Db* db) {
	FILE* file = fopen(".\\data\\information.db", "r+");
	Cos* temp = malloc(sizeof(Cos));
	int i = 1;
	if (fgetc(file) == EOF) {
		free(temp);
		return;
	}
	fseek(file, -1, SEEK_CUR);
	fread(temp, sizeof(Cos), 1, file);
	free(temp);
	do {
		Cos* buffer = malloc(sizeof(Cos));
		cos_Fread(buffer, file);
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
	checkString(address);
	checkString(ID);
	checkString(name);
	checkString(telephone);
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

void cos_Fread(Cos* cos, FILE* stream) {
	Cos* temp = malloc(sizeof(Cos));
	AccRec* accRec = malloc(sizeof(AccRec));
	accRec->acc = 0;
	accRec->AccRecLast = NULL;
	accRec->AccRecNext = NULL;
	cos->accList = accRec;
	fread(temp, sizeof(Cos), 1, stream);
	cos->CosLast = NULL;
	cos->CosNext = NULL;
	strcpy(cos->ID, temp->ID);
	strcpy(cos->name, temp->name);
	strcpy(cos->address, temp->address);
	strcpy(cos->telephone, temp->telephone);
	if (strcmp(temp->ID, "") == 0) {
		cos->accList == NULL;
	}
	else {
		int i = 1;
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
				cos_LinkAccRec(cos, buffer);
			}
		}
	}
}

void cos_Print(Cos* cos, int mode) {
	if (mode < 0) {
		return;
	}
	printf("Name: %s\n", cos->name);
	printf("ID  : %s\n", cos->ID);
	printf("Tel : %s\n", cos->telephone);
	printf("Add : %s\n", cos->address);
}

void db_Print(Db* db, int mode) {
	Cos* buffer = db->cosList;
	if (buffer->CosNext == NULL) {
		puts("No record");
	}
	else {
		while (buffer->CosNext != NULL) {
			buffer = buffer->CosNext;
			cos_Print(buffer, mode - 1);
			puts("");
		}
		puts("Finished");
	}
}

void db_Free(Db* db) {
	Cos* buffer = db->cosList;
	while (buffer->CosNext != NULL) {
		buffer = buffer->CosNext;
	}
	while (buffer->CosLast != NULL) {
		buffer = buffer->CosLast;
		free(buffer->CosNext);
	}
	free(buffer);
	free(db);
}

void db_Fwrite(Db* db) {
	Cos* buffer = db->cosList;
	Cos empty = { NULL, "", NULL, NULL, "", "", "" };
	FILE* file = fopen(".\\data\\information.db", "w+");
	cos_Fwrite(buffer, file);
	while (buffer->CosNext != NULL) {
		buffer = buffer->CosNext;
		cos_Fwrite(buffer, file);
	}
	cos_Fwrite(&empty, file);
	fclose(file);
}

void cos_Fwrite(Cos* cos, FILE* stream) {
	if (cos->accList == NULL) {
		fwrite(cos, sizeof(Cos), 1, stream);
	}
	else {
		AccRec* index = cos->accList;
		AccRec empty = { 0, NULL, NULL };
		fwrite(cos, sizeof(Cos), 1, stream);
		while (index->AccRecNext != NULL) {
			fwrite(index, sizeof(AccRec), 1, stream);
			index = index->AccRecNext;
		}
		fwrite(index, sizeof(AccRec), 1, stream);
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
}

void checkString(char str[]) {
	int l = strlen(str);
	int i = 0;
	for (i = 0; i < l; i++) {
		if (str[i] == '\n') {
			str[i] = '\0';
		}
	}
}

/*unfinished*/
Acc* cos_AddAcc(Cos* cos, int PIN) {
	Acc* acc = (Acc*)malloc(sizeof(Acc));
	acc->accountNumber = time(0);
	acc->PIN = PIN;
	acc->state = 1;
	acc->AccLast = NULL;
	acc->AccNext = NULL;
	acc->OPRecList = NULL;
	acc->SORecList = NULL;
	return acc;
}