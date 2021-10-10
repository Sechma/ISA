#ifndef ARGPARSE_H_
#define ARGPARSE_H_


#include "mytftpclient.h"


struct flags{
			bool R_flag=false;
			bool multi_flag=false;
			int counter_RW=0;
			std::string path;
			int timeout = 0;
			int high_pass = 0;
			std::string adress;
};


flags parsing(int argc2, char* argv2[]);


#endif