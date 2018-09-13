#ifndef PAGE_HH
#define PAGE_HH

#include<iostream>
#include "block.hh"
#include "ssd.hh"

using namespace std;

class Page{

private:
	uint64_t id;
	int block_id;
	int plane_id;
	int die_id;
	int page_in_block;
	int page_writes;

	/* there will be three states of a page as follows:
		free = 0;
		write = 1;
		invalidate = -1;
	*/
	int stat;

	int page_written;

	uint64_t lba;

public:

	Page();

	//template <class id>
	Page(uint64_t new_id, int blk_id, int pln_id, int di_id);

	//template <class A>
	//bool set_id(uint64_t new_id, int blk_id, int pln_id, int di_id);

	uint64_t get_id();

	//bool set_block_id(long new_id);
	int get_block_id();
	int get_plane_id();
	int get_die_id();

	bool set_page_in_block(int seek);
	long get_page_in_block(){return page_in_block;}

	bool inc_page_writes();
	int get_page_writes();

	uint64_t get_lba();
	bool write_page(uint64_t lpn);
	bool invalidate_page();
	bool free_page();

	int get_page_stat();

	int get_page_written();
};

#endif
