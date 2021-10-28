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

		int sock, msg_size;
		struct sockaddr_in server;
		struct sockaddr_in6 server6;
		socklen_t server_len; // #todo
		/*Init*/
		if(flag.ipv6_flag){
			sock = socket(AF_INET6 , SOCK_DGRAM , 0);
			server6.sin6_family = AF_INET6;
			inet_pton(AF_INET6, flag.ip_adress.c_str(), &server6.sin6_addr);
			server6.sin6_port = htons(std::stoi(flag.port));
			server_len = sizeof(server6);
		}
		else{
			sock = socket(AF_INET , SOCK_DGRAM , 0);
			server.sin_family = AF_INET;
			server.sin_addr.s_addr = inet_addr(flag.ip_adress.c_str());
			server.sin_port = htons(std::stoi(flag.port));
			server_len = sizeof(server);
		}
		
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
		std::string blc_size = "blksize";
		std::string tmt = "timeout";
		unsigned int index = 0;
		//buffer_tftp[MAX_OCTET+32]
		char buffer_tftp[flag.high_pass],*p;
		int c;
		if( flag.RW_flag )
			*(short*)buffer_tftp = htons(RRQ);
		else
			*(short*)buffer_tftp = htons(WRQ);

		p = buffer_tftp + 2;

		strcpy(p,flag.path.c_str()); /* /home/my/file.txt*/
		p += flag.path.length()+1;
		index += flag.path.length()+1; /*posun o delu path + nula*/

		strcpy(p,flag.mode.c_str()); /* MODE(ascii,binary,..)*/
		p += flag.mode.length()+ 1;
		index += flag.mode.length()+1;
		if( flag.high_pass != 512){
			strcpy(buffer_tftp,blc_size.c_str());
			p += blc_size.length()+ 1;
			index += blc_size.length()+1;
			
			buffer_tftp[index] = flag.high_pass;
			p += sizeof(unsigned int)+1;
			index += sizeof(unsigned int)+1;
		}
		if(flag.timeout != 0 ){
			strcpy(buffer_tftp,tmt.c_str());
			p += tmt.length()+ 1;
			index += tmt.length()+1;

			buffer_tftp[index] = flag.timeout;
			p += sizeof(unsigned int)+1;
			index += sizeof(unsigned int)+1;
		}
		msg_size = p - buffer_tftp;
		if(flag.RW_flag ){
			//read_tftp(&sock, buffer_tftp ,msg_size,&flag.high_pass, (struct sockaddr_in *) &server, (struct sockaddr_in6 *) &server6,flag.ipv6_flag);
			read_tftp(&sock, buffer_tftp ,msg_size,&flag, (struct sockaddr_in *) &server, (struct sockaddr_in6 *) &server6);
		}
		else{
			char buffer[flag.high_pass];
			FILE * file_w;
 			file_w = fopen(flag.path.c_str(),"rb");
			while (fread(buffer, 512, 1,file_w ) == 1){
				std::cout << buffer;
			}
			fclose(file_w);
		}

		close(sock);
	}
	return 0;
}


/*
Unsigned char z chamar

*/

/*Function*/
//int read_tftp(int *sock, char* buffer_tftp, int msg_size, unsigned int *high_pass,sockaddr_in *server,sockaddr_in6 *server6, bool ipv6){
int read_tftp(int *sock, char* buffer_tftp, int msg_size,flags *flag,sockaddr_in *server,sockaddr_in6 *server6){
	int c;
	socklen_t server_len;
	std::string filename = file_name(flag->path);
	FILE *file_r;
	file_r = fopen(filename.c_str(),"wb+");


	if(!flag->ipv6_flag){
		server_len = sizeof(*server);
		c = sendto(*sock,buffer_tftp,msg_size,0,(struct sockaddr *) server, server_len);
	}
	else{
		server_len = sizeof(*server6);
		c = sendto(*sock,buffer_tftp,msg_size,0,(struct sockaddr *) server6, server_len);
	}
	do {
		if(!flag->ipv6_flag) server_len = sizeof(*server);
		else server_len = sizeof(*server6);

		if(!flag->ipv6_flag) c = recvfrom(*sock,buffer_tftp,flag->high_pass,0,(struct sockaddr*) server, &server_len);
		else c = recvfrom(*sock,buffer_tftp,flag->high_pass,0,(struct sockaddr*) server6, &server_len);

		if ( ntohs(*(short *)buffer_tftp) == ACK) fprintf(stderr, "rcat: %s\n", buffer_tftp+4);
		else {
			fwrite(buffer_tftp, 1, c , file_r);
			//write(1, buffer_tftp+4, c-4); //only for debug
			*(short *)buffer_tftp = htons(ACK);
			if(! flag->ipv6_flag) sendto(*sock, buffer_tftp, 4, 0, (struct sockaddr *) server,server_len);
			else sendto(*sock, buffer_tftp, 4, 0, (struct sockaddr *) server6,server_len);
		}
	} while (c == 516);

	fclose(file_r);
	return 0;
}
// Source: https://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path
std::string file_name(std::string const & path){
	return path.substr(path.find_last_of("/\\") + 1);
}