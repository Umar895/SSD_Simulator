#include "block.hh"

Block::Block(const int & Np, int new_id, int pln_id, int di_id){

	N_pages = Np;

	/* set block, plane, and die id */
	id = new_id;
	plane_id = pln_id;
	die_id = di_id;
 
	//page_ptrs.resize(Np);

	/* initially all pages in a block are invalid */
	for(int i=0; i<Np; i++){
		page_ptrs[i] = NULL;
	}

	/* There are only free pages directly after init */
	free_pgs = Np;
	valid_pgs = 0;
	dirty_pgs = 0;
	erase_cnt = 0;
	t_zero = 0;

	/* Start writing from the beginning of the block */
	seek = 0;

	/* The block has not yet been accessed */
	waccess = 0;
}

int Block::get_id(){
	return id;
}

int Block::get_plane_id(){
	return plane_id;
}

int Block::get_die_id(){
	return die_id;
}

int Block::get_free_pgs(){
	return free_pgs;
}

int Block::get_dirty_pgs(){
	return dirty_pgs;
}

int Block::get_valid_pgs(){
	return valid_pgs;
}

long Block::get_erase_cnt(){
	return erase_cnt;
}

Page* Block::get_page_at(int pos){
	
	return page_ptrs[pos];
}


bool Block::remove_page(Page* page, long long time){

	if(page->get_page_stat()==0 || page->get_page_stat()==-1){
		cout<<"page is not written or already invalidated, how it can be invalidated?"<<endl;
		cout<<"page status: "<<page->get_page_stat()<<endl;
		assert(1<0);
	}
	
	page->invalidate_page();
	valid_pgs--;
	dirty_pgs++;
	waccess = time;

	return true;
}

bool Block::erase_block(long long time){

	erase_cnt++;

	for(int i=0; i<N_pages; i++){
		page_ptrs[i] = NULL;
	}

	free_pgs = N_pages;
	valid_pgs = 0;
	dirty_pgs = 0;
	t_zero = time;

	seek = 0;

	waccess = 0;

	return true;
}

bool Block::write_page(Page* page, long long time){

	if(free_pgs == 0){
		return false;
	}

	page_ptrs[seek] = page;
	page->set_page_in_block(seek);

	free_pgs--;
	valid_pgs++;
	seek++;

	if(time!=-1){
		waccess = time;
	}

	//page->write_page();
	
	return true;
}


bool Block::write_xor(uint64_t lba, Page* page, int seek, long long time){

	if(free_pgs == 0){
		return false;
	}

	page_ptrs[seek] = page;
	page->set_page_in_block(seek);

	free_pgs--;
	valid_pgs++;

	if(time!=-1){
		waccess = time;
	}

	page->write_page(lba);
	
	return true;
}


bool Block::write_page(uint64_t lba, Page* page, long long time){

	if(free_pgs == 0){
		return false;
	}

	page_ptrs[seek] = page;

	page->set_page_in_block(seek);

	free_pgs--;
	valid_pgs++;
	seek++;

	if(time!=-1){
		waccess = time;
	}

	page->write_page(lba);
	
	return true;
}

