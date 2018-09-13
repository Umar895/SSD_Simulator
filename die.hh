#ifndef DIE_HH
#define DIE_HH

#include<iostream>

//#include "plane.hh"
//#include "block.hh"
#include "ssd_config.hh"
#include "ssd.hh"

using namespace std;
class Plane;

class Die{

private:
	long id;
	long long t_zero;
	int free_blks;
	int free_planes;
	int erase_count;

	Plane* pln_inuse;

	//vector<Plane*> plane_ptrs;

public:

	Die(const int & planes, const int & blocks, int new_id);
	virtual ~Die(){};

	int get_id();

	bool set_plnInuse(Plane* plane_id);
	Plane* get_plnInuse();

	int get_free_blks();

	bool set_time(long long time_zero);
	long long get_time();

	bool inc_erase_count();
	int get_erase_count();
	bool reset_erase_count();
};

#endif
