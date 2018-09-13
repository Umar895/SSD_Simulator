#include<iostream>
#include<string>
#include<fstream>
#include<random>
#include<assert.h>
#include<ctime>

#include "ssd_config.hh"
//#include "tracegen/trace.hh"
#include "ssd.hh"

using namespace std;

SSD_Config * config = NULL;
SSD *ssd = NULL;

int random_gen(int min, int max){

	static bool first = true;

	if(first){
		srand(42);
		first = false;
	}
	return min + rand() % (max - min);
}

int main(int argc, char ** argv){

	if(argc != 2){
		cout<<"\033[1;31mCorrect call <config file> \033[0m "<<endl;
		exit(-1);
	}

	std::string configFile = argv[1];

	long long inc_time = 1;
	config = new SSD_Config(configFile);

	ssd = new SSD(*config);

	//ssd->init_SSD(config);
	bool validConfig = config->checkConfig();
	if(!validConfig){
		cout<<"\t################################"<<endl;
		cout<<"\tWARNING: config file is not valid !!"<<endl;
		cout<<"\t################################"<<endl;
		cout<<endl;
	}

	std::string FileName = config->traceFile;

	std::mt19937 gen(config->seed);
	std::uniform_int_distribution<>dis(0,config->Nlp-1);

	time_t start_time = time(0);
	// Sequential Writes
	cout<<"Sequential Writes "<<endl;
	for(int i=0; i<1; i++){
		for(uint64_t z=0; z<config->Nlp; z++){
			ssd->external_write(z,1,inc_time);
			inc_time++;
		}
	}

	int req_size = 0;
	uint64_t req = 0;
	int r_w;
	// Random Writes
	cout<<"Random Writes "<<endl;
	for(int j=0; j<2; j++){
		for(uint64_t i=0; i<config->Nlp; i++){
			//break;
			//cout<<"rand:"<<random_gen(0, config->Nlp)<<endl;
			req_size = random_gen(1,2);
			r_w = random_gen(0,20);
			req = random_gen(1, config->Nlp);

			for(int x=0; x < req_size; x++){
				
					ssd->external_write(req+x, req_size , inc_time);

					inc_time++;
			}

			if(r_w%3==0){
				for(int y=0; y<req_size; y++){
					ssd->external_read(req+y, req_size, inc_time);
					inc_time++;
				}
			}
		}
	}

	time_t end_time = time(0);
	uint64_t duration = (uint64_t)difftime(end_time, start_time);
	cout<<"Total Simulation time: "<<duration/3600<<":"<<(duration%3600)/60<<":"<<((duration%3600)%60)<<endl;

	delete config;
	delete ssd;

	return 0;
}
