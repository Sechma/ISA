
#include "argparse.h"

	flags parsing(int argc2, char* argv2[]){
		flags flag;
		int opt;
		while( (opt = getopt(argc2,argv2,"RWd:t:s:mc:a:")) != -1){
			switch(opt){
				case('R'):
					flag.RW_flag = 1;
				case ('W'):
					flag.RW_flag = 0;
					flag.counter_RW++;
					if(flag.counter_RW > 1){
						std::cerr << "You coudnt use write and read in one command!"<<std::endl;
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
						flag.mode = "binary";
					}
					else if(strcmp("octet", optarg)== 0){
						flag.mode = "octet";
					}
					else if(strcmp("netascii", optarg)== 0){
						flag.mode = "netascii";
					}
					else if (strcmp("ascii", optarg)== 0){
						flag.mode = "ascii";
					}
					else{
						std::cerr << "Wrong mode for parametr -c "<< std::endl<<"Try: -c binary/octet/ascii/netascii";
						exit(1);
					}
					break;
				case ('a'):
					flag.ip_adress = optarg;
					if (is_ipv4(flag.ip_adress,&flag.port) == 0){
						if(is_ipv6(flag.ip_adress,&flag.port) == 0){
							std::cerr << "Wrong IPv4 or IPv6 address" <<std::endl;
							exit(1);
						}
						else
							flag.ipv6_flag = true;
					}
					break;
				default:
					std::cerr<<"Example of usage: " << argv2[0] << " -R/W" << " -d /home/user/readme.txt" << std::endl;
					exit(1);
			}
		}
		if(flag.RW_flag == -1 || flag.path == ""){
			std::cerr << "Mising compulsory parametres" << std::endl << "Example of usage: -R/W" << " -d /home/user/readme.txt" << std::endl;
			exit(1);
		}
		return flag;
		
	}

bool is_ipv4(const std::string& str, std::string *default_port){
	int tmp = str.length()-1;
	std::string port;
	std::string new_add;

	struct sockaddr_in sa;
	if (inet_pton(AF_INET, str.c_str(), &(sa.sin_addr))==0)//not ipv4 or with port
	{
		for(int i=0; i<6;i++){ // max port 5 cifer .. 65000
			if(str[tmp-i] == ':'){
				std::reverse(port.begin(), port.end());
				break;
			}
			port += str[tmp-i];
		}
		new_add = str.substr(0,tmp-port.length());
		
		if(inet_pton(AF_INET, new_add.c_str(), &(sa.sin_addr))!=0){
			*default_port = port;
			return true;
		}
		else
			return false;
	}
	else
		return true;
	
}


bool is_ipv6(const std::string& str,std::string *default_port){
	/*struct sockaddr_in6 add;
	return inet_pton(AF_INET6, str.c_str(), &(add.sin6_addr))!=0;*/

	int tmp = str.length()-1;
	std::string port;
	std::string new_add;

	struct sockaddr_in6 sa;
	if (inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr))==0)//not ipv4 or with port
	{
		for(int i=0; i<6;i++){ // max port 5 cifer .. 65000
			if(str[tmp-i] == ':'){
				std::reverse(port.begin(), port.end());
				break;
			}
			port += str[tmp-i];
		}
		new_add = str.substr(0,tmp-port.length());
		
		if(inet_pton(AF_INET6, new_add.c_str(), &(sa.sin6_addr))!=0){
			*default_port = port;
			return true;
		}
		else
			return false;
	}
	else
		return true;
}