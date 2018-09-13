#ifndef BLOCK_H
#define BLOCK_H

#include<map>
#include<iostream>

//#include "page.hh"
//#include "ssd.hh"
#include "ssd_config.hh"
#include "plane.hh"

class SSD_Config;
class Page;

class Block{

private:
	int id;
	int plane_id;
	int die_id;

	int valid_pgs;
	int dirty_pgs;
	int free_pgs;

	long erase_cnt;

	int seek;

	long waccess;

	long t_zero;

	int N_pages;

	map<int,Page*> page_ptrs;


public:


	Block(const int & Np, int new_id, int pln_id, int di_id);
	virtual ~Block(){};

	virtual bool erase_block(long long time);

	bool write_page(Page *page, long long time);
	bool write_page(uint64_t lba, Page *page, long long time);
	bool write_xor(uint64_t lba, Page *page, int seek, long long time);

	virtual bool remove_page(Page *page, long long time);

	//bool set_id(long new_id);

	int get_id();
	int get_plane_id();
	int get_die_id();

	int get_free_pgs();
	int get_dirty_pgs();
	int get_valid_pgs();

	long long get_waccess();
	//bool set_t_zero();

	Page* get_page_at(int pos);
	long get_erase_cnt();

};
#endif
