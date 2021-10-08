#include <iostream>
#include <unistd.h>
#include <string>

#include<bits/stdc++.h>
#include "mytftpclient.h"


int main( int argc, char* argv[]) {
	if(argc < 3){
		std::cerr << "Mising parametres!"<< std::endl << "Example of usage: " << argv[0] << " -R/W" << " -d /home/user/readme.txt" << std::endl;
		/*nemusí byt implicitne adresa a port uvazujese automaticky 
		o 127.0.0.1:69*/
		return 1;
	}
	else{
		int opt,counter_RW = 0;
		bool R_flag = false; /* Flag uchovva hodnotu parametru pro cteni nebo zapis */
		bool multi_flag = false;/*Flag for multicast*/
		while( (opt = getopt(argc,argv,"RWdtsmc:a:")) != -1){
			switch(opt){
				case('R'):
					R_flag = true;
				case ('W'):
					counter_RW++;
					if(counter_RW > 1){ /*Bylo zadane R a W soucasne*/
						std::cerr << "You coudnt use write and read in one command!";
					}
					break;
				case ('d'): /*Absolutni cesta /*/
					break;
				case ('t'):
					break;
				case ('s'):
					break;
				case ('m'):
					multi_flag = true;
					break;
				case ('c'):
					if(strcmp("binary", optarg)== 0){
						std::cout <<"c:"<< optarg << std::endl;
					}
					else if(strcmp("octet", optarg)== 0){
						std::cout <<"c:"<< optarg << std::endl;
					}
					else if(strcmp("netascii", optarg)== 0){
						std::cout <<"c:"<< optarg << std::endl;
					}
					else if (strcmp("ascii", optarg)== 0){
						std::cout <<"c:"<< optarg << std::endl;
					}
					else{
						std::cerr << "Wrong mode for parametr -c "<< std::endl<<"Try: -c binary/octet/ascii/netascii";
						return 1;
					}
					break;
				case ('a'):
					break;
				default:
					std::cerr<<"Example of usage: " << argv[0] << " -R/W" << " -d /home/user/readme.txt" << std::endl;
					return 1;
			}
		}
	}
	return 0;
}
