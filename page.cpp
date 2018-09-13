#include "page.hh"

Page::Page(){
	id = -1;
	block_id = -1;
	page_in_block = -1;
	page_writes = 0;
	stat = 0;
	lba = 0;
}

Page::Page(uint64_t new_id, int blk_id, int pln_id, int di_id){
	this->id = new_id;
	this->block_id = blk_id;
	this->plane_id = pln_id;
	this->die_id = di_id;

	page_in_block = -1;
	page_writes = 0;
	stat = 0;
	page_written = 0;
}

//bool Page::set_id(long new_id){
//	id = new_id;
//	return true;
//}

uint64_t Page::get_id(){
	return id;
}

//bool Page::set_block_id(long new_id){
//	block_id = new_id;
//	return true;
//}

int Page::get_block_id(){
	return block_id;
}

int Page::get_plane_id(){
	return plane_id;
}

int Page::get_die_id(){
	return die_id;
}

bool Page::inc_page_writes(){
	page_writes++;
	return true;
}

bool Page::set_page_in_block(int seek){
	page_in_block = seek;
	return true;
}

int Page::get_page_writes(){
	return page_writes;
}

uint64_t Page::get_lba(){
	return lba;
}

bool Page::write_page(uint64_t lpn){
	stat = 1;
	page_written++;
	lba = lpn;
	return true;
}

bool Page::invalidate_page(){
	stat = -1;
	return true;
}

bool Page::free_page(){
	stat = 0;
	lba = 0;
	return true;
}

int Page::get_page_stat(){
	return stat;
}

int Page::get_page_written(){
	return page_written;
}

