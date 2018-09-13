#ifndef SSD_H
#define SSD_H

#include<iostream>
#include<vector>
#include<map>
#include<assert.h>
#include<list>
#include<bitset>
#include<functional>
#include<algorithm>

#include "ssd_config.hh"
#include "die.hh"
#include "plane.hh"
#include "block.hh"
#include "page.hh"

using namespace std;
class SSD_Config;
class Die;
class Page;

class SSD{

private:

	int N_pages=0;
	int N_blocks=0;
	int N_planes=0;
	int N_dies=0;

	int uw=0;
	int ur=0;
	int gcw=0;
	int iw=0;
	int xor_w=0;
	int reads_blocked=0;

	int t_di = 0;
	int t_pln = 0;
	int t_blk = 0;

	int read_missed=0;
	int read_bt_gc_flag_zero = 0;

	uint64_t t_page = 0;

	int op_blocks = 0;
	// For GC operation
	int High_water_mark = 0;
	int Low_water_mark = 0;

	bool invalid_page;

	Die *xor_die=NULL;

	// Data Structures for the corresponding SSD architecture
	list<Die*>ssd_die;
	map<Die*, list<Plane*> >die_plane;
	map<Plane*, list<Block*> >plane_blk;
	map<Block*, list<Page*> >block_page;

	struct iops{
		int write;
		int read;
		int blocked;
	};

	map<Die*, iops> gc_flag;
	list<Block*>tmp_blks;

public:


	struct reverse_ptr{
		Die *die;
		Plane *plane;
		Block *block;
		Page *page;
	};

	map<uint64_t, reverse_ptr>lpn_ppn;

	struct write_loc{
		Die *die;
		Plane *plane;
		Block *block;
		Page *page;
	};

	//template <class any>
	SSD(const SSD_Config &config);
	virtual ~SSD();			// Destructor to clean the Data Structure
	bool external_write(uint64_t lpn, int IOSize, double time);
	bool external_read(uint64_t lpn, int IOSize, double time);
	bool gc_flag_set(Die* di_id);
	bool update_gc_flag();
	void get_write_loc(write_loc *loc, Die* die_inv, Plane* pln_inv);

	int random_gen(int min, int max);
	bool xoring_block(Block* blk, double time);
	bool check_distance(int dis1, int dis2, int dis3);

	void garbage_collect(Die* die,Plane* plane, long long time);
	virtual bool internal_write(Die* die,uint64_t lpn, Plane* plane, long long time);
	void stat();
	void debug(Plane* plane);
};

#endif
