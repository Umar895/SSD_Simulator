#include "ssd_config.hh"

SSD_Config::SSD_Config(std::string configFile){

	std::ifstream in(configFile);
	if(! in.is_open()){
		std::cerr<<"ERROR: Config file "<<configFile<<" cannot be opened "<<std::endl;
		exit(1);
	}

	std::string keyword;

	while(in >> keyword){

		if(keyword == "#"){
			std::getline(in,keyword);
			continue;
		}

		if(keyword == "logFolder"){
			in >> logFolder;
			
			if(logFolder.back() != '/'){
				logFolder = logFolder + "/";
			}
			continue;
		}

		if(keyword == "traceFile"){
			in >> traceFile;
			continue;
		}

		if(keyword == "printConfig"){
			in >> printConfig;
			continue;
		}

		if(keyword == "NUM_DIES"){
			in >> N_dies;
			continue;
		}

		if(keyword == "NUM_PLANES"){
			in >> N_planes;
			continue;
		}

		if(keyword == "NUM_BLOCKS"){
			in >> N_blocks;
			continue;	
		}

		if(keyword == "NUM_PAGES"){
			in >> N_pages;
			continue;
		}

		if(keyword == "OVER_PROV"){
			in >> O_prov;
			continue;
		}

		if(keyword == "CAPACITY"){
			in >> Capacity;
			continue;
		}

		if(keyword == "PAGE_WRITE_DELAY"){
			in >> w_delay;
			continue;
		}

		if(keyword == "PAGE_READ_DELAY"){
			in >> r_delay;
			continue;
		}

		if(keyword == "BLOCK_ERASE_DELAY"){
			in >> e_delay;
			continue;
		}

		if(keyword == "DELAY_CONTROLLER_TO_REGISTER"){
			in >> ctor_delay;
			continue;
		}

		if(keyword == "RANDOM_SEED"){
			in >> seed;
			continue;
		}

		std::getline(in,keyword);
	}

	Nlp = N_pages * N_blocks * N_planes * N_dies;

	//std::cout<<logFolder<<std::endl;
	mkdir(logFolder.c_str(),0777);		

	//init_SSD(configFile);
	if(printConfig){
		cout<<"*****************************"<<endl;
		cout<<"Using trace file:"<<traceFile<<endl;
		cout<<"Using log folder:"<<logFolder<<endl;
		cout<<"die per ssd:"<<N_dies<<endl;
		cout<<"planes per die:"<<N_planes<<endl;
		cout<<"blocks per plane:"<<N_blocks<<endl;
		cout<<"pages per block:"<<N_pages<<endl;
		cout<<"Over Provisioning:"<<O_prov<<"%"<<endl;
		cout<<"Spare Factor: "<<float(1-float(N_blocks)/(float(N_blocks * O_prov/100) + N_blocks))<<endl;
		cout<<"SSD Size: "<<(long long)(Nlp * 4096)/(long long)(1024*1024*1024)<<"GB"<<endl;
		cout<<"*****************************"<<endl;
		cout<<endl;

	}

	in.close();

	copyConfigFile(configFile, logFolder);

}

bool SSD_Config::checkConfig() const{

	bool check = true;

	if(logFolder == ""){
		cout<<"WARNING: logFolder was not set! "<<endl;
		check = false;
	}

	if(traceFile == ""){
		cout<<"WARNING: tracefile was not set! "<<endl;
		check = false;
	}

	if(N_dies == 0){
		cout<<"ERROR: Number of dies is not set or is set to 0"<<endl;
		check = false;
	}

	if(N_planes == 0){
		cout<<"ERROR: Number of planes is not set or equals to 0"<<endl;
		check = false;
	}

	if(N_blocks == 0){
		cout<<"ERROR: Number of blocks per plane is not set or equals to 0!"<<endl;
		check = false;	
	}

	if(N_pages == 0){
		cout<<"ERROR: Number of pages per block is not set or equals to 0"<<endl;
		check = false;
	}

	if(O_prov == 0){
		cout<<"ERROR: Over-provisioning is not set or equals to 0"<<endl;
		check = false;
	}

	if(Capacity == 0){
		cout<<"ERROR: capacity of the ssd is not set! or equals to 0 "<<endl;
		check = false;
	}

	if(w_delay == 0){
		cout<<"ERROR: write delay is not set!"<<endl;
		check = false;
	}

	if(r_delay == 0){
		cout<<"ERROR: read delay is not set!"<<endl;
		check = false;
	}

	if(e_delay == 0){
		cout<<"ERROR: erase delay is not set!"<<endl;
		check = false;
	}

	if(ctor_delay == 0){
		cout<<"ERROR: channel to register delay is not set! "<<endl;
		check = false;
	}

	if(seed == 0){
		cout<<"ERROR: Random Seed is not set! "<<endl;
		check = false;
	}


	if(!check){
		cout<<endl;
	}

	return check;
}

//SSD * SSD_Config::init_SSD(string configFile) const{

//	SSD *ssd = NULL;

//	ssd = new SSD();

//	return ssd;
//}


void SSD_Config::copyConfigFile(std::string configFile, std::string logFolder){

	std::ifstream src(configFile, std::ios::binary);

	std::ofstream dst(logFolder + "run.conf",std::ios::binary);

	dst << src.rdbuf();

	src.close();
	dst.close();

}

