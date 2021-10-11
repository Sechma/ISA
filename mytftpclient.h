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
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4

#define ERROR 5

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
#endif