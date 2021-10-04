#include <iostream>
#include <unistd.h>
#include "mytftpclient.h"


int main( int argc, char* argv[]) {
	if(argc < 3){
		std::cerr << "Mising parametres!"<< std::endl << "Example of usage: " << argv[0] << " -R/W" << " -d /home/user/readme.txt" << std::endl;
		/*nemusí byt implicitne adresa a port uvazujese automaticky 
		o 127.0.0.1:69*/
		return 1;
	}
	else{
		int arg;
		while( (arg = getopt(argc,argv,"RWdtsmca:")) != -1){
			switch(arg){
				case ('W'):
					break;
				case ('R'):
					break;
				case 'd':
					break;
				case 't':
					break;
				case 's':
					break;
				case 'm':
					break;
				case 'c':
					break;
				case 'a':
					break;
				default:
					std::cout<<arg;
					std::cerr<<"Example of usage: " << argv[0] << " -R/W" << " -d /home/user/readme.txt" << std::endl;
			}
		}
	}
	return 0;
}
