#ifndef MYTFTPCLIENT_H_
#define MYTFTPCLIENT_H_

#include <iostream>
#include <unistd.h>
#include <string>
#include <algorithm>
#include<bits/stdc++.h>
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>
#include<err.h>
#include <ctime>


#include "argparse.h"


/*Macros*/

/*OPCODES 2bytes*/
#define RRQ 01
#define WRQ 02
#define DATA 03
#define ACK 04
/*
 0X - 0 je zde důvodu problematiky big/little endian způsob, který používá TFTP je opačný než od ostatních architektur
*/
#define ERROR 05
#define RETRANSMIT_TIMEOUT 2000000
#define SLEEP_TIMEOUT 1000
/*TFTP erro kody */
struct err_codes{
	short value[8] = {0,1,2,3,4,5,6,7};
	std::string meaning[8] = {
		"Not defined, see error message",
		"File not found.",
		"Access violation.",
		"Disk full or allocation exceeded.",
		"Illegal TFTP operation.",
		"Unknown transfer ID",
		"File already exists",
		"No such user"
	};
};




// 2scs
//int read_tftp(int *sock, char* buffer_tftp, int msg_size, unsigned int *high_pass,sockaddr_in *server, sockaddr_in6 *server6,bool ipv6);
int read_tftp(int *sock, char* buffer_tftp, int msg_size,struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6);

std::string file_name(std::string const & path);



#endif