/* Project:    ISA - TFTP Klient
 University: Brno University of Technology, FIT
 Date:       14.11. 2021
 Author:  Marek Sechra <xsechr00@stud.fit.vutbr.cz>
*/

#include "mytftpclient.h"

int main( int argc, char* argv[]) {
	if(argc < 3){
		std::cerr << "Mising parametres!"<< std::endl << "Example of usage: " << argv[0] << " -R/W" << " -d /home/user/readme.txt" << std::endl;
		return 1;
	}
	else{
		std::cout << "<";
		std::cout << std::endl << "$ mytftpclient started 			";
		print_actual_time();

		flags flag = parsing(argc, argv);
		int sock, msg_size;
		struct sockaddr_in server;
		struct sockaddr_in6 server6;
		socklen_t server_len;
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
		struct timeval tv;
		tv.tv_sec = 3;  /* 30 Secs Timeout */
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

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
		char buffer_tftp[flag.high_pass+32],*p;
		int c;
		bool blcsize = false;
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
			blcsize = true;
			
			strcpy(p,flag.s_high_pass.c_str());
			p += flag.s_high_pass.length()+ 1;

		}
		if(flag.timeout != 0 ){
			strcpy(p,tmt.c_str());
			p += tmt.length()+ 1;

		}
		msg_size = p - buffer_tftp;

		if(flag.RW_flag ){
			std::cout << "$ start reading on " << flag.ip_adress << " port:" << flag.port;
			print_actual_time();
			read_tftp(&sock, buffer_tftp ,msg_size,&flag, (struct sockaddr_in *) &server, (struct sockaddr_in6 *) &server6);
			std::cout << "$ end";
			print_actual_time();
		}
		else{
			std::cout << "$ start writing";
			print_actual_time();
			int c;
			char buffer_data[1500];
			ushort block_num = 1;
			FILE * file_w;
			long file_size;
			unsigned int last_size;
			if( (0 == flag.mode.compare("netascii")) || (0 == flag.mode.compare("ascii")) )
 				file_w = fopen(flag.path.c_str(),"r");
 			else
 				file_w = fopen(flag.path.c_str(),"rb");

 			if(file_w == NULL){
				std::cerr << "File doesnt exist";
				exit(1);
			}

	 		c = my_sendto(&sock,buffer_tftp,msg_size,&flag,(struct sockaddr_in *) &server, (struct sockaddr_in6 *) &server6); //1.
 			fseek (file_w , 0 , SEEK_END);
			file_size = ftell(file_w);
			rewind (file_w);


			wait_for_ack(&sock,&flag,(struct sockaddr_in *) &server,(struct sockaddr_in6 *) &server6, &server_len);

 			unsigned int counter;
 			last_size = file_size%flag.high_pass;
			counter = file_size/flag.high_pass;
			while(1){
				std::cout << ".";
				char *p_write = NULL;
				char buffer_sent[flag.high_pass+4];
				memset(buffer_sent,0,flag.high_pass+4);
				*(short*)buffer_sent = htons(DATA);
				p_write = buffer_sent + 2;
				*(short*)p_write = htons(block_num);
				p_write += 2;

				
				if(counter > 0){
					memset(buffer_data,0,flag.high_pass);
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
						strcpy(p_write,last_buffer);
						free(last_buffer);
						std::cout << std::endl << "$ " << "Last packet" << std::endl;
					}
					else{
						free(last_buffer);
						break;
					}
				}
	 			msg_size =  strlen(p_write) - strlen(buffer_sent);
	 			c = my_sendto(&sock,buffer_sent,msg_size+4,&flag,(struct sockaddr_in *) &server, (struct sockaddr_in6 *) &server6);

 				wait_for_ack(&sock,&flag,(struct sockaddr_in *) &server,(struct sockaddr_in6 *) &server6, &server_len);
 				block_num++;
			}
			std::cout << std::endl;
			wait_for_ack(&sock,&flag,(struct sockaddr_in *) &server,(struct sockaddr_in6 *) &server6, &server_len);
			fclose(file_w);
			std::cout << "$ end";
			print_actual_time();
		}

		close(sock);
	}
	std::cout << ">";
	return 0;
}


/*Functions blok*/
int read_tftp(int *sock, char* buffer_tftp, int msg_size,struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6){
	int c;
	socklen_t server_len;
	std::string filename = file_name(flag->path);
	FILE *file_r;
	file_r = fopen(filename.c_str(),"wb+");
	if (file_r == NULL){ 
		std::cerr << "wrong opening file for read in server";
		exit(1);
	}
	my_sendto(sock,buffer_tftp,msg_size,flag,(struct sockaddr_in *) server, (struct sockaddr_in6 *) server6);
	
	if( flag->high_pass > 512 ){ // we expected OACK
			c = my_recvfrom(sock,buffer_tftp,(flag->high_pass+4),flag,(struct sockaddr_in *) server, (struct sockaddr_in6 *) server6);
			if ( ntohs(*(short *)buffer_tftp) != OACK){
				std::cerr << "Server dont sent 0ACK" << std::endl;
				exit(1);
			}
			else{//send ACK for 0ACK
				int msg;
				char *p_ack;
				char buffer_ack[100];
				memset(buffer_ack,0,100);
				*(short*)buffer_ack = htons(ACK);
				p_ack = buffer_ack + 2;
				p_ack += 3; //0
				msg =  p_ack - buffer_ack;
				my_sendto(sock,buffer_ack,msg,flag,(struct sockaddr_in *) server, (struct sockaddr_in6 *) server6);
			}
		}

	do {
		std::cout << ".";
		c = my_recvfrom(sock,buffer_tftp,(flag->high_pass+4),flag,(struct sockaddr_in *) server, (struct sockaddr_in6 *) server6);
		if ( ntohs(*(short *)buffer_tftp) == ACK) fprintf(stderr, "rcat: %s\n", buffer_tftp+4);
		else {
			bool flag_ascii = false;
			char tmp;
			if( (0 == flag->mode.compare("netascii")) || (0 == flag->mode.compare("ascii")) ) {
				for(int i = 4 ; i < c ; i++){
					if( int(buffer_tftp[i]) == 13 )
						flag_ascii = true;
					else{
						if(int(buffer_tftp[i]) == 10 && flag_ascii){//13 and 10
							flag_ascii = false;
							tmp = '\n';
							fwrite(&tmp, 1, 1 , file_r);
						}
						if(int(buffer_tftp[i]) == 0 && flag_ascii){//13 and 0
							flag_ascii = false;
							tmp = '\r';
							fwrite(&tmp, 1, 1 , file_r);
						}
						else{
							if( int(buffer_tftp[i]) != 10 && int(buffer_tftp[i]) != 0 ){
							flag_ascii = false;
							tmp = buffer_tftp[i];
							fwrite(&tmp, 1, 1 , file_r);
							}
						}
					}
				}
			}
			/* In for i iterate from 4 because before is header of TFTP
			*  and iam sending by byte to file raw ascii data
			*  1310 change to \n and 130 to \r
			*  others chars are simply wrote to file.
			*/
			else fwrite(buffer_tftp+4, 1, c-4 , file_r);

			*(short *)buffer_tftp = htons(ACK);
			my_sendto(sock,buffer_tftp,4,flag,(struct sockaddr_in *) server, (struct sockaddr_in6 *) server6);
		}
	} while (c == flag->high_pass+4);
	std::cout << std::endl;
	fclose(file_r);
	std::cout << "$ read ended succesfully";
	print_actual_time();
	return 0;
}

std::string file_name(std::string const & path){
	return path.substr(path.find_last_of("/\\") + 1);
}

void wait_for_ack(int *sock,struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6,socklen_t *server_len){
	char buffer_ack[100];
	memset(buffer_ack,0,100);
	int c;
		c = my_recvfrom(sock,buffer_ack,flag->high_pass,flag,(struct sockaddr_in *) server, (struct sockaddr_in6 *) server6);
		if( c > 0 && c < 4){
			std::cerr << "Wrong sent packet!"<<std::endl;
			exit(1);
		}
		else{
			if(ntohs(*(short *)buffer_ack) == ACK){
				;
			}
			else if(ntohs(*(short *)buffer_ack) == OACK){
				;
			}
			else if( ntohs(*(short *)buffer_ack) == ERROR){
				std::cerr << "Error on server side" << std::endl;
				exit(1);
			}
			else{
				std::cerr << "Wrong opcode" << std::endl;
				exit(1);
			}
		}
		//usleep(10000);
}

int my_sendto(int *sock, char* buffer, int msg, struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6 ){
	int c;
	socklen_t server_len;
	if(!flag->ipv6_flag){
		server_len = sizeof(*server);
		c = sendto(*sock,buffer,msg,0,(struct sockaddr *) server, server_len);
	}
	else{
		server_len = sizeof(*server6);
		c = sendto(*sock,buffer,msg,0,(struct sockaddr *) server6, server_len);
	}
	return c;
}

int my_recvfrom(int *sock, char* buffer, int size, struct flags *flag,sockaddr_in *server,sockaddr_in6 *server6 ){
	int c;
	socklen_t server_len;
	if(!flag->ipv6_flag){
		server_len = sizeof(*server);
		c = recvfrom(*sock,buffer,size,0,(struct sockaddr *) server, &server_len);
	}
	else{
		server_len = sizeof(*server6);
		c = recvfrom(*sock,buffer,size,0,(struct sockaddr *) server6, &server_len);
	}
	return c;
}

void print_actual_time(){
	time_t t; // t passed as argument in function time()
	struct tm * tt; // decalring variable for localtime()
	time (&t); //passing argument to time()
	tt = localtime(&t);
 	std::cout << "	"<< asctime(tt);
 	
}