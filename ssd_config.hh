#ifndef SSD_CONFIG_H
#define SSD_CONFIG_H

#include<iostream>
#include<fstream>
#include<string>

#include<sys/stat.h>
#include<sys/types.h>
//#include<fnctl>

#include<unistd.h>
#include "ssd.hh"

using namespace std;


class SSD_Config{

private:
	void copyConfigFile(std::string configFile, std::string logFolder);

public:

	uint64_t Nlp = 0;
	int N_dies = 0;			// Dies per SSD
	int N_planes = 0;		// Planes per Die or channel
	int N_blocks = 0;		// blocks per plane
	int N_pages = 0;		// pages per block
	int Capacity = 0;		// TODO:Not used yet

	double w_delay = 0;		// Write delay for a single page
	double r_delay = 0;		// Read delay for a single page
	double e_delay = 0;		// erase delay for a block

	double ctor_delay = 0;	// controller to register delay or vice versa for a single page
	double O_prov = 0;		// percentage of over-provisioing
	int seed = 0;

	//std::vector<std::string> ignoredKeywords;

	bool printConfig = true;

	std::string logFolder = "";

	std::string traceFile = "";

	SSD_Config(string configFile);

	//SSD * init_SSD(string configFile) const;

	virtual ~SSD_Config(){ }
	virtual bool checkConfig()const;
};

#endif
