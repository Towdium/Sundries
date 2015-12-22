#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <process.h> 

int ma_Login();
void utl_CheckString(char str[]);

int main() {
	ma_Login();
}

int ma_Login() {
	char pw[10] = "1342cp7048", input_pw[25];

	while (1) {
		printf("\t(Enter capital E to exit.)\nPlease enter the password to access manager interface: ");
		rewind(stdin);
		fgets(input_pw, 25, stdin);
		utl_CheckString(input_pw);

		if (input_pw[0] == 'E' && input_pw[1] == '\0') {
			system("cls");
			return 0;
		}
		if (strcmp(input_pw, pw) == 0) {
			printf("\n\n\n\t\t\tHello! Manager~");
			Sleep(1500);
			system("cls");
			return 1;
		}
		printf("\t\tPassword entered is incorrect.\n\n");
	}
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