#include "mytftpclient.h"

int main( int argc, char* argv[]) {
	if(argc < 3){
		std::cerr << "Mising parametres!"<< std::endl << "Example of usage: " << argv[0] << " -R/W" << " -d /home/user/readme.txt" << std::endl;
		/*nemusí byt implicitne adresa a port uvazujese automaticky 
		o 127.0.0.1:69*/
		return 1;
	}
	else{
		flags flag = parsing(argc, argv);

		int sock;
		if ((sock = socket(AF_INET , SOCK_DGRAM , 0)) == -1){
			std::cerr << "socket failed!" << std::endl << "Try again" << std::endl;
		}
		/*Init*/
		struct sockaddr_in server;
		server.sin_addr.s_addr = inet_addr(flag.ip_adress.c_str());
		if(flag.ipv6_flag){
			server.sin_family = AF_INET6;
		}
		server.sin_family = AF_INET;
		server.sin_port = htons(std::stoi(flag.port));
		
		/*My own terminal*/
		time_t act_time = time(0);
		std::string t = ctime(&act_time);

		/*Its time build Request packet for start communication*/
		/*
			IP HEADER = 20B
			UPD HEADER = 8B
			OPCDE  1/2 (RRQ/WRQ) = 2B
			FILENAME = XB
			space 0 = 1B
			MODE NB
			space 0 = 1B
		
		*/
		char buffer_tftp[512],*p_buff;
		*(short*)buffer_tftp = htons(RRQ); /*OPCODE 1/2*/
		p_buff = buffer_tftp + 2;
		strcpy(p_buff,flag.path.c_str()); /* /home/my/file.txt*/
		p_buff += flag.path.length()+1;		/*posun o delu path + nula*/
		strcpy(p_buff,flag.mode.c_str()); /* MODE(ascii,binary,..)*/
		p_buff += flag.mode.length()+ 1;

	/*	while(1){
			std::cout  << t << std::endl;
			std::cout << "	data sendig.." << std::endl;
			//todo buffer etc.
		}
	*/


		close(sock);
	}
	return 0;
}
