#include "auth.h"
#include "lw_http.hpp"
#include "md5wrapper.h"
#include "print.h"
#include "hwid.h"
#include "xor.h"
#include "crypto.h"
#ifdef _WIN32
#include <io.h> 
#define access    _access_s
#else
#include <unistd.h>
#endif

bool FileExists(const std::string& Filename)
{
	return access(Filename.c_str(), 0) == 0;
}
c_crypto crypto;



bool replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void auth::set_license_key(const std::string serial, const std::string id)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return;
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), md5->getHashFromString(Serial::GetHardwareId(id)).c_str());
	lw_http_d.add_field(xor_a("b"), crypto.key.c_str());
	lw_http_d.add_field(xor_a("c"), crypto.iv.c_str());
	lw_http_d.add_field(xor_a("d"), id.c_str());
	lw_http_d.add_field(xor_a("e"), serial.c_str());
	const auto b_lw_http = lw_http.post(L"/////", s_reply, lw_http_d);
	lw_http.close_session();
	if (!b_lw_http)
	{
		return;
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "201"))
	{
		print::set_error(xor_a("INFO : Sorry, look wrong application id.\n"));
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "202"))
	{
		print::set_warning(xor_a("INFO : Application is paused cheat is update.\n"));
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "203"))
	{
		print::set_error(xor_a("INFO : Sorry, Hardware id is invalid please reset it.\n"));
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "100"))
	{
		print::set_error(xor_a("INFO : You enter wrong serial key.\n"));
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "101"))
	{
		print::set_error(xor_a("INFO : Sorry, serial key already used by other.\n"));
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "102"))
	{
		print::set_error(xor_a("INFO : Sorry, you enter invalid serial key.\n"));
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "103"))
	{
		std::remove("C:\\Windows\\IME\\client.txt");
		print::set_error(xor_a("INFO : Sorry, serial key has been expired.\n"));
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "104"))
	{
		print::set_error(xor_a("INFO : Sorry, user already unsed.\n"));
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), ("200")))
	{
		if (FileExists("C:/Windows/IME/client.txt"))
		{
			system("cls");
			std::string pack_id = "1";
			char buffer[256];
			sprintf(buffer, "Your HWID:  %s\nPackage ID: %s\nExpiration Date: %s\n\n\n", md5->getHashFromString(Serial::GetHardwareId(pack_id.c_str())).c_str(), pack_id.c_str(), auth::get_sub_info(pack_id.c_str()).c_str());
			print::set_ok(buffer);
			Sleep(3000);
			system("CLS");
			print::set_ok("\n  INFO : F2 to inject\n");
			while (1)
			{
				if (GetAsyncKeyState(VK_F2) & 1)
				{
					system("cls");
					Sleep(100);
					Beep(222, 100);
					print::set_warning("\n  INFO : Injecting..\n");
					Sleep(5000);
					print::set_ok("\n  INFO : Injected successfully !\n");
					system("pause");
					system("exit");

				}
			}
		}
		else
		{
			system("echo. > C:/Windows/IME/client.txt");
			print::set_ok(xor_a("INFO : Please re-open loader.\n"));
			system("pause");
		}
	
	}
}

int auth::get_valid_user(const std::string& id)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return 0;
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), md5->getHashFromString(Serial::GetHardwareId(id)).c_str());
	lw_http_d.add_field(xor_a("b"), crypto.key.c_str());
	lw_http_d.add_field(xor_a("c"), crypto.iv.c_str());
	lw_http_d.add_field(xor_a("d"), id.c_str());
	lw_http_d.add_field(xor_a("e"), xor_a(""));
	const auto b_lw_http = lw_http.post(xor_w(L"/////"), s_reply, lw_http_d);
	lw_http.close_session();

	if (!b_lw_http)
	{
		return 0;
	}

	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "201"))
	{
		print::set_error(xor_a("INFO : Sorry, look wrong application id.\n"));
		return 0;
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "202"))
	{
		print::set_error(xor_a("INFO : Application is paused cheat is update.\n"));
		return 0;
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "203"))
	{
		print::set_ok(xor_a("INFO : Initialized successsfully.\n"));
		return 0;
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "100"))
	{
		print::set_error(xor_a("INFO : You enter wrong serial key.\n"));
		return 0;
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "101"))
	{
		print::set_error(xor_a("INFO : Sorry, serial key already used by other.\n"));
		return 0;
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "102"))
	{
		printf(xor_a("INFO : Sorry, you enter invalid serial key.\n"));
		return 0;
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "103"))
	{
		print::set_error(xor_a("INFO : Sorry, serial key has been expired.\n"));
		return 0;
	}
	if (!strcmp(c_crypto::decrypt(s_reply, crypto.key, crypto.iv).c_str(), "200"))
	{
		return 0x100;
	}

	return 0;
};

std::string auth::get_sub_info(const std::string id)
{
	c_lw_http	lw_http;
	c_lw_httpd	lw_http_d;
	auto md5 = new md5wrapper();
	if (!lw_http.open_session())
	{
		return{};
	}
	std::string s_reply;
	lw_http_d.add_field(xor_a("a"), md5->getHashFromString(Serial::GetHardwareId(id)).c_str());
	lw_http_d.add_field(xor_a("b"), crypto.key.c_str());
	lw_http_d.add_field(xor_a("c"), crypto.iv.c_str());
	lw_http_d.add_field(xor_a("d"), id.c_str());
	lw_http_d.add_field(xor_a("e"), xor_a("info"));
	const auto b_lw_http = lw_http.post(L"/////", s_reply, lw_http_d);
	if (!b_lw_http)
	{
		return{};
	}
	lw_http.close_session();
	std::string sstring(crypto.decrypt(s_reply.c_str(), crypto.key.c_str(), crypto.iv.c_str()).c_str());
	replace(sstring, "\"", "");
	replace(sstring, "\"", "");
	char data[256];
	sprintf(data, xor_a("%s"), sstring.c_str());
	return std::string(data);
}