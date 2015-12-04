#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*stucture define*/
typedef struct StudentStrt {
	char nameFamily[20];
	char nameGiven[20];
	int numberID;
	char email[40];
	int grade1;
	int grade2;
	int grade3;
	int grade4;
	int grade5;
	int grade6;
	float gradeAvg;
	struct Student* stdFormer;
	struct Student* stdLatter;
}Student;

typedef struct DatabaseStrct {
	Student listStudent;
	FILE* fileStudent;
}Database;

/*fcunction define*/
int getChoice(int length);
Database databaseNew();
Database databaseOpen();
Database databaseLoad(char filename[]);
Database databaseErrorExist(char filename[]);
Database databaseErrorNotfound(char filename[]);
void databaseSearchElement(Database* db);
int databaseOperates(Database* db);
void quit();
void elementInsert(Student *elementDest, Student *elementToInsert);
int listGetPos(Student* list, int numberID, Student** buffer);
void databaseAddElement(Database* db);
void databaseSave(Database* db);
int checkNum(char str[], int length);
void databasePrint(Database* db);
void elementPrint(Student* stu);
void elementSave(Student* stu, FILE* file);
int markInput();



int main() {
	int inputChoice, i = 0;
	Database db;
	puts("Welcome to the student database!");
	puts("Following choices are available:");
	puts("1. Creat a new database;");
	puts("2. Load an exist database;");
	puts("0. Quit");
	printf("Please choose one action by entering the number:");
	inputChoice = getChoice(2);
	switch (inputChoice) {
	case 1:
		db = databaseNew();
		break;
	case 2:
		db = databaseOpen();
		break;
	case 0:
		exit(0);
		break;
	default:
		break;
	}
	while (i == 0) {
		i = databaseOperates(&db);
	}
	databaseSave(&db);
	fclose(db.fileStudent);
	system("pause");
}

int getChoice(int length) {
	int inputChoice, inputValid = 1;
	rewind(stdin);
	scanf("%d", &inputChoice);
	if (inputChoice<0 || inputChoice>length) {
		inputValid = 0;
	}
	else if (getchar() != '\n') {
		inputValid = 0;
	}
	while (inputValid == 0) {
		inputValid = 1;
		printf("Invalid input, please try again:");
		rewind(stdin);
		scanf("%d", &inputChoice);
		if (inputChoice<0 || inputChoice>length) {
			inputValid = 0;
		}
		else if (getchar() != '\n') {
			inputValid = 0;
		}
	}
	return inputChoice;
}

Database databaseNew() {
	Database db;
	FILE* file;
	Student list = { "","",0,"",0,0,0,0,0,0,0,NULL,NULL };
	char fileName[34];
	/*input filename*/
	printf("Please input filename of the database (witout suffix):");
	rewind(stdin);
	fgets(fileName, 30, stdin);
	fileName[strlen(fileName) - 1] = 0;
	strcat(fileName, ".txt");
	/*open file*/
	file = fopen(fileName, "rb");
	if (file != NULL) {
		fclose(file);
		return databaseErrorExist(fileName);
	}
	file = fopen(fileName, "wb+");
	/*assign*/
	db.fileStudent = file;
	db.listStudent = list;
	strcpy(db.listStudent.email, fileName);
	return db;
}

Database databaseOpen() {
	Database db;
	FILE* file;
	char fileName[34];
	printf("Please input filename of the database (witout suffix)");
	rewind(stdin);
	fgets(fileName, 30, stdin);
	fileName[strlen(fileName) - 1] = 0;
	strcat(fileName, ".txt");
	file = fopen(fileName, "rb");
	if (file == NULL) {
		db = databaseErrorNotfound(fileName);
		return db;
	}
	fclose(file);
	file = fopen(fileName, "rb+");
	return databaseLoad(fileName);
}

int databaseOperates(Database* db) {
	int choice;
	Student* buffer;
	system("cls");
	puts("Database is ready to use, following choices are available:");
	puts("1. Add a student;");
	puts("2. Search for studets's information;");
	puts("3. Sort and print database;");
	puts("0. Quit.");
	printf("Please choose one action:");
	choice = getChoice(3);
	switch (choice)
	{
	case 1:
		databaseAddElement(db);
		break;
	case 2:
		databaseSearchElement(db);
		break;
	case 3:
		databasePrint(db);
		break;
	case 0:
		return 1;
		break;
	default:
		puts("Program error: command not found;");
		quit();
		break;
	}
	return 0;
}

Database databaseErrorExist(char filename[]) {
	int choice;
	Database db;
	Student student = { "","",0,"",0,0,0,0,0,0,0,NULL,NULL };
	FILE* file;
	system("cls");
	puts("File already exist, you have following actions available:");
	puts("1. Open the file;");
	puts("2. Clear the content and open the file;");
	puts("3. Creat another file;");
	puts("0. Quit.");
	printf("Please choose one action:");
	choice = getChoice(3);
	switch (choice)
	{
	case 1:
		return databaseLoad(filename);
		break;
	case 2:
		file = fopen(filename, "wb+");
		db.fileStudent = file;
		db.listStudent = student;
		strcpy(db.listStudent.email, filename);
		break;
	case 3:
		return databaseNew();
		break;
	case 0:
		quit();
		break;
	default:
		puts("Program error: command not found;");
		quit();
		break;
	}
	return db;
}

void quit() {
	system("cls");
	puts("You are leaving the system");
	puts("Have fun with the database!");
	Sleep(1000);
	exit(0);
}

Database databaseLoad(char filename[]) {
	Student stuList = { "","",0,"",0,0,0,0,0,0,0,NULL,NULL };
	FILE* file = fopen(filename, "rb+");
	Database db;
	db.fileStudent = file;
	db.listStudent = stuList;
	strcpy(db.listStudent.email, filename);
	while (EOF != fgetc(file)) {
		fseek(file, -1, SEEK_CUR);
		Student* temp = (Student*)malloc(sizeof(Student));
		Student* pos = NULL;
		fread(temp, sizeof(Student), 1, file);
		temp->stdFormer = NULL;
		temp->stdLatter = NULL;
		listGetPos(&(db.listStudent), temp->numberID, &pos);
		elementInsert(pos, temp);
	}
	return db;
}

Database databaseErrorNotfound(char filename[]) {
	int choice;
	Database db;
	Student student = { "","",0,"",0,0,0,0,0,0,0,NULL,NULL };
	FILE* file;
	system("cls");
	puts("File is not found, you have following actions available:");
	puts("1. Choose another file to open;");
	puts("2. Creat this file;");
	puts("0. Quit.");
	printf("Please choose one action:");
	choice = getChoice(2);
	switch (choice)
	{
	case 1:
		return databaseOpen();
		break;
	case 2:
		file = fopen("filename", "wb+");
		db.fileStudent = file;
		db.listStudent = student;
		strcpy(db.listStudent.email, filename);
		break;
	case 0:
		quit();
		break;
	default:
		puts("Program error: command not found;");
		quit();
		break;
	}
	return db;
}

void elementInsert(Student* elementDest, Student* elementToInsert) {
	Student* temp;
	if (elementDest->stdLatter == NULL) {
		elementDest->stdLatter = elementToInsert;
		elementToInsert->stdFormer = elementDest;
	}
	else {
		temp = elementDest->stdLatter;
		elementDest->stdLatter = elementToInsert;
		temp->stdFormer = elementToInsert;
		elementToInsert->stdFormer = elementDest;
		elementToInsert->stdLatter = temp;
	}
}

int listGetPos(Student* list, int numberID, Student** buffer) {
	Student *pIndex;
	pIndex = list;
	while (pIndex->numberID < numberID && pIndex->stdLatter != NULL) {
		pIndex = pIndex->stdLatter;
	}
	if (pIndex->numberID == numberID) {
		*buffer = pIndex;
		return -1;
	}
	else if (pIndex->numberID > numberID) {
		pIndex = pIndex->stdFormer;
	}
	*buffer = pIndex;
	return 0;
}

void databaseAddElement(Database* db) {
	/*initialize*/
	Student* stu = (Student*)malloc(sizeof(struct StudentStrt));
	Student* buffer = NULL;
	char numberID[9];
	int duplicate = -1;
	strcat(stu->email, "");
	strcat(stu->nameFamily, "");
	strcat(stu->nameGiven, "");
	stu->grade1 = -1;
	stu->grade2 = -1;
	stu->grade3 = -1;
	stu->grade4 = -1;
	stu->grade5 = -1;
	stu->grade6 = -1;
	stu->gradeAvg = -1;
	stu->numberID = -1;
	stu->stdFormer = NULL;
	stu->stdLatter = NULL;
	system("cls");
	puts("Please fill following information:");
	/*ID number*/
	printf("Student ID:");
	rewind(stdin);
	fgets(numberID, 9, stdin);
	while (checkNum(numberID, 7) != 1) {
		puts("Incorrect format, ID number should be 7 numbers.");
		printf("Student ID:");
		rewind(stdin);
		fgets(numberID, 9, stdin);
	}
	stu->numberID = atoi(numberID);
	/*check if duplicate and add*/
	duplicate = listGetPos(&(db->listStudent), stu->numberID, &buffer);
	while (duplicate == -1) {
		puts("Duplicate ID number, try again");
		printf("Student ID:");
		rewind(stdin);
		fgets(numberID, 9, stdin);
		while (checkNum(numberID, 7) != 1) {
			puts("Incorrect format, ID number should be 7 numbers.");
			printf("Student ID:");
			rewind(stdin);
			fgets(numberID, 9, stdin);
		}
		stu->numberID = atoi(numberID);
		duplicate = listGetPos(&(db->listStudent), stu->numberID, &buffer);
	}
	/*family name*/
	printf("Family name:");
	rewind(stdin);
	fgets(stu->nameFamily, 20, stdin);
	stu->nameFamily[strlen(stu->nameFamily) - 1] = 0;
	/*given name*/
	printf("Given name:");
	rewind(stdin);
	fgets(stu->nameGiven, 20, stdin);
	stu->nameGiven[strlen(stu->nameGiven) - 1] = 0;
	/*email*/
	printf("Email address:");
	rewind(stdin);
	fgets(stu->email, 9, stdin);
	stu->email[strlen(stu->email) - 1] = 0;
	/*marks*/
	printf("1st mark:");
	stu->grade1 = markInput();
	printf("2nd mark:");
	stu->grade2 = markInput();
	printf("3rd mark:");
	stu->grade3 = markInput();
	printf("4st mark:");
	stu->grade4 = markInput();
	printf("5st mark:");
	stu->grade5 = markInput();
	printf("6st mark:");
	stu->grade6 = markInput();
	stu->gradeAvg = (stu->grade1 + stu->grade2 + stu->grade3 + stu->grade4 + stu->grade5 + stu->grade6) / 6;
	elementInsert(buffer, stu);
}

void databaseSave(Database* db) {
	Student* stu;
	fclose(db->fileStudent);
	fopen(db->listStudent.email, "wb+");
	stu = &(db->listStudent);
	while (stu->stdLatter != NULL) {
		stu = stu->stdLatter;
		elementSave(stu, db->fileStudent);
	}
}

void elementSave(Student* stu, FILE* file) {
	fwrite(stu, sizeof(Student), 1, file);
}

int checkNum(char str[], int length) {
	int i;
	for (i = 0; i < length; i++) {
		if ((str[i] - '0') > 10 || (str[i] - '0') < 0) {
			return 0;
		}
	}
	if (str[i] != '\n') {
		return 0;
	}
	return 1;
}

void databasePrint(Database* db) {
	Student* stu;
	system("cls");
	puts("The database has following records:\n");
	stu = &(db->listStudent);
	while (stu->stdLatter != NULL) {
		stu = stu->stdLatter;
		elementPrint(stu);
		puts("");
	}
	printf("Finished. Press enter to continue.");
	getchar();
}

void elementPrint(Student* stu) {
	printf(" Student %07d:\n", stu->numberID);
	printf("  Name:                %s %s;\n", stu->nameGiven, stu->nameFamily);
	printf("  Email:               %s\n", stu->email);
	printf("  Recent 6 grades:     %d; %d; %d; %d; %d; %d;\n", stu->grade1, stu->grade2, stu->grade3, stu->grade4, stu->grade5, stu->grade6);
	printf("  Average of 6 grades: %.2f;\n", stu->gradeAvg);
}

void databaseSearchElement(Database* db) {
	Student* stu = NULL;
	int numberIDIn;
	char numberIDCh[9];
	system("cls");
	printf("Enter the ID number of the student to search:");
	rewind(stdin);
	fgets(numberIDCh, 9, stdin);
	while (checkNum(numberIDCh, 7) != 1) {
		puts("Incorrect format, ID number should be 7 numbers.");
		printf("Try Again:");
		rewind(stdin);
		fgets(numberIDCh, 9, stdin);
	}
	numberIDIn = atoi(numberIDCh);
	if (-1 == listGetPos(&(db->listStudent), numberIDIn, &stu)) {
		puts("");
		elementPrint(stu);
	}
	else {
		puts("\n Student not found.\n");
	}
	printf("\nPress enter to contimue.");
	getchar();
}

int markInput() {
	int i;
	rewind(stdin);
	scanf("%d", &i);
	while (i < 0 || i > 100) {
		puts("Incorrect format, the mark should be integer 0 - 100.");
		printf("Try again:");
		rewind(stdin);
		scanf("%d", &i);
	}
	return i;
}