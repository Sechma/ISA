#ifndef ARGPARSE_H_
#define ARGPARSE_H_


#include "mytftpclient.h"

/*
Struktura reprezentuje možné parametry
*/
struct flags{
			int RW_flag = -1; /*R = 1 W = 0 missing = -1*/
			bool multi_flag=false;
			int counter_RW=0;
			std::string path;
			int timeout = 0;
			int high_pass = 0;
			std::string ip_adress = "127.0.0.1";
			bool ipv6_flag = false;
			std::string port = "69";
			std::string mode ="binary";
};
/*
Funkce parsing rozrazuje parametry a popripade vraci chybu a ukoncuje program a vypisuje hlasku 
Vstupy:
	int argc2: pocet argumentu
	char *argv2[] pole stringu, ve kterem jsou parametry z prikazove řádky
Vystup:
	Pokud dojde k chybě, dojde k ukončení programu.
	Jinak naplní strukturu flags daty.
*/
flags parsing(int argc2, char* argv2[]);

/*
Funkce is_ipv4 overuje jestli je vstupni pole charu ve tvaru ipv4 
a popripade vraci port.

Vstupy:
	char *add : pole charu, reprezentujici adresu
	int length : delka adresy
Výstup:
	0 - ipv4 bez portu
	1 - neni to ipv4 adresa
	cislo - je to ipv4 a vracim cislo portu

*/
/*
Source:http://www.zedwood.com/article/cpp-is-valid-ip-address-ipv4-ipv6*/

bool is_ipv4(const std::string& str,std::string *default_port);
bool is_ipv6(const std::string& str,std::string *default_port);

#endif