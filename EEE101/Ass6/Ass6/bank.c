/****************************************************************************
Name: 
	bank manage system
File Name:
	bank.c
Copyright:
	Free
Author: 
	Chenxi Xia, Juntong Liu, Jiaxu Meng, Huiqian Ying, Xiaocheng Yang
Description:
	A bank information system to manage the banking affairs.
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <process.h>
#define MA_PW 000001  /*HERE TO CHANGE MANAGER PASSWORD*/
#define CLK_PW 654321 /*HERE TO CHANGE CLERK   PASSWORD*/

/********************************************************************************
=================================================================================
=================================================================================
==============================Strcture Declaration===============================
=================================================================================
=================================================================================
********************************************************************************/

typedef struct Database {
	struct Customer* cusList;
	struct Account* accList;
	int SO_State; /*0 operation idle, 1 under operation, 2 main thread call to end, 3 thread terminated*/
} Db;

typedef struct Customer {
	struct AccountRecord* accList;
	char name[30];
	char address[100];
	char telephone[20];
	char ID[20];
	struct Customer* CusLast;
	struct Customer* CusNext;
} Cus;

typedef struct Account{
	double balance;
	char ownerID[20];
	long accountNumber;
	int PIN;
	int state;/*1 when active, 0 when freezed*/
	struct StandOrderRecord* SORecList;
	struct OperationRecord* OPRecList;
	struct Account* AccLast;
	struct Account* AccNext;
} Acc;

typedef struct StandOrderRecord {
	double amount;
	time_t timeAssigned;
	time_t timeLastOperation;
	long interval;
	time_t timeEnd;
	long dest;
	struct StandOrderRecord* SORecLast;
	struct StandOrderRecord* SORecNext;
} SORec;

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
	* 9 when stand order failed dut to account not found
	*/
	int type;
	time_t time;
	double amount;
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
- Cus*: the customer found or NULL when not found
function:
- Search and return the customer with corresponding ID in the database
- Return NULL when no such customer found
********************************************************************************/
Cus* db_GetCus(Db* db, char ID[]);

/********************************************************************************
input:
- Db* db: pointer to the database
- long accNum: account number
return:
- Acc*: the account found or NULL when not found
function:
- Search and return the account with corresponding account number in the database
- Return NULL when no such customer found
********************************************************************************/
Acc* db_GetAcc(Db* db, long accNum);

/********************************************************************************
input:
- Db* db: pointer to the database
- others: customer information
return:
- Cus*: generated customer
function:
- Add one customer to the exist database
- Remenber to check if there is already a customer with the ID. Use db_GetCus
********************************************************************************/
Cus* db_AddCus(Db* db, char address[], char ID[], char name[], char telephone[]);

/********************************************************************************
input:
- Db* db: the destination database
- Cus* cus: the destination customer
- int PIN: the PIN number
return:
- The generated account
function:
- Add one account to the exist customer
********************************************************************************/
Acc* db_AddAcc(Db* db, int PIN, Cus* cus);

/********************************************************************************
input:
- Acc* acc: the destination account
- long interval: the interval between each transfer
- long duration: the time this function will operate
- double amount: the amount to move
function:
- Add one stand order record to exsiting account
********************************************************************************/
void acc_AddSORec(Acc* acc, long interval, long duration, double amount, long dest);

/********************************************************************************
input:
- Acc* acc: the destination account
- double amount: the amount to deposit
function:
- Deposit money to exsiting account
remarks:
- This function will automatically add operation record
********************************************************************************/
void acc_Deposit(Acc* acc, double amount);

/********************************************************************************
input:
- Acc* acc: the destination account
- double amount: the amount to withdraw
function:
- Withdraw money to exsiting account
remarks:
- Remember to check if there is enough balance
- This function will automatically add operation record
********************************************************************************/
void acc_Withdraw(Acc* acc, double amount);

/********************************************************************************
input:
- Cus* cus: the destination customer
- Db* db: the database
return:
- double: the avarage balance of the customer
function:
- Calculate and return the avarage balance of the customer
********************************************************************************/
double cus_GetAvg(Cus* cus, Db* db);

/********************************************************************************
input:
- Db* db: database to print
- int mode: 1 to print all customers
function:
- Print information in the database, for debug use
********************************************************************************/
void db_Print(Db* db, int mode);

/********************************************************************************
input:
- Cus* cus: customer to print
- int mode: 
- - 1 to print all customer information
- - 2 to print all customer with account
function:
- Print information of the customer
remarks:
- This is used as a debug tool
********************************************************************************/
void cus_Print(Cus* cus, int mode, Db* db);

/********************************************************************************
input:
- Db* db: database to put all the infomation
function:
- Read the database
********************************************************************************/
void db_Fread(Db* db);

/********************************************************************************
input:
- Db* db: database to link the customer
- Cus* cus: the customer to be added
function:
- Link one existing customer to the database
********************************************************************************/
void db_LinkCus(Db* dest, Cus* cus);

/********************************************************************************
input:
- Db* db: database to write
function:
- Write all the content in the database into files
********************************************************************************/
void db_Fwrite(Db* db);

/********************************************************************************
input:
- Cus* cus: the customer to write
- FILE* stream: the file to write customer information to
- Db* db: the database
function:
- Write all the content of the customer into the filestream
- Write the accounts linked to the customer to individual files
********************************************************************************/
void cus_Fwrite(Cus* cus, FILE* stream, Db* db);

/********************************************************************************
input:
- Cus* cus: the customer to read
-FILE* stream: the file to read customer information from
- Db* db: the database
function:
- Read all the content of the customer from the filestream
- Read the accounts linked to the customer form other files
********************************************************************************/
void cus_Fread(Cus* cus, FILE* stream, Db* db);

/********************************************************************************
input:
- char str[]: the string to check
function:
-  all the enters (\n) in the string
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
- Cus* cus: the customer to be add the account
- long accNum: the account number of the account
function:
- Add a account record the an existing customer
********************************************************************************/
void cus_AddAcc(Cus* cus, long accNum);

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
void acc_AddOPRec(Acc* dest, double amount, char info[], int type);

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
- Cus* cus: the customer to free
function:
- Free all the allocated memory linked to the customer
********************************************************************************/
void cus_Free(Cus* cus);

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
return:
- long: the input account number
function:
- Input and check the input account number
********************************************************************************/
long utl_GetAccNum();

/********************************************************************************
input:
- Db* db: the database
function:
- Activate the menu and options for manager
********************************************************************************/
void ma_Entrance(Db * db);

/********************************************************************************
input:
- int num: number of choices available
return:
- int: the input chooice
function:
- Input choice and check if it's acceptable
********************************************************************************/
int utl_getChoice(int num);

/********************************************************************************
input:
- char str[]: the string to check
return:
- int: 0 when incorrect, 1 when correct
function:
- Check if the string is acceptable as an ID number
remarks:
- This function checks the \n at the end. Use fgets for proper operation
********************************************************************************/
int utl_checkID(char str[]);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for managers to view information of customers
********************************************************************************/
void ma_ViewCusInfo(Db * db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for managers to view information of accounts
********************************************************************************/
void ma_ViewAccInfo(Db * db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for managers to view bank statistics
********************************************************************************/
void ma_ViewBanSta(Db * db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for managers to block one account
********************************************************************************/
void ma_BloAcc(Db * db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for managers to view all operations of the user
********************************************************************************/
void ma_ActAcc(Db * db);

/********************************************************************************
input:
- Db* db: the database
function:
- delete the account from the database
- including structure in database, account record in customer and the file
********************************************************************************/
void db_DelAcc(Db* dest, long accNum);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for clerk operations
********************************************************************************/
void clk_Entrance(Db* db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for clerk to add an account
********************************************************************************/
void clk_AddAcc(Db* db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for clerk to delete an account
********************************************************************************/
void clk_DelAcc(Db* db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for clerk to amend an account
********************************************************************************/
void clk_AmendAcc(Db* db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for clerk to deposit money to an account
********************************************************************************/
void clk_Deposit(Db* db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide the menu for clerk to add stand order record to an account
********************************************************************************/
void clk_SetSO(Db* db);

/********************************************************************************
input:
- char const* msg: the message to give
function:
- Ask the user to input a double and check the input
********************************************************************************/
double utl_InputDouble(char const* msg);

/********************************************************************************
input:
- char const* msg: the message to give
function:
- Ask the user to input a integer and check the input
********************************************************************************/
int utl_InputInt(char const* msg);

/********************************************************************************
input:
- char const* msg: the message to give
function:
- Ask the user to input a integer in range and check the input
********************************************************************************/
int utl_InputIntInRange(char const * msg, int l, int r);

/********************************************************************************
input:
- char const* msg: the message to give
function:
- Ask the user to input a string and check the input
********************************************************************************/
char* utl_InputLine(char const * msg, char * str);

/********************************************************************************
input:
- Cus* dest: the customer containing the account record
- loong accNum: the account number of the account to delete
function:
- Delete account record of one account linked to the customer
********************************************************************************/
void cus_DelAcc(Cus* dest, long accNum);

/********************************************************************************
input:
- char str[]: the strying to check
- int length: the required length of the number
output:
- int: 1 when meets require, 0 when not
function:
- Check if the given string is constructed with certain amount of numbers.
remarks:
- For instance, "0000001" is acceptable and "1" is not when the required length is 7.
********************************************************************************/
int utl_checkNum(char str[], int length);

/********************************************************************************
return:
- int: 1 to give up and 0 to continue
function:
- Provide warnings before exitting and ask to confirm.
********************************************************************************/
int main_warning();

/********************************************************************************
input:
- Db* db: the database
function:
- Provide menu for customers to choose operations
********************************************************************************/
void cu_Entrance(Db* db);

/********************************************************************************
input:
- Acc* acc: the account to check
function:
- Provide menu for customers to check the bakance of the account
********************************************************************************/
void cu_DisBal(Acc* acc);

/********************************************************************************
input:
- Acc* acc: the account to login
return:
- int: 0 when ended normally and -1 when the user choose to give up
function:
- Ask the user to input the PIN of the account
- Ask to enter again when PIN is not correct
********************************************************************************/
int utl_CheckPin(Acc* acc);

/********************************************************************************
input:
- Acc* acc: the account to display
function:
- Provide menu for custoemr to view all operations of the account
********************************************************************************/
void cu_DisAct(Acc* acc);

/********************************************************************************
input:
- Acc* acc: the account to withdraw from
function:
- Provide menu for custoemr to withdraw from one account
********************************************************************************/
void cu_Withdraw(Acc* acc);

/********************************************************************************
input:
- Acc* acc: the account to print
function:
- Provide menu for custoemr to view general information of the account
********************************************************************************/
void cu_Print(Acc* acc);

/********************************************************************************
function:
- Just a c version of system("pause")
********************************************************************************/
void utl_Pause();

/********************************************************************************
input:
- Db* db: the database
function:
- Provide menu for manager to login and check password
********************************************************************************/
void ma_Login(Db* db);

/********************************************************************************
input:
- Db* db: the database
function:
- Provide menu for clerk to login and check password
********************************************************************************/
void clk_Login(Db* db);

/********************************************************************************
input:
- char* accNumC: the string to check
return:
- int: 0 when incorrect, 1 when correct
function:
- Check if the string can be transformed into a acceptable account number.
********************************************************************************/
int utl_checkAccNum(char* accNumC);


/********************************************************************************
=================================================================================
=================================================================================
=================================Function code===================================
=================================================================================
=================================================================================
********************************************************************************/


main(){
	system("color F5"); 
	int I,s, choice; Db *db;	
	db=db_Load();
	printf("\n\n\n\n\n\n\n\n\n\n\n\t\t\t   Welcome to the XJTLU BANK!\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t");
	system("pause");
	for(I=1;;I++)
	{
		system("cls");
		printf("Dear user, welcome to the bank system!\n");
		puts("==================================================================");
		puts("Please choose one identity available to choose:");
		printf("\t1. Manager\n");
		printf("\t2. Bank Clerk\n");
		printf("\t3. Customer\n"); 
		printf("\t0. Shut down\n");
		puts("==================================================================");
		printf("Your choice is: ");
		choice = utl_getChoice(3);
		system("cls");
		if(choice == 1)
			ma_Login(db);
		if(choice == 2)
			clk_Login(db);
		if(choice == 3)
			cu_Entrance(db);	
		if(choice == 0)
		{
			s=main_warning();
			if(s==0)
				break;
		}
	}
	db_Free(db);
	system("cls");
	printf("\n\n\n\n\t\t\n\n\n\n\n\t\t\tThank you for your using~~~~\n\n\n\t\t\t\tBye-bye~~~~\n\n\n\n\n\n\n\n\n");
	utl_Pause();
}

Db* db_Load() {
	Db* db = malloc(sizeof(Db));
	db->cusList = NULL;
	/*declare an empty customer as the first element in the list*/
	Cus* cus = malloc(sizeof(Cus));
	cus->accList = NULL;
	cus->CusLast = NULL;
	cus->CusNext = NULL;
	strcpy(cus->address, "");
	strcpy(cus->ID, "");
	strcpy(cus->name, "");
	strcpy(cus->telephone, "");
	db->cusList = cus;
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
	Cus* temp = malloc(sizeof(Cus));
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
	fread(temp, sizeof(Cus), 1, file);
	free(temp);
	/*read other elements*/
	do {
		Cus* buffer = malloc(sizeof(Cus));
		cus_Fread(buffer, file, db);
		/*check if the program reaches the last empty element*/
		if (strcmp(buffer->ID, "") != 0) {
			db_LinkCus(db, buffer);
		}
		else {
			i = 0;
			free(buffer);
		}
	} while (i);
	fclose(file);
}

void db_LinkCus(Db* dest, Cus* cus) {
	Cus* buffer = dest->cusList;
	while (buffer->CusNext != NULL) {
		buffer = buffer->CusNext;
	}
	buffer->CusNext = cus;
	cus->CusLast = buffer;
}

void cus_LinkAccRec(Cus* dest, AccRec* accRec) {
	AccRec* buffer = dest->accList;
	while (buffer->AccRecNext != NULL) {
		buffer = buffer->AccRecNext;
	}
	buffer->AccRecNext = accRec;
	accRec->AccRecLast = buffer;
}

Cus* db_AddCus(Db* db, char address[], char ID[], char name[], char telephone[]) {
	Cus* buffer = malloc(sizeof(Cus));
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
	buffer->CusLast = NULL;
	buffer->CusNext = NULL;
	db_LinkCus(db, buffer);
	/*update file content*/
	db_Fwrite(db);
	return buffer;
}

void cus_Fread(Cus* cus, FILE* stream, Db* db) {
	Cus* temp = malloc(sizeof(Cus));
	/*set the first account to be an empty account*/
	AccRec* accRec = malloc(sizeof(AccRec));
	accRec->acc = 0;
	accRec->AccRecLast = NULL;
	accRec->AccRecNext = NULL;
	cus->accList = accRec;
	/*read the information and assign*/
	fread(temp, sizeof(Cus), 1, stream);
	cus->CusLast = NULL;
	cus->CusNext = NULL;
	strcpy(cus->ID, temp->ID);
	strcpy(cus->name, temp->name);
	strcpy(cus->address, temp->address);
	strcpy(cus->telephone, temp->telephone);
	/*check it is the last empty customer record in the file*/
	if (strcmp(temp->ID, "") == 0) {
		cus->accList = NULL;
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
				cus_LinkAccRec(cus, buffer);
				acc_Fread(db, buffer->acc);
			}
		}
	}
}

void cus_Print(Cus* cus, int mode, Db* db) {
	if (mode < 0) {
		return;
	}
	printf("\tName   : %s\n", cus->name);
	printf("\tID     : %s\n", cus->ID);
	printf("\tTel    : %s\n", cus->telephone);
	printf("\tAddress: %s\n", cus->address);
	/*mode > 0 means more information about accounts should be presented*/
	if (mode > 0) {
		AccRec* buffer = cus->accList;
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
	Cus* buffer = db->cusList;
	if (buffer->CusNext == NULL) {
		puts("No record");
	}
	else {
		while (buffer->CusNext != NULL) {
			buffer = buffer->CusNext;
			cus_Print(buffer, mode - 1, db);
			puts("");
		}
		puts("Print finished");
	}
}

void db_Free(Db* db) {
	Cus* bufferC = db->cusList;
	Acc* bufferA = db->accList;
	/*send information to ask the thread to terminate*/
	db->SO_State = 2;
	/*check if it has finised work*/
	while (db->SO_State != 3) {
		Sleep(10);
	}
	/*get to the last element*/
	while (bufferC->CusNext != NULL) {
		bufferC = bufferC->CusNext;
	}
	/*free all customers*/
	while (bufferC->CusLast != NULL) {
		bufferC = bufferC->CusLast;
		cus_Free(bufferC->CusNext);
	}
	/*free the last element*/
	free(bufferC);
	/*get to the last element*/
	while (bufferA->AccNext != NULL) {
		bufferA = bufferA->AccNext;
	}
	/*free all customers*/
	while (bufferA->AccLast != NULL) {
		bufferA = bufferA->AccLast;
		acc_Free(bufferA->AccNext);
	}
	/*free the last element*/
	free(bufferA);
}

void db_Fwrite(Db* db) {
	Cus* buffer = db->cusList;
	Cus empty = { NULL, "", NULL, NULL, "", "", "" };
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
	cus_Fwrite(buffer, file, db);
	while (buffer->CusNext != NULL) {
		buffer = buffer->CusNext;
		cus_Fwrite(buffer, file, db);
	}
	cus_Fwrite(&empty, file, db);
	/*print the final mark for other thread to check*/
	fprintf(file, "%d", -1);
	fclose(file);
}

void cus_Fwrite(Cus* cus, FILE* stream, Db* db) {
	/*check if it is an empty customer*/
	if (cus->accList == NULL) {
		fwrite(cus, sizeof(Cus), 1, stream);
	}
	else {
		AccRec* index = cus->accList;
		AccRec empty = { 0, NULL, NULL };
		Acc* acc;
		fwrite(cus, sizeof(Cus), 1, stream);
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

Cus* db_GetCus(Db* db, char ID[]) {
	Cus* index = db->cusList;
	utl_CheckString(ID);
	while (index->CusNext != NULL) {
		index = index->CusNext;
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

Acc* db_AddAcc(Db* db, int PIN, Cus* cus) {
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
	strcpy(buffer->ownerID, cus->ID);
	db_LinkAcc(db, buffer);
	cus_AddAcc(cus, buffer->accountNumber);
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
	OPRec emptyO = { -1, "", 0, 0, 0, 0 };
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

void cus_AddAcc(Cus* dest, long accNum) {
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
	printf("\tAcc Num : %ld\n", acc->accountNumber);
	printf("\tBalance : %.2f\n", acc->balance);
	printf("\tCustomer: %s\n", acc->ownerID);
	/*printf("\tPIN     : %06d\n", acc->PIN);*/
	printf("\tState   : %d\n", acc->state);
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
			puts("\nNo operation record");
		}
		/*print stand order records*/
		if (index2->SORecNext != NULL) {
			while (index2->SORecNext != NULL) {
				index2 = index2->SORecNext;
				soRec_Print(index2);
			}
		}
		else {
			puts("\nNo stand order record");
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
	fclose(file);
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

void acc_AddOPRec(Acc* dest, double amount, char info[], int type) {	
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

void acc_Deposit(Acc* acc, double amount) {
	acc->balance += amount;
	acc_AddOPRec(acc, amount, "", 1);
}

void acc_Withdraw(Acc* acc, double amount) {
	acc->balance -= amount;
	acc_AddOPRec(acc, amount, "", 0);
}

void soRec_Print(SORec* soRec) {
	char buffer[30];
	printf("\tinterval   : %ld\n", soRec->interval);
	printf("\tstart time : %s\n", utl_timeSprintf(buffer, soRec->timeAssigned));
	printf("\tend time   : %s\n", utl_timeSprintf(buffer, soRec->timeEnd));
	printf("\tlast oprate: %s\n", utl_timeSprintf(buffer, soRec->timeLastOperation));
}

void opRec_Print(OPRec* opRec) {
	char buffer[30];
	printf("\tRecord: ");
	/*check operation type and print*/
	switch (opRec->type)
	{
	case 0:
		puts("withdraw");
		printf("\tAmount: %.2f\n", opRec->amount);
		break;
	case 1:
		puts("deposit");
		printf("\tAmount: %.2f\n", opRec->amount);
		break;
	case 2:
		puts("stand order out");
		printf("\tTo    : %s\n", opRec->info);
		printf("\tAmount: %.2f\n", opRec->amount);
		break;
	case 3:
		puts("stand order in");
		printf("\tFrom  : %s", opRec->info);
		printf("\tAmount: %.2f\n", opRec->amount);
		break;
	case 4:
		puts("stand order failed: not enough balance");
		printf("\tTo    : %s\n", opRec->info);
		break;
	case 5:
		puts("stand order failed: destination account freezed");
		printf("\tTo    : %s\n", opRec->info);
		break;
	case 6:
		puts("Block");
		break;
	case 7:
		puts("Activate");
		break;
	case 8:
		puts("stand order failed: account freezed");
		break;
	case 9:
		puts("stand order failed: account not found");
		break;
	default:
		puts("unknown record type");
		break;
	}
	/*other information*/
	printf("\tTime  : %s\n", utl_timeSprintf(buffer, opRec->time));
}

char* utl_timeSprintf(char buffer[], time_t time) {
	sprintf(buffer, "%s", asctime(localtime(&time)));
	utl_CheckString(buffer);
	return buffer;
}

double cus_GetAvg(Cus* cus, Db* db) {
	int i = 0;
	double total = 0;
	double tempf;
	AccRec* index = cus->accList;
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

void cus_Free(Cus* cus) {
	AccRec* buffer = cus->accList;
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
	free(cus);
}

void acc_AddSORec(Acc* acc, long interval, long duration, double amount, long dest) {
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
			soRec->timeLastOperation = tm;
		}
		/*refresh last operation record*/
		
	}
}

void acc_OperateSO(Acc* acc, SORec* soRec, Db* db) {
	/*if balance enough*/
	if (acc->balance > soRec->amount) {
		Acc* dest = db_GetAcc(db, soRec->dest);
		if (dest == NULL) {
			char buffer[15];
			/*add operation records*/
			sprintf(buffer, "%ld", soRec->dest);
			acc_AddOPRec(acc, 0, buffer, 9);
		}
		else if (acc->state == 0) {
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
	_endthread();
}

void acc_Freeze(Acc* acc) {
	acc->state = 0;
	acc_AddOPRec(acc, 0, "", 6);
}

void acc_unFreeze(Acc* acc) {
	acc->state = 1;
	acc_AddOPRec(acc, 0, "", 7);
}

void ma_Entrance(Db * db){
	while (1) {
		system("cls");
		puts("Welcome, manager!");
		puts("==================================================================");
		puts("You have following operations available.");
		printf("\t1. View customer information\n\t2. View account information\n\t3. View banking statistics\n\t4. Block an account\n\t5. Activate an account\n");
		puts("\t0. Log out");
		puts("==================================================================");
		printf("Enter your choice: ");
		int op = utl_getChoice(5);     /*obtain input, determine option*/
		
		switch (op) {
		case 1: ma_ViewCusInfo(db);     /*view customer information*/
			break;
		case 2: ma_ViewAccInfo(db);     /*view account information*/
			break;
		case 3: ma_ViewBanSta(db);     /*view banking statistics*/
			break;
		case 4: ma_BloAcc(db);     /*block an account*/
			break;
		case 5: ma_ActAcc(db);     /*activate an account*/
			break;
		case 0: 
			return;
		default: printf("\t\tWrong direction entered.");     
			break;     /*continue asking for an option*/
		}
	}
}

int utl_getChoice(int num) {     /*support the use of switch() whose cases are numbers*/
	int n;
	char inputChoice[5];

	while (1) {
		rewind(stdin);
		fgets(inputChoice, 5, stdin);
		n = atoi(inputChoice);     /*convert inputChoice to an integer*/
		
		if (n >= 0 && n <= num && (inputChoice[1] == '\n'))     
			return n;     /*input number is in the range of normal choice*/
		
		printf("Invalid input, please input a number between 0 and %d.\nTry again: ", num);     /*not E && not B && not meaningful number, ask to enter again*/
	}
}

int utl_checkID(char str[]) {     /*check if input ID is in correct form*/
	int i;

	for (i = 0; i < 17; i++) {     
		if ((str[i] - '0') > 9 || (str[i] - '0') < 0)     /*check ID[0~16].*/
			return 0;     /* if one of them is not a digit*/
	}
	if (str[17] != 'X' && ((str[17] - '0') > 9 || (str[17] - '0') < 0))     /*check ID[17].*/
		return 0;     /*if it is neither capital X nor digit*/

	return 1;     /*if correct*/
}

void ma_ViewCusInfo(Db * db) {     /*view informaiton by searching customer's ID*/
	char ID[20];
	int mode = 0, op;
	Cus * cus;
	AccRec* buffer;
	system("cls");
	puts("You have logged in as manager.");
	puts("==================================================================");
	puts("The system is ready to search.");
	puts("Enter B to return to previous menu.");
	puts("==================================================================");
	printf("Enter ID of the customer to view: ");
	rewind(stdin);
	fgets(ID, 20, stdin);
	if ((ID[0] == 'B' || ID[0] == 'b') && ID[1] == '\n') {
		return;
	}
	while (utl_checkID(ID) != 1) {
		printf("Sorry, please enter a Chinese ID number as length of 18.\n");
		printf("Try again: ");
		rewind(stdin);
		fgets(ID, 20, stdin);
		if ((ID[0] == 'B' || ID[0] == 'b') && ID[1] == '\n') {
			return;
		}
	}
	cus = db_GetCus(db, ID);
	/*if customer not found*/
	if (cus == NULL) {
		puts("==================================================================");
		printf("Cannot find this ID.\n");
		puts("==================================================================");
		puts("Press enter to continue");
		rewind(stdin);
		getchar();
		return;     
	}
	/* customer found*/
	else {
		buffer = cus->accList;
		system("cls");
		puts("You have logged in as manager.");
		puts("==================================================================");
		puts("Information of the user shows as follows:");
		cus_Print(cus, 0, db);
		puts("==================================================================");
		puts("You have following choices available:");
		puts("\t1. View detailed account records");
		puts("\t0. Return to previous menu");
		puts("==================================================================");
		printf("Enter your choice: ");
		op = utl_getChoice(1);
		/*choose to view detail*/
		if (op == 1) {
			char accNumC[20];
			long accNum;
			Acc* acc;
			system("cls");
			puts("You have logged in as manager.");
			puts("==================================================================");
			puts("The customer has following accounts:");
			/*if no records found*/
			if (buffer->AccRecNext == 0) {
				puts("\t---------------------------------------");
				puts("\tNo records found");
			}
			/*records exist*/
			else{
				Acc* accbuffer;
				/*loop to print all record*/
				while (buffer->AccRecNext != 0) {
					puts("\t---------------------------------------");
					buffer = buffer->AccRecNext;
					printf("\tAccount: %ld\n", buffer->acc);
					accbuffer = db_GetAcc(db, buffer->acc);
					printf("\tBalance: %lf\n", accbuffer->balance);
				}
			}
			/*end account print*/
			puts("==================================================================");
			puts("Enter the account number to further check");
			puts("Enter B to return to previous menu");
			puts("==================================================================");
			printf("Enter: ");
			rewind(stdin);
			fgets(accNumC, 20, stdin);
			/*input account number*/
			while (utl_checkAccNum(accNumC) == 0) {
				printf("Invalid account number. It should be 10 numbers.\n");
				printf("Try again: ");
				rewind(stdin);
				fgets(accNumC, 20, stdin);
			}
			/*if choose to return*/
			if (utl_checkAccNum(accNumC) == -1) {
				return;
			}
			accNum = atol(accNumC);
			acc = db_GetAcc(db, accNum);
			/*if no such account*/
			if (acc == NULL) {
				puts("==================================================================");
				printf("Sorry, the user is not found.\n");
				puts("==================================================================");
				puts("Press enter to continue");
				rewind(stdin);
				getchar();
				return;
			}
			/*print detail information of the account*/
			system("cls");
			puts("You have logged in as manager.");
			puts("==================================================================");
			printf("Information of account %ld shows as follows:\n", accNum);
			acc_Print(acc, 0);
			puts("==================================================================");
			puts("You have following choices available:");
			puts("\t1. View detailed operation records");
			puts("\t2. Return to previous menu");
			puts("==================================================================");
			printf("Enter your choice: ");
			op = utl_getChoice(1);
			/*if choose to view detailed record*/
			if (op == 1) {
				OPRec* index1 = acc->OPRecList;
				SORec* index2 = acc->SORecList;
				/*print operation records*/
				system("cls");
				puts("You have logged in as manager.");
				puts("==================================================================");
				printf("Account %ld has following operation records:\n", accNum);
				if (index1->OPRecNext != NULL) {
					while (index1->OPRecNext != NULL) {
						index1 = index1->OPRecNext;
						puts("\t---------------------------------------");
						opRec_Print(index1);
					}
				}
				else {
					puts("No operation record");
				}
				puts("\t---------------------------------------");
				/*print stand order records*/
				puts("This account have following stand order settings:");
				if (index2->SORecNext != NULL) {
					while (index2->SORecNext != NULL) {
						index2 = index2->SORecNext;
						puts("\t---------------------------------------");
						soRec_Print(index2);
					}
				}
				else {
					puts("No stand order record");
				}
				puts("==================================================================");
				puts("Press enter to return to previous menu");
				getchar();
			}
			/*if choose to leave*/
			else if (op == 0) {
				return;
			}
			else {
				puts("Program error, please contact the maintaince team.");
			}
		}
	}
}

void ma_ViewAccInfo(Db * db) {     /*view information by searching an account number*/
	long accNum;
	int op, mode = 0;
	char accNumC[20];
	Acc * acc;
	system("cls");
	puts("You have logged in as manager.");
	puts("==================================================================");
	puts("The system is ready to search.");
	puts("Enter B to return to previous menu.");
	puts("==================================================================");
	printf("Enter account number of the account to view: ");
	rewind(stdin);
	fgets(accNumC, 20, stdin);
	/*enter account number*/
	while (utl_checkAccNum(accNumC) == 0) {
		printf("Invalid account number. It should be 10 numbers.\n");
		printf("Try again: ");
		rewind(stdin);
		fgets(accNumC, 20, stdin);
	}
	/*if choose to leave*/
	if (utl_checkAccNum(accNumC) == -1) {
		return;
	}
	accNum = atol(accNumC);
	acc = db_GetAcc(db, accNum);
	/*if account not found*/
	if (acc == NULL) {
		puts("==================================================================");
		printf("Sorry, the user is not found.\n");
		puts("==================================================================");
		puts("Press enter to continue");
		rewind(stdin);
		getchar();
		return;
	}
	/*print account information*/
	system("cls");
	puts("You have logged in as manager.");
	puts("==================================================================");
	printf("Information of account %ld shows as follows:\n", accNum);
	acc_Print(acc, 0);
	puts("==================================================================");
	puts("You have following choices available:");
	puts("\t1. View detailed operation records");
	puts("\t0. Return to previous menu");
	puts("==================================================================");
	printf("Enter your choice: ");
	op = utl_getChoice(1);
	/*if choose to print detail records*/
	if (op == 1) {
		OPRec* index1 = acc->OPRecList;
		SORec* index2 = acc->SORecList;
		/*print operation records*/
		system("cls");
		puts("You have logged in as manager.");
		puts("==================================================================");
		printf("Account %dl have following operation records:\n", accNum);
		if (index1->OPRecNext != NULL) {
			while (index1->OPRecNext != NULL) {
				index1 = index1->OPRecNext;
				puts("\t---------------------------------------");
				opRec_Print(index1);
			}
		}
		else {
			puts("No operation record");
		}
		puts("\t---------------------------------------");
		/*print stand order records*/
		puts("This account have following stand order settings:");
		if (index2->SORecNext != NULL) {
			while (index2->SORecNext != NULL) {
				index2 = index2->SORecNext;
				puts("\t---------------------------------------");
				soRec_Print(index2);
			}
		}
		else {
			puts("No stand order record");
		}
		puts("==================================================================");
		puts("Press enter to return to previous menu");
		getchar();
	}
	/*if choose to leave*/
	else if(op == 0){
		return;
	}
	else {
		puts("Program error, please contact the maintaince team.");
	}
}

void ma_ViewBanSta(Db * db) {
	int c, a;
	Cus * cus = db->cusList, * bufferC = db->cusList;
	Acc * acc = db->accList, * bufferA = db->accList;
	double tot = 0;
	system("cls");
	puts("You have logged in as manager.");
	puts("==================================================================");
	puts("Statistics of the bank shows as follows:");
	if (cus->CusNext == NULL) {     /*no real customer in the cusList*/
		printf("\tNo customer in the database.\n");
		puts("==================================================================");
		printf("Press enter to continue");
		rewind(stdin);
		getchar();
		return;
	}
	for (c = 0; cus->CusNext != NULL; c++) {
		cus = cus->CusNext;
	}/*count the number of customers to the end of the cusList*/
	printf("\tNumber of customers         : %d\n", c);

	if (acc->AccNext == NULL) {
		printf("\tNo account in the database.\n");
		puts("==================================================================");
		printf("Press enter to continue");
		rewind(stdin);
		getchar();
		return;
	}
	for (a = 0; acc->AccNext != NULL; a++) {
		acc = acc->AccNext;
	}
	printf("\tNumber of accounts          : %d\n", a);
	printf("\tAverage account pre customer: %.2f\n", (float)a/(float)c);
	while (bufferA->AccNext != NULL) {
		bufferA = bufferA->AccNext;
		tot += bufferA->balance;     /*add balances of all accounts together*/
	}
	printf("\tAvg acc balance             : %.2f\n", tot / (double)a);     /*calculate and print average account balance of the bank*/
	puts("==================================================================");
	printf("Press enter to continue");
	rewind(stdin);
	getchar();
}

void ma_BloAcc(Db * db) {
	long accNum;
	Acc * acc = db->accList;
	char accNumC[20];
	system("cls");
	puts("You have logged in as manager");
	puts("==================================================================");
	if (acc->AccNext == NULL) {
		printf("\t\tNo account in the database.\n");
		puts("==================================================================");
		puts("Press enter to continue");
		rewind(stdin);
		getchar();
		return;
	}
	puts("The system ie ready to operate.");
	puts("Enter the account number to block.");
	puts("Enter B to return.");
	puts("==================================================================");
	printf("Enter the account number: ");
	/*enter account number*/
	rewind(stdin);
	fgets(accNumC, 20, stdin);
	while (utl_checkAccNum(accNumC) == 0) {
		printf("Invalid the account number should be 10 numbers.\nTry again: ");
		rewind(stdin);
		fgets(accNumC, 20, stdin);
	}
	/*if choose to leave*/
	if (utl_checkAccNum(accNumC) == -1) {
		return;
	}
	accNum = atol(accNumC);
	acc = db_GetAcc(db, accNum);
	/*if account not found*/
	if (acc == NULL) {
		puts("==================================================================");
		printf("Cannot find this account number.\n");
		puts("==================================================================");
		puts("Press enter to continue.");
		rewind(stdin);
		getchar();
		return;
	}
	acc_Freeze(acc);     /*set state of the account to 0 and add operation record*/
	puts("==================================================================");
	printf("Account blocked.\n");
	puts("==================================================================");
	puts("Press enter to continue.");
	rewind(stdin);
	getchar();
}

void ma_ActAcc(Db * db) {
	long accNum;
	Acc * acc = db->accList;
	char accNumC[20];
	system("cls");
	puts("You have logged in as manager");
	puts("==================================================================");
	if (acc->AccNext == NULL) {
		printf("No account in the database.\n");
		puts("==================================================================");
		puts("Press enter to continue");
		rewind(stdin);
		getchar();
		return;
	}
	puts("The system ie ready to operate.");
	puts("Enter the account number to activate.");
	puts("Enter B to return.");
	puts("==================================================================");
	printf("Enter the account number: ");
	/*enter account number*/
	rewind(stdin);
	fgets(accNumC, 20, stdin);
	while (utl_checkAccNum(accNumC) == 0) {
		printf("Invalid the account number should be 10 numbers.\nTry again: ");
		rewind(stdin);
		fgets(accNumC, 20, stdin);
	}
	/*if choose to leave*/
	if (utl_checkAccNum(accNumC) == -1) {
		return;
	}
	accNum = atol(accNumC);
	acc = db_GetAcc(db, accNum);
	/*if account not found*/
	if (acc == NULL) {
		puts("==================================================================");
		printf("Cannot find this account number.\n");
		puts("==================================================================");
		puts("Press enter to continue.");
		rewind(stdin);
		getchar();
		return;
	}
	acc_unFreeze(acc);     /*set state of the account to 0 and add operation record*/
	puts("==================================================================");
	printf("Account activated.\n");
	puts("==================================================================");
	puts("Press enter to continue.");
	rewind(stdin);
	getchar();
}

void clk_Entrance(Db* db) {
	int choise; 
	do
	{
		system("cls");
		puts("You have logged in as clerk");
		puts("==================================================================");
		puts("You have following opertions available:");
		puts("\t1.Add account");
		puts("\t2.Delete account");
		puts("\t3.Amend account");
		puts("\t4.Make deposits");
		puts("\t5.Set up a standing order");
		puts("\t0.Back");
		puts("==================================================================");
		choise = utl_InputIntInRange("Please enter your choice:", 0, 5);
		printf("\n");
		switch (choise)
		{
		case 1:
			clk_AddAcc(db);
			break;
		case 2:
			clk_DelAcc(db);
			break;
		case 3:
			clk_AmendAcc(db);
			break;
		case 4:
			clk_Deposit(db);
			break;
		case 5:
			clk_SetSO(db);
			break;
		case 0:
			break;
		default:
			puts("Unknown command");
			system("pause");
		}
	} while (choise != 0);

}
void clk_AddAcc(Db* db)
{
	char id[20],name[30],address[100],phone[20];
	char pinC[10];
	Cus* tempCos;
	int pin1, pin2, choose;
	Acc *acc;
	system("cls");
	puts("You have logged in as clerk");
	puts("==================================================================");
	puts("You have following choices to creat an account:");
	puts("\t1.Creat a new customer");
	puts("\t2.Add the account to an exisisting customer");
	puts("\t0.Give up and return to previous menu");
	puts("==================================================================");
	printf("Please enter your choice: ");
	choose = utl_getChoice(2);
	/*choose to quit*/
	if (choose == 0) {
		return;
	}
	/*choose to creat new account*/
	if(choose==1)
	{
		system("cls");
		puts("You have logged in as clerk");
		puts("==================================================================");
		puts("Please enter following information of the customer");
		puts("==================================================================");
		/*get customer information*/
		printf("Name: ");
		rewind(stdin);
		fgets(name, 30, stdin);
		printf("Address: ");
		rewind(stdin);
		fgets(address, 100, stdin);
		printf("Telephone number: ");
		rewind(stdin);
		fgets(phone, 20, stdin);
		printf("ID number: ");
		rewind(stdin);
		fgets(id, 20, stdin);
		while (utl_checkID(id) == 0) {
			printf("Please enter a Chinese ID number in length of 18.\nTry again: ");
			rewind(stdin);
			fgets(id, 20, stdin);
		}
		/*check if the ID exist*/
		tempCos = db_GetCus(db, id);
		if (tempCos != NULL) {
			puts("==================================================================");
			puts("Sorry, this ID has been used.");
			puts("Try add the account to the ID.");
			puts("==================================================================");
			utl_Pause();
			return;
		}
		tempCos = db_AddCus(db, address, id, name, phone);
	}
	/*choose to add to exisisting customer*/
	else if(choose == 2)
	{
		system("cls");
		puts("You have logged in as clerk");
		puts("==================================================================");
		puts("Input the ID of the customer to be linked to.");
		puts("==================================================================");
		printf("Please enter the ID: ");
		/*get ID*/
		rewind(stdin);
		fgets(id, 20, stdin);
		while (utl_checkID(id) == 0) {
			printf("Please enter a Chinese ID number in length of 18.\nTry again: ");
			rewind(stdin);
			fgets(id, 20, stdin);
		}
		tempCos = db_GetCus(db, id);
		/*if customer not found*/
		if (tempCos == NULL)
		{
			puts("==================================================================");
			printf("There is no related records with the id!\n");
			puts("==================================================================");
			puts("Press enter to continue.");
			rewind(stdin);
			getchar();
			return;
		}
	}
	/*set pin for the account*/
	system("cls");
	puts("You have logged in as clerk");
	puts("==================================================================");
	puts("Set PIN of the new account");
	puts("==================================================================");
	
	do{
		printf("Enter the PIN number to use: ");
		rewind(stdin);
		fgets(pinC, 20, stdin);
		while (utl_checkNum(pinC, 6) != 1) {
			printf("Invalid input, please enter 6 numbers.\nTry again: ");
			rewind(stdin);
			fgets(pinC, 20, stdin);
		}
		pin1 = atoi(pinC);
		printf("Enter PIN again to confirm: ");
		rewind(stdin);
		fgets(pinC, 20, stdin);
		while (utl_checkNum(pinC, 6) != 1) {
			printf("Invalid input, please enter 6 numbers.\nTry again: ");
			rewind(stdin);
			fgets(pinC, 20, stdin);
		}
		pin2 = atoi(pinC);
		/*check if two input is identical*/
		if (pin1 != pin2) {
			puts("Sorry, your two inputs don't match. Try again.");
		}
	} while (pin1 != pin2);
	/*add the account*/
	acc = db_AddAcc(db, pin1, tempCos);
	puts("==================================================================");
	printf("The account number is %ld, please keep it well.\n",acc->accountNumber);
	puts("==================================================================");
	puts("Press enter to continue.");
	rewind(stdin);
	getchar();
}

void clk_DelAcc(Db* db)
{
	long id;
	Acc* tempAcc;
	system("cls");
	puts("You have logged in as clerk");
	puts("==================================================================");
	puts("You are going to delete one account. Be careul.");
	puts("==================================================================");
	printf("Please input the account number: ");
	/*get account number*/
	id = utl_GetAccNum();
	tempAcc = db_GetAcc(db, id);
	/*if account not found*/
	if (tempAcc == NULL)
	{
		puts("==================================================================");
		printf("There is no related records with the id.\n");
		puts("==================================================================");
		puts("Press enter to continue.");
		rewind(stdin);
		getchar();
		return;
	}
	/*do operations*/
	db_DelAcc(db, tempAcc->accountNumber);
	puts("==================================================================");
	printf("Operate successfully.\n");
	puts("==================================================================");
	puts("Press enter to continue.");
	rewind(stdin);
	getchar();
}

void clk_AmendAcc(Db* db)
{
	char accNumC[20], pinC[10];
	long accNum;
	Acc* acc;
	int pin1, pin2;
	system("cls");
	puts("You have logged in as clerk");
	puts("==================================================================");
	puts("System is ready for modification.");
	puts("==================================================================");
	printf("Enter the account number of the account to amend: ");
	/*input account number*/
	rewind(stdin);
	fgets(accNumC, 20, stdin);
	while (utl_checkAccNum(accNumC) == 0) {
		printf("Invalid the account number should be 10 numbers.\nTry again: ");
		rewind(stdin);
		fgets(accNumC, 20, stdin);
	}
	/*if choose to leave*/
	if (utl_checkAccNum(accNumC) == -1) {
		return;
	}
	accNum = atol(accNumC);
	acc = db_GetAcc(db, accNum);
	/*if account not found*/
	if (acc == NULL) {
		puts("==================================================================");
		printf("Cannot find this account number.\n");
		puts("==================================================================");
		puts("Press enter to continue.");
		rewind(stdin);
		getchar();
		return;
	}
	/*set new Pin*/
	do {
		printf("Enter the PIN number to use: ");
		rewind(stdin);
		fgets(pinC, 20, stdin);
		/*input first pin*/
		while (utl_checkNum(pinC, 6) != 1) {
			printf("Invalid input, please enter 6 numbers.\nTry again: ");
			rewind(stdin);
			fgets(pinC, 20, stdin);
		}
		pin1 = atoi(pinC);
		printf("Enter PIN again to confirm: ");
		/*input seconde pin*/
		rewind(stdin);
		fgets(pinC, 20, stdin);
		while (utl_checkNum(pinC, 6) != 1) {
			printf("Invalid input, please enter 6 numbers.\nTry again: ");
			rewind(stdin);
			fgets(pinC, 20, stdin);
		}
		/*check if they are identical*/
		pin2 = atoi(pinC);
		if (pin1 != pin2) {
			puts("Sorry, your two inputs don't match. Try again.\n");
		}
	} while (pin1 != pin2);
	/*end up*/
	puts("==================================================================");
	printf("Operate successfully.\n");
	puts("==================================================================");
	puts("Press enter to continue.");
	rewind(stdin);
	getchar();
	acc->PIN = pin1;
	db_Fwrite(db);

}
void clk_Deposit(Db* db)
{
	long accNum;
	Acc* tempAcc;
	double amount;
	char accNumC[20];
	system("cls");
	puts("You have logged in as clerk");
	puts("==================================================================");
	printf("You are going to deposit money to accounts.\n");
	puts("==================================================================");
	printf("Enter the account number of the account to deposit: ");
	/*input account number*/
	rewind(stdin);
	fgets(accNumC, 20, stdin);
	while (utl_checkAccNum(accNumC) == 0) {
		printf("Invalid the account number should be 10 numbers.\nTry again: ");
		rewind(stdin);
		fgets(accNumC, 20, stdin);
	}
	/*if choose to leave*/
	if (utl_checkAccNum(accNumC) == -1) {
		return;
	}
	accNum = atol(accNumC);
	tempAcc = db_GetAcc(db, accNum);
	/*if account not found*/
	if (tempAcc == NULL) {
		puts("==================================================================");
		printf("Cannot find this account number.\n");
		puts("==================================================================");
		puts("Press enter to continue.");
		rewind(stdin);
		getchar();
		return;
	}
	amount = utl_InputDouble("Please input the amount:");
	/*do operation*/
	acc_Deposit(tempAcc, amount);
	puts("==================================================================");
	printf("Operate successfully.\n");
	puts("==================================================================");
	puts("Press enter to continue.");
	rewind(stdin);
	getchar();
}

void clk_SetSO(Db* db)
{
	long accNum,dest;int i,time;
	Acc *tempAcc,*destAcc;
	long interval,duration;
	double amount;
	char accNumC[20];
	
	system("cls");
	puts("You have logged in as clerk");
	puts("==================================================================");
	puts("Stand order settings is ready to be added.");
	puts("==================================================================");
	printf("Enter the peyer account:");
	/*enter payer account*/
	rewind(stdin);
	fgets(accNumC, 20, stdin);
	while (utl_checkAccNum(accNumC) == 0) {
		printf("Invalid account number. It should be 10 numbers.\n");
		printf("Try again: ");
		rewind(stdin);
		fgets(accNumC, 20, stdin);
	}
	/*if choose to leave*/
	if (utl_checkAccNum(accNumC) == -1) {
		return;
	}
	accNum = atol(accNumC);
	tempAcc = db_GetAcc(db, accNum);
	/*if account not found*/
	if (tempAcc == NULL)
	{
		puts("==================================================================");
		printf("There is no related records with the account number!\n");
		puts("==================================================================");
		puts("Press enter to continue.");
		rewind(stdin);
		getchar();
		return;
	}
	for(i=0;;i++)
	{
		/*enter payee account*/
		printf("Enter the peyee account:");
		rewind(stdin);
		fgets(accNumC, 20, stdin);
		while (utl_checkAccNum(accNumC) == 0) {
			printf("Invalid account number. It should be 10 numbers.\n");
			printf("Try again: ");
			rewind(stdin);
			fgets(accNumC, 20, stdin);
		}
		/*if choose to leave*/
		if (utl_checkAccNum(accNumC) == -1) {
			return;
		}
		dest = atol(accNumC);
		if(dest==accNum)
			printf("Please give another account\n");
		else
			break;
	}
	destAcc = db_GetAcc(db, dest);
	/*if account not found*/
	if (destAcc == NULL)
	{
		puts("==================================================================");
		printf("There is no related records with the account number!\n");
		puts("==================================================================");
		puts("Press enter to continue.");
		rewind(stdin);
		getchar();
		return;
	}
	/*input detail information*/
	interval = utl_InputInt("Please input the interval in seconds:");/*needs more work*/
	amount = utl_InputDouble("Please input the amount:");
	time = utl_InputInt("Please input how many times you want to do:");
	duration = (time+1)*interval;
	/*do operation*/
	acc_AddSORec(tempAcc, interval, duration, amount, dest);
	puts("==================================================================");
	printf("Operation succeeded.\n");
	puts("==================================================================");
	puts("Press enter to continue.");
	rewind(stdin);
	getchar();

}

double utl_InputDouble(char const* msg) {
	double result;
	printf("%s", msg);
	while (scanf("%lf", &result) != 1) {
		rewind(stdin);
		printf("%s", msg);
	}
	rewind(stdin);
	return result;
}

int utl_InputInt(char const* msg) {
	int result;
	printf("%s", msg);
	while (scanf("%d", &result) != 1) {
		rewind(stdin);
		printf("%s", msg);
	}
	rewind(stdin);
	return result;
}

int utl_InputIntInRange(char const * msg, int l, int r) {
	int j;
	while (1) {
		j = utl_InputInt(msg);
		/*check range*/
		if (j<l || j>r) {
			printf("Not an integer between %d and %d, try again.\n", l, r);
			continue;
		}
		else
			break;
	}
	return j;
}

char* utl_InputLine(char const * msg, char * str) {
	int i = sizeof(str) / sizeof(char);
	printf("%s", msg);
	fgets(str, i, stdin);
	utl_CheckString(str);
	return str;
}

void cus_DelAcc(Cus* dest, long accNum)
{
	AccRec* buffer = dest->accList;
	while (buffer->AccRecNext != NULL) {
		/*check by term*/
		buffer = buffer->AccRecNext;
		/*if get needed account*/
		if (buffer->acc == accNum) {
			/*if it is the last term*/
			if (buffer->AccRecNext == NULL) {
				buffer->AccRecLast->AccRecNext = NULL;
				free(buffer);
				return;
			}
			/*if it's the term in the middle*/
			else {
				buffer->AccRecLast->AccRecNext = buffer->AccRecNext;
				buffer->AccRecNext->AccRecLast = buffer->AccRecLast;
				free(buffer);
				return;
			}
		}
	}
}

void db_DelAcc(Db* dest, long accNum)
{
	Acc* buffer = db_GetAcc(dest, accNum);
	Cus* bufferC = db_GetCus(dest, buffer->ownerID);
	char filename[25] = ".\\data\\";
	char accNumC[20];
	sprintf(accNumC, "%ld.db", buffer->accountNumber);
	strcat(filename, accNumC);
	if (buffer == NULL) {
		return;
	}
	/*if it is the last term*/
	if (buffer->AccNext == NULL) {
		buffer->AccLast->AccNext = NULL;
		free(buffer);
	}
	/*if it is a middle term*/
	else {
		buffer->AccLast->AccNext = buffer->AccNext;
		buffer->AccNext->AccLast = buffer->AccLast;
		free(buffer);
	}
	/*delete record in customer side*/
	cus_DelAcc(bufferC, accNum);
	/*delete file*/
	remove(filename);
	/*refresh file*/
	db_Fwrite(dest);
}

void cu_Entrance(Db* db) {
	long accNum;
	Acc* acc;
	int choice;
	puts("Logging in");
	puts("==================================================================");
	puts("Please enter the account number.");
	puts("Enter B to return to previous menu.");
	puts("==================================================================");
	printf("Enter the account number: ");
	accNum = utl_GetAccNum();
	/*if choose to leave*/
	if (accNum == -1) {
		return;
	}
	else {
		acc = db_GetAcc(db, accNum);
	}
	/*if account not found*/
	if (acc == NULL) {
		puts("==================================================================");
		puts("Sorry, this account is not found.");
		puts("==================================================================");
		utl_Pause();
		return;
	}
	/*if account blocked*/
	if (acc->state == 0) {
		puts("==================================================================");
		puts("Sorry, this account has been blocked.");
		puts("Contact clerks for solution.");
		puts("==================================================================");
		utl_Pause();
		return;
	}
	/*print user nemu*/
	do {
		system("cls");
		printf("Hello, %ld\n", acc->accountNumber);
		puts("==================================================================");
		printf("You have these services:\n");
		printf("\t1. Display the balance\n\t2. Display all the banking activities\n\t3. Withdraw the money\n\t4. get the account information\n\t0. log out\n");
		puts("==================================================================");
		printf("Please enter your choice: ");
		choice = utl_getChoice(4);
		switch (choice)
		{
		case 1:
			cu_DisBal(acc);
			break;
		case 2:
			cu_DisAct(acc);
			break;
		case 3:
			cu_Withdraw(acc);
			break;
		case 4:
			cu_Print(acc);
		default:
			break;
		}
	} while (choice != 0);
}

int main_warning()
{
	int i;
	char choose[10];
	for(i=1;;i++)
	{
		puts("==================================================================");
		printf("Are you sure to shut down the program?\nIf so, all the stand order check will be paused.\n");
		printf("They will start again when the program start to operate.\n");
		printf("You have following choices.\n");
		puts("\t1. continue\n\t0. give up");
		puts("==================================================================");
		printf("Enter your choice: ");
		rewind(stdin);
		scanf("%s",choose);			
		if((choose[0]=='0'||choose[0]=='1')&&(choose[1]=='\0'))
		break;
		else
		{
			printf("Please give a choice availble.");
			Sleep(1000);
			system("cls");
		}
	}
	if(choose[0]=='1')
		return 0;
	if(choose[0]=='0')
		return 1;
	return 1;
}

int utl_checkAccNum(char* accNumC) {
	int i;
	/*check if want to leave*/
	if ((accNumC[0] == 'B' || accNumC[0] == 'b')&&accNumC[1] == '\n') {
		return -1;
	}
	/*check numbers*/
	for (i = 0; i < 10; i++) {
		if (!(accNumC[i] >= '0' && accNumC[i] <= '9')) {
			return 0;
		}
	}
	/*check string end*/
	if (accNumC[10] != '\n') {
		return 0;
	}
	return 1;
}

int utl_checkNum(char str[], int length) {
	int i;
	/*check numbers*/
	for (i = 0; i < length; i++) {
		if ((str[i] - '0') > 10 || (str[i] - '0') < 0) {
			return 0;
		}
	}
	/*check string end*/
	if (str[i] != '\n') {
		return 0;
	}
	return 1;
}

long utl_GetAccNum() {
	char accNumC[20];
	rewind(stdin);
	fgets(accNumC, 20, stdin);
	/*if decide to leave*/
	if (utl_checkAccNum(accNumC) == -1) {
		return -1;
	}
	/*if illeagle input detected*/
	while (utl_checkAccNum(accNumC) == 0) {
		printf("Invalid account number. It should be 10 numbers.\n");
		printf("Try again: ");
		rewind(stdin);
		fgets(accNumC, 20, stdin);
		if (utl_checkAccNum(accNumC) == -1) {
			return -1;
		}
	}
	return atol(accNumC);
}

int utl_CheckPin(Acc* acc) {
	char buffer[10];
	int pin;
	puts("==================================================================");
	printf("Please enter password to continue: ");
	/*check if the input is corresponding to account record*/
	do {
		/*check if the input is in correct form*/
		do {
			rewind(stdin);
			fgets(buffer, 10, stdin);
			/*if decide toleave*/
			if ((buffer[0] == 'B' || buffer[0] == 'b') && buffer[1] == '\n') {
				return -1;
			}
			/*if format incorrect*/
			if ((utl_checkNum(buffer, 6) != 1)) {
				printf("Please enter the PIN as 6 numbers.\nTry again: ");
			}
		} while (utl_checkNum(buffer, 6) != 1);
		pin = atol(buffer);
		if(pin != acc->PIN){
			printf("Wrong PIN. Enter B to give up.\nTry again: ");
		}
	} while (pin != acc->PIN);
	return 0;
}

void utl_Pause() {
	puts("Press enter to continue");
	rewind(stdin);
	getchar();
}

void cu_DisBal(Acc* acc) {
	system("cls");
	printf("You have logged in as %ld\n", acc->accountNumber);
	if (utl_CheckPin(acc) == -1) {
		return;
	}
	else {
		printf("Your balance:%.2f\n", acc->balance);
		puts("==================================================================");
		utl_Pause();
	}
}

void cu_DisAct(Acc* acc) {
	system("cls");
	OPRec* index1 = acc->OPRecList;
	printf("You have logged in as %ld\n", acc->accountNumber);
	if (utl_CheckPin(acc) == -1) {
		return;
	}
	else {
		puts("==================================================================");
		puts("This account has following operation records:");
		if (index1->OPRecNext != NULL) {
			/*print all records by term*/
			while (index1->OPRecNext != NULL) {
				index1 = index1->OPRecNext;
				puts("\t---------------------------------------");
				opRec_Print(index1);
			}
		}
		else {
			puts("\nNo operation record");
		}
		puts("==================================================================");
		utl_Pause();
	}
}

void cu_Withdraw(Acc* acc) {
	double amount;
	system("cls");
	OPRec* index1 = acc->OPRecList;
	printf("You have logged in as %ld\n", acc->accountNumber);
	/*if choose to leave*/
	if (utl_CheckPin(acc) == -1) {
		return;
	}
	else {
		/*input amouunt*/
		printf("Amount to withdraw: ");
		rewind(stdin);
		scanf("%lf", &amount);
		while (getchar() != '\n') {
			printf("Please enter a positive number.\nTry again:");
			rewind(stdin);
			scanf("%lf", &amount);
		}
		/*if no enough balance*/
		if (acc->balance < amount) {
			int choice;
			puts("==================================================================");
			puts("Balance not enough, you have following choices:");
			puts("\t1. Withdraw all the money");
			puts("\t0. Give up");
			puts("==================================================================");
			printf("Enter your choice: ");
			choice = utl_getChoice(1);
			switch (choice)
			{
			case 1:
				acc_Withdraw(acc, acc->balance);
				break;
			case 0:
				return;
			default:
				printf("Program error.");
				break;
			}
		}
		/*if balance enough*/
		else {
			acc_Withdraw(acc, amount);
		}
		puts("==================================================================");
		puts("Operation succeeded.");
		puts("==================================================================");
		utl_Pause();
	}
}

void cu_Print(Acc* acc) {
	system("cls");
	printf("You have logged in as %ld\n", acc->accountNumber);
	if (utl_CheckPin(acc) == -1) {
		return;
	}
	puts("==================================================================");
	puts("Information of the account shows as follows:");
	acc_Print(acc, 0);
	puts("==================================================================");
	utl_Pause();
}

void ma_Login(Db* db) {
	char buffer[10];
	int pin;
	puts("Logging in");
	puts("==================================================================");
	printf("Please enter password to continue: ");
	do {
		/*check if in correct format*/
		do {
			rewind(stdin);
			fgets(buffer, 10, stdin);
			/*if choose to leave*/
			if ((buffer[0] == 'B' || buffer[0] == 'b') && buffer[1] == '\n') {
				return;
			}
			/*if format incorrect*/
			if ((utl_checkNum(buffer, 6) != 1)) {
				printf("Please enter the PIN as 6 numbers.\nTry again: ");
			}
		} while (utl_checkNum(buffer, 6) != 1);
		pin = atol(buffer);
		/*if PIN in correct*/
		if (pin != MA_PW) {
			printf("Wrong PIN. Enter B to give up.\nTry again: ");
		}
	} while (pin != MA_PW);
	ma_Entrance(db);
}

void clk_Login(Db* db) {
	char buffer[10];
	int pin;
	puts("Logging in");
	puts("==================================================================");
	printf("Please enter password to continue: ");
	do {
		/*check if in correct format*/
		do {
			rewind(stdin);
			fgets(buffer, 10, stdin);
			/*if choose to leave*/
			if ((buffer[0] == 'B' || buffer[0] == 'b') && buffer[1] == '\n') {
				return;
			}
			/*if format incorrect*/
			if ((utl_checkNum(buffer, 6) != 1)) {
				printf("Please enter the PIN as 6 numbers.\nTry again: ");
			}
		} while (utl_checkNum(buffer, 6) != 1);
		pin = atol(buffer);
		/*if PIN in correct*/
		if (pin != CLK_PW) {
			printf("Wrong PIN. Enter B to give up.\nTry again: ");
		}
	} while (pin != CLK_PW);
	clk_Entrance(db);
}