
#include "argparse.h"

	flags parsing(int argc2, char* argv2[]){
		flags flag;
		int opt;
		while( (opt = getopt(argc2,argv2,"RWd:t:s:mc:a:")) != -1){
			switch(opt){
				case('R'):
					flag.R_flag = true;
				case ('W'):
					flag.counter_RW++;
					if(flag.counter_RW > 1){
						std::cerr << "You coudnt use write and read in one command!";
					}
					break;
				case ('d'): /*Absolutni cesta /*/
					flag.path = optarg;
					break;
				case ('t'):
					flag.timeout = std::stoi(optarg);
					std::cout << flag.timeout;
					break;
				case ('s'):
					flag.high_pass = std::stoi(optarg); /* == int(optarg)*/
					break;
				case ('m'):
					flag.multi_flag = true;
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
						exit(1);
					}
					break;
				case ('a'):
					flag.adress = optarg;
					break;
				default:
					std::cerr<<"Example of usage: " << argv2[0] << " -R/W" << " -d /home/user/readme.txt" << std::endl;
					exit(1);
			}
		}
		return flag;
	}