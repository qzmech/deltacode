#include <iostream>
#include <fstream>
#include <string>
#include "DCODE_PROCESS.h"

using namespace std;

int main(int argc, char *file[])
{
	system("title Delta_Commander_v0.8 [alpha]");
	setlocale(LC_ALL, "Russian");
	string filename = "";

	if (argc > 1)
	{
		filename = file[1];

		DCODE_PROCESS program(":0", filename);
		_register result = program.start();

		if (result.value_register != "~null")
		{
			cout << endl << "Program terminated with code " << result.value_register << endl;
			cout << "Press any key to exit...";
		}
	}
	else
	{
		cout << "Delta_Commander. Copyright(C) Kuzmichev Nicolay 2018" << endl;
		cout << ">> ";
		getline(cin, filename);
		DCODE_PROCESS program(":0", filename);
		_register result = program.start();
		cout << endl << "Program terminated with code " << result.value_register << endl;
		cout << "Press any key to exit...";
	}
	getchar();
	getchar();
	return 0;
}