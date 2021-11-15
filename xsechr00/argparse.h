/* Project:    ISA - TFTP Klient
 University: Brno University of Technology, FIT
 Date:       14.11. 2021
 Author:  Marek Sechra <xsechr00@stud.fit.vutbr.cz>
*/

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
			unsigned int timeout = 0;
			unsigned int high_pass = 512;
			std::string s_high_pass = "512";
			std::string ip_adress = "127.0.0.1";
			bool ipv6_flag = false;
			std::string port = "69";
			std::string mode ="octet";
};

#define MAX_OCTET 1468 /*MAX OCTET IN IP (MTU)*/

flags parsing(int argc2, char* argv2[]);
/*
Function parsing() parsing arguments from stdin a write err_codes to sderr 
Inputs:
	int argc2: number of arguments
	char *argv2[] array of string, arguments are there
Otputs:
	If function found some invalid input >> stderr
	Else fill structure for next time
*/
bool is_ipv4(const std::string& str,std::string *default_port,std::string *new_ad);
bool is_ipv6(const std::string& str,std::string *default_port,std::string *new_ad6);
/*
Function is_ipv4/6 checkinf if ipv4/6 address is in correct form

Input:
	char *add : address in ipv4 (maybe)
	int length :length of address
Otput:
	0 - only address without port
	1 - means error, wrong format of ipv4
	num - address is correct and num represent port of ipv4

Inspirated by source:http://www.zedwood.com/article/cpp-is-valid-ip-address-ipv4-ipv6
*/




#endif