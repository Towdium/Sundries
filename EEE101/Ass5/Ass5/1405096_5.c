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
int databaseOperates(Database* db);
void quit();
void elementInsert(Student *elementDest, Student *elementToInsert);
void listAdd(Student* index, Student* elementToAdd);
Student* listGetPos(Student* index, int numberID);
void databaseAddElement(Database* db);
Student* inputStudent();
void databaseSave(Database* db);
int checkNum(char str[], int length);
void databasePrint(Database* db);
void elementPrint(Student* stu);
void elementSave(Student* stu, FILE* file);



int main() {
	char a[10];
	int inputChoice, i = 0;
	Student list = { "","",0,"",0,0,0,0,0,0,0,NULL,NULL };
	Database db;
	db.listStudent = list;
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
	char fileName[34];
	printf("Please input filename of the database (witout suffix)");
	rewind(stdin);
	fgets(fileName, 30, stdin);
	fileName[strlen(fileName) - 1] = 0;
	strcat(fileName, ".txt");
	file = fopen(fileName, "r");
	if (file != NULL) {
		fclose(file);
		db = databaseErrorExist(fileName);
		return db;
	}
	file = fopen(fileName, "w+");
	db.listStudent.numberID = 0;
	db.fileStudent = file;
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
	file = fopen(fileName, "r");
	if (file == NULL) {
		fclose(file);
		db = databaseErrorNotfound(fileName);
		return db;
	}
	fclose(file);
	file = fopen(fileName, "r");
	db = databaseLoad(fileName);
	return db;
}

int databaseOperates(Database* db) {
	int choice;
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
		db = databaseLoad(filename);
		break;
	case 2:
		file = fopen(filename, "w+");
		db.fileStudent = file;
		db.listStudent = student;
		break;
	case 3:
		db = databaseNew();
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
	Database db = { &stuList, NULL };
	puts("* File loaded");
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
		db = databaseOpen();
		break;
	case 2:
		file = fopen("filename", "w+");
		db.fileStudent = file;
		db.listStudent = student;
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

void listAdd(Student* index, Student* elementToAdd) {
	elementInsert((listGetPos(index, elementToAdd->numberID)), elementToAdd);
}

Student* listGetPos(Student* index, int numberID) {
	Student *pIndex;
	pIndex = index;
	while (pIndex->numberID < numberID && pIndex->stdLatter != NULL) {
		pIndex = pIndex->stdLatter;
	}
	if (pIndex->numberID == numberID) {
		printf("Program error: duplicate ID number.");
		quit();
	}
	else if (pIndex->numberID > numberID) {
		pIndex = pIndex->stdFormer;
	}
	return pIndex;
}

void databaseAddElement(Database* db) {
	system("cls");
	listAdd(&(db->listStudent), inputStudent());
	databaseSave(db);
}

Student* inputStudent() {
	Student* stu = (Student*)malloc(sizeof(struct StudentStrt));
	char numberID[9];
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
	/*family name*/
	puts("Please fill following information:");
	printf("Family name:");
	rewind(stdin);
	fgets(stu->nameFamily, 20, stdin);
	stu->nameFamily[strlen(stu->nameFamily) - 1] = 0;
	/*given name*/
	printf("Given name:");
	rewind(stdin);
	fgets(stu->nameGiven, 20, stdin);
	stu->nameGiven[strlen(stu->nameGiven) - 1] = 0;
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
	/*email*/
	printf("Email address:");
	rewind(stdin);
	fgets(stu->email, 9, stdin);
	stu->email[strlen(stu->email) - 1] = 0;
	/*1st mark*/
	printf("1st mark:");
	rewind(stdin);
	scanf("%d", &stu->grade1);
	while (stu->grade1 < 0 || stu->grade1>100) {
		puts("Incorrect format, the mark should be integer 0 - 100.");
		printf("1st mark:");
		rewind(stdin);
		scanf("%d", &stu->grade1);
	}
	/*2nd mark*/
	printf("2nd mark:");
	rewind(stdin);
	scanf("%d", &stu->grade2);
	while (stu->grade2 < 0 || stu->grade2>100) {
		puts("Incorrect format, the mark should be integer 0 - 100.");
		printf("2nd mark:");
		rewind(stdin);
		scanf("%d", &stu->grade2);
	}
	/*3rd mark*/
	printf("3rd mark:");
	rewind(stdin);
	scanf("%d", &stu->grade3);
	while (stu->grade3 < 0 || stu->grade3>100) {
		puts("Incorrect format, the mark should be integer 0 - 100.");
		printf("3rd mark:");
		rewind(stdin);
		scanf("%d", &stu->grade3);
	}
	/*4st mark*/
	printf("4st mark:");
	rewind(stdin);
	scanf("%d", &stu->grade4);
	while (stu->grade4 < 0 || stu->grade4>100) {
		puts("Incorrect format, the mark should be integer 0 - 100.");
		printf("4st mark:");
		rewind(stdin);
		scanf("%d", &stu->grade4);
	}
	/*5st mark*/
	printf("5st mark:");
	rewind(stdin);
	scanf("%d", &stu->grade5);
	while (stu->grade5 < 0 || stu->grade5>100) {
		puts("Incorrect format, the mark should be integer 0 - 100.");
		printf("5st mark:");
		rewind(stdin);
		scanf("%d", &stu->grade5);
	}
	/*6st mark*/
	printf("6st mark:");
	rewind(stdin);
	scanf("%d", &stu->grade6);
	while (stu->grade6 < 0 || stu->grade6>100) {
		puts("Incorrect format, the mark should be integer 0 - 100.");
		printf("6st mark:");
		rewind(stdin);
		scanf("%d", &stu->grade6);
	}
	stu->gradeAvg = (stu->grade1 + stu->grade2 + stu->grade3 + stu->grade4 + stu->grade5 + stu->grade6) / 6;
	return stu;
}

void databaseSave(Database* db) {
	Student* stu;
	stu = &(db->listStudent);
	while (stu->stdLatter != NULL) {
		stu = stu->stdLatter;
		elementSave(stu, db->fileStudent);
	}
}

void elementSave(Student* stu, FILE* file) {

}

int checkNum(char str[8], int length) {
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
	}
	puts("\nFinished.");
	getchar();
}

void elementPrint(Student* stu) {
	printf(" Student %07d:\n", stu->numberID);
	printf("  Name:                %s %s;\n", stu->nameGiven, stu->nameFamily);
	printf("  Email:               %s", stu->email);
	printf("  Recent 6 grades:     %d; %d; %d; %d; %d; %d;\n", stu->grade1, stu->grade2, stu->grade3, stu->grade4, stu->grade5, stu->grade6);
	printf("  Average of 6 grades: %f;\n", stu->gradeAvg);
}
