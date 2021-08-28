#include <Windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include "xor.h"
#include "lw_http.hpp"
#include <ostream>
#include <regex>
#include "print.h"
#include "hwid.h"
#include "md5wrapper.h"
#include "crypto.h"
#include "auth.h"
#include "cleaner.cpp"

char title[100];

extern c_crypto crypto;


void HideConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

void strGetRandomAlphaNum(char* sStr, unsigned int iLen)
{
	char Syms[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned int Ind = 0;
	srand(time(NULL) + rand());
	while (Ind < iLen)
	{
		sStr[Ind++] = Syms[rand() % 62];
	}
	sStr[iLen] = '\0';
}


int main()
{
	strGetRandomAlphaNum(title, 8);
	SetConsoleTitle(title);
	std::string pack_id = "1";
	std::string license_key;
	std::wstring packid;
	system("start notepad.exe");
	Sleep(3000);
	system("taskkill /f /im notepad.exe >nul 2>&1");
	system("taskkill /f /im EpicGamesLauncher.exe >nul 2>&1");
	system("taskkill /f /im UnrealCEFSubProcess.exe >nul 2>&1");
	system("taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1");
	system("CLS");
	ShowConsole();
	const int is_valid_user = auth::get_valid_user(pack_id);
	if (!is_valid_user) // if its not registered.. he asking the serial number for activation.
	{

		std::cout << "INFO : please enter your serial number\n";
		std::getline(std::cin, license_key);
		auth::set_license_key(license_key, pack_id);

	}
	//else // is already registerd.. ok is different serial look because of id. wait i wrong something.
	//{
	//	char buffer[256];
	//	sprintf(buffer, "Your HWID:  %s\nPackage ID: %s\nExpiration Date: %s\n\n\n", md5->getHashFromString(Serial::GetHardwareId(pack_id.c_str())).c_str(), pack_id.c_str(), auth::get_sub_info(pack_id.c_str()).c_str());
	//	print::set_ok(buffer);
	//}
}
