/* Project:    ISA - TFTP Klient
 University: Brno University of Technology, FIT
 Date:       14.11. 2021
 Author:  Marek Sechra <xsechr00@stud.fit.vutbr.cz>
*/

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
#include <chrono>
#include <ctime> 
#include <sstream>

#include "argparse.h"


/*Macros*/

/*OPCODES 2bytes*/
#define RRQ 01
#define WRQ 02
#define DATA 03
#define ACK 04
#define ERROR 05
#define OACK 06
/*
 0X - this zero is there for fix little endian problems on different architecture
*/

int read_tftp(int *sock, char* buffer_tftp, int msg_size,struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6);
/* Function: read_tftp() - represent client if user want read from server and doing this stuff
	Input:
		int sock = simply, socket
		char *buffer_tftp = represent all data which are neccesery for sending requests
		int msg_size = how big is buffer_tftp
		struct flags *flag = pointer on my own structure, there are all information about inputs from user
		sockaddr_in/6 = adress,port,hostname for sending requests
	Output:
		always 0
*/
int my_sendto(int *sock,char *buffer, int msg, struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6 );
/* Function my_sendto() zip send_to functions for ipv4 and ipv6
	Input/Output are similaary like function read_tftp()
*/
int my_recvfrom(int *sock, char* buffer, int size, struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6 );
/* Function my_sendto() zip recvfrom functions for ipv4 and ipv6
	Input/Output are similaary like function read_tftp()
*/
void wait_for_ack(int *sock,struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6,socklen_t *server_len);
/* Function wait_for_ack waiting for ack from server
	Input/Output are similaary like function read_tftp()
*/
std::string file_name(std::string const & path);
/*
	Function is inspirated by - 
	Source: https://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path
*/

void print_actual_time();
/*
	Function print actual time 
*/

std::string tsize_write(std::string const & filename);
/*
Function tsize_write returning size of file which name is on input like string
INPUT: std::string
OUPUT std::string
*/
#endif