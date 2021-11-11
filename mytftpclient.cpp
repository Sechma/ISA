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
			strcpy(p,blc_size.c_str());
			p += blc_size.length()+ 1;
			index += blc_size.length()+1;
			
			p[index] = flag.high_pass;
			p += sizeof(unsigned int)+1;
			index += sizeof(unsigned int)+1;
		}
		if(flag.timeout != 0 ){
			strcpy(p,tmt.c_str());
			p += tmt.length()+ 1;
			index += tmt.length()+1;

			p[index] = flag.timeout;
			p += sizeof(unsigned int)+1;
			index += sizeof(unsigned int)+1;
		}
		msg_size = p - buffer_tftp;

		if(flag.RW_flag ){
			//read_tftp(&sock, buffer_tftp ,msg_size,&flag.high_pass, (struct sockaddr_in *) &server, (struct sockaddr_in6 *) &server6,flag.ipv6_flag);
			read_tftp(&sock, buffer_tftp ,msg_size,&flag, (struct sockaddr_in *) &server, (struct sockaddr_in6 *) &server6);
		}
		else{
			int c;
			char buffer_data[flag.high_pass];
			ushort block_num = 1;
			FILE * file_w;
			long file_size;
			unsigned int last_size;
 			file_w = fopen(flag.path.c_str(),"rb");
 			c = sendto(sock,buffer_tftp,msg_size,0,(struct sockaddr *) &server, server_len);
 			
 			fseek (file_w , 0 , SEEK_END);
			file_size = ftell (file_w);
			rewind (file_w);

			wait_for_ack(&sock,buffer_tftp,&flag,(struct sockaddr_in *) &server, &server_len);
 			unsigned int counter;
 			last_size = file_size%flag.high_pass;
 			counter = file_size/flag.high_pass;
 			while(1){
 				char *p_write;
 				char buffer_sent[flag.high_pass+4];
 				*(short*)buffer_sent = htons(DATA);
 				p_write = buffer_sent + 2;
 				*(short*)p_write = htons(block_num);
 				p_write += 2;

 				std::cout << std::endl << "<<" << " Writing file..." << std::endl;
 				if(counter > 0){
		 			if (fread(buffer_data, flag.high_pass, 1,file_w ) == 1){
		 				strcpy(p_write,buffer_data);
		 				counter--;
		 			}
		 		}
	 			else{
	 				char *last_buffer;
	 				last_buffer = (char*)malloc(sizeof(char)*last_size+1);
	 				memset(last_buffer,0,last_size+1);
	 				if( fread(last_buffer,1,last_size,file_w ) == last_size){
	 					std::cout << last_buffer << std::endl;
	 					std::cout << last_size << std::endl;
	 					strcpy(p_write,last_buffer);
	 					free(last_buffer);
	 				}
	 				else{
	 					free(last_buffer);
	 					break;
	 				}
	 			}
	 			msg_size =  strlen(p_write) - strlen(buffer_sent);
	 			if(counter == 0)
	 				c = sendto(sock,buffer_sent,msg_size+4,0,(struct sockaddr *) &server, server_len);
	 			else
	 				c = sendto(sock,buffer_sent,msg_size+4,0,(struct sockaddr *) &server, server_len);
 				wait_for_ack(&sock,buffer_tftp,&flag,(struct sockaddr_in *) &server, &server_len);
 				block_num++;
			}
			wait_for_ack(&sock,buffer_tftp,&flag,(struct sockaddr_in *) &server, &server_len);
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
int read_tftp(int *sock, char* buffer_tftp, int msg_size,struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6){
	int c;
	socklen_t server_len;
	std::string filename = file_name(flag->path);
	FILE *file_r;
	file_r = fopen(filename.c_str(),"wb+");
	//Todo timer
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
			if( 0 == flag->mode.compare("ascii")){
				for(int i = 0 ; i < flag->high_pass ; i++){
					if( int(buffer_tftp[i]) == 13)
						buffer_tftp[i] = '\r';
					if(int(buffer_tftp[i]) == 10)
						buffer_tftp[i] = '\n';
				}
			}
			if( c == 512 )
				fwrite(buffer_tftp, 1, c , file_r);
			else
				fwrite(buffer_tftp, 1, c-4 , file_r);
			std::cout << std::endl;
			std::cout << "sizeof:" << c;
			//write(1, buffer_tftp+4, c-4); //only for debug
			*(short *)buffer_tftp = htons(ACK);
			if(! flag->ipv6_flag) sendto(*sock, buffer_tftp, 4, 0, (struct sockaddr *) server,server_len);
			else sendto(*sock, buffer_tftp, 4, 0, (struct sockaddr *) server6,server_len);
		}
	} while (c+4 == 516);

	fclose(file_r);
	return 0;
}
// Source: https://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path
std::string file_name(std::string const & path){
	return path.substr(path.find_last_of("/\\") + 1);
}

void wait_for_ack(int *sock, char* buffer_tftp,struct flags *flag,sockaddr_in *server,socklen_t *server_len){
	int c;
	for(int i;i<200;i++){ //2sc wait for end
		
		c = recvfrom(*sock,buffer_tftp,flag->high_pass,0,(struct sockaddr *) server, server_len);
		if( c > 0 && c < 4){
			std::cerr << "Wrong sent packet!"<<std::endl;
			exit(1);
		}
		else{
			if(ntohs(*(short *)buffer_tftp) == ACK){
				break;
			}
			if( ntohs(*(short *)buffer_tftp) == ERROR){
				std::cerr << "Error on server side" << std::endl;
				exit(1);
			}
		}
		usleep(10000);
	}
}