#
# Makefile
#
# Project:    ISA - TFTP Klient
# University: Brno University of Technology, FIT
# Date:       28.9. 2021
# Author:  Marek Sechra <xsechr00@stud.fit.vutbr.cz>
#


CC=g++
CFLAGS=-std=c++11

default: mytftpclient

mytftpclient: mytftpclient.o argparse.o
	$(CC) $(CFLAGS) -o mytftpclient mytftpclient.o argparse.o

mytftpclient.o: mytftpclient.cpp mytftpclient.h argparse.h
	$(CC) $(CFLAGS) -c mytftpclient.cpp

argparse.o:	argparse.cpp argparse.h
	$(CC) $(CFLAGS) -c argparse.cpp

clean:
	rm *.o mytftpclient

run: mytftpclient
	./mytftpclient

