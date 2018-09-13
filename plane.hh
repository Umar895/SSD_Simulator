#ifndef PLANE_HH
#define PLANE_HH

#include<iostream>
#include<vector>
#include<list>
#include<limits>

//#include "block.hh"
//#include "page.hh"
#include "ssd_config.hh"
#include "die.hh"

using namespace std;
class Block;

class Plane{

private:
	int id;
	int die_id;
	//int Nb;

	int free_blks;
	int written_blk;
	Block* blk_inuse;

	map<int,Block*> blk_ptrs;

	list<Block*> full_blk;

public:
	Plane(const int & Nb, int new_id, int di_id);

	bool set_id(int new_id);
	int get_id();

	bool set_dieId(int die_Id);
	int get_dieId();

	bool set_block(int b_id, Block* new_blk);

	bool reduce_free_blks();
	int get_free_blks();
	bool add_free_blks();

	bool set_blkInuse(Block* blk_inuse);
	Block* get_blkInuse();

	bool push_block(Block* temp_blk);

	int get_block_size();

	Block* get_full_blk();

	Block* get_victim_block(long long time);
};
#endif
