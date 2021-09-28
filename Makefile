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

all: mytftpclient

mytftpclient: mytftpclient.o
mytftpclient.o:mytftpclient.cpp

clean:
	rm *.o mytftpclient

run: mytftpclient
	./mytftpclient

