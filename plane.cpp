#include "plane.hh"

Plane::Plane(const int & Nb, int new_id, int di_id){
	
	id = new_id;
	die_id = di_id;

	free_blks = Nb;
	written_blk = 0;

	for(int i=0; i<Nb; i++){
		blk_ptrs[i] = NULL;
	}

	blk_inuse = NULL;
	
}

bool Plane::set_id(int new_id){
	id = new_id;
	return true;
}

int Plane::get_id(){
	return id;
}

bool Plane::set_dieId(int die_Id){
	die_id = die_Id;
	return true;
}

int Plane::get_dieId(){
	return die_id;
}

bool Plane::set_block(int b_id, Block* new_blk){
	blk_ptrs[b_id] = new_blk;
	return true;
}

bool Plane::reduce_free_blks(){
	free_blks--;
	written_blk++;
	return true;
}

int Plane::get_free_blks(){
	return free_blks;
}

bool Plane::add_free_blks(){
	free_blks++;
	written_blk--;
	return true;
}

bool Plane::set_blkInuse(Block* bl_inuse){
	blk_inuse = bl_inuse;
	return true;
}

Block* Plane::get_blkInuse(){
	return blk_inuse;
}

// Returns the block sequentially for xor operations
Block* Plane::get_full_blk(){
	Block *next_blk;
	next_blk = full_blk.front();
	full_blk.pop_front();
	full_blk.push_back(next_blk);
	return next_blk;
}

bool Plane::push_block(Block* temp_blk){
	full_blk.push_back(temp_blk);
	return true;
}

int Plane::get_block_size(){

	return full_blk.size();

}

Block* Plane::get_victim_block(long long time){

	Block* victim_block;
	Block* block;

	int min_valid = numeric_limits<int>::max();
	list<Block*>::iterator it, it_erase;

	//cout<<"size blks: "<<full_blk.size()<<endl;

    for (it=full_blk.begin();it!=full_blk.end();it++){
        block = *it;

		//cout<<"valid_pgs "<<block->get_valid_pgs()<<endl;

		if(block->get_valid_pgs() < min_valid){
			min_valid = block->get_valid_pgs();
			victim_block = *it;
			it_erase = it;
		}
		
	}
	//cout<<"victim valid_pgs "<<victim_block->get_valid_pgs()<<endl;
	full_blk.erase(it_erase);
	//cout<<"size blks: "<<full_blk.size()<<endl;

	return victim_block;
	
}



