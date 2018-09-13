#include "ssd.hh"

//template <class any>
SSD::SSD(const SSD_Config &config){

	Die *new_die;
	Plane *new_plane;
	Block *new_block;
	Page *new_page;

	N_dies = config.N_dies;
	N_planes = config.N_planes;
	N_blocks = config.N_blocks;
	N_pages = config.N_pages;
	
	op_blocks = (N_blocks * config.O_prov)/100;

	//write_delay = config.w_delay;
	//read_delay = config.r_delay;
	//erase_delay = config.e_delay;

	// For uniform distribution of op_blocks across the dies
	if(op_blocks%N_dies!=0){
		op_blocks = ((op_blocks + N_dies/2) / N_dies) * N_dies;
	}

	for(auto i=0; i<N_dies; i++){
		new_die = new Die(N_planes, N_blocks + op_blocks, t_di);
		ssd_die.push_back(new_die);


		for(auto j=0; j<N_planes; j++){
			new_plane = new Plane(N_blocks + op_blocks,t_pln, t_di);
			die_plane[new_die].push_back(new_plane);


			for(auto k=0; k<N_blocks+op_blocks; k++){
				new_block = new Block(N_pages,t_blk, t_pln, t_di);
				//new_plane->set_block(t_blk,new_block);
				plane_blk[new_plane].push_back(new_block);


				for(auto l=0; l<N_pages; l++){
					new_page = new Page(t_page, t_blk, t_pln, t_di);
					block_page[new_block].push_back(new_page);

					t_page++;
				}
				t_blk++;
			}
			t_pln++;
		}
		t_di++;
	}

	// A whole die is used for the parity of rest of dies
	xor_die = ssd_die.back();
	ssd_die.pop_back();
	//N_dies = N_dies-1;

	cout<<"die:"<<config.N_dies<<" plane:"<<config.N_planes<<" blk:"<<config.N_blocks<<" page:"<<config.N_pages<<endl;
	cout<<"die:"<<t_di<<" plane:"<<t_pln<<" blk:"<<t_blk<<" page:"<<t_page<<endl;

	if(t_di!=config.N_dies && 
		   t_pln!=config.N_dies * config.N_planes &&
		   t_blk!=config.N_dies * config.N_planes * (config.N_blocks + op_blocks)&&
		   t_page!=config.N_dies * config.N_planes * (config.N_blocks + op_blocks) * config.N_pages){

		cout<<" Cannot create the required configuration metrics!!"<<endl;
	}else{
		cout<<"Total pages, blocks, planes and dies created are as follows:"<<endl;
		cout<<"t_pages:"<<t_page<<" t_blks:"<<t_blk<<" t_pln:"<<t_pln<<" t_di:"<<t_di<<endl;

	}
	High_water_mark = N_blocks + op_blocks - 4;
	 Low_water_mark = High_water_mark  - op_blocks/4 ;

	//High_water_mark_xor = N_blocks + op_blocks - 16;

	 cout<<"high: "<<High_water_mark<<" low: "<<Low_water_mark<<endl;
}

/* Destructor for the SSD */
SSD::~SSD(){

	cout<<"destructor is called "<<endl;

	ssd_die.push_back(xor_die);

	Die *old_die;
	Plane *old_plane;
	Block *old_block;
	Page *old_page;

	for(auto i=0; i<N_dies; i++){
		old_die = ssd_die.front();

		//for(auto j=0; j<die_plane[old_die].size(); j++){
		for(auto j:die_plane){
			old_plane = die_plane[old_die].front();

			//for(auto k=0; k<plane_blk[old_plane].size(); k++){
			for(auto k:plane_blk){
				old_block = plane_blk[old_plane].front();

				//for(auto l=0; l<block_page[old_block].size(); l++){
				for(auto l:block_page){
					old_page = block_page[old_block].front();
					block_page[old_block].pop_front();
					delete old_page;
				}
				plane_blk[old_plane].pop_front();
				delete old_block;
			}
			die_plane[old_die].pop_front();
			delete old_plane;
		}
		ssd_die.pop_front();
		delete old_die;
	}
	

}


/* The writing strategy is as follows:
 * Die: For each write, die is selected in round robin fashion
 * Plane: For each request inside die, planes are also selected in round robin 
 * Block: The next inuse block in the plane is selected as the block, otherwise next free block of plane
 * ####################################################
 * For Example:
 * we have Die=2, plane=4
 *
 * Assume incoming request is as follows: (1,2,3,4,5,6,7,8,9,10) and are distributed as follows:
 * 
 * 1 - Die-0 => plane-0 => block-0 => page-0
 * 2 - Die-1 => plane-0 => block-0 => page-0
 * 3 - Die-0 => plane-1 => block-0 => page-0
 * 4 - Die-1 => plane-1 => block-0 => page-0
 * 5 - Die-0 => plane-2 => block-0 => page-0
 * 6 - Die-1 => plane-2 => block-0 => page-0
 * 7 - Die-0 => plane-3 => block-0 => page-0
 * 8 - Die-1 => plane-3 => block-0 => page-0
 * 9 - Die-0 => plane-0 => block-0 => page-1
 * 10- Die-1 => plane-0 => block-0 => page-1
 * ####################################################
 * 
 * But in a case when a page is invalidated, then same die is selected to overwrite the invalid page
 */


void SSD::get_write_loc(write_loc *loc, Die* die_inv, Plane* pln_inv){

	Die *next_die=NULL;
	Plane *next_plane=NULL;
	Block *next_block=NULL;
	Page* next_page=NULL;

	// Round Robin for selecting die to write, every die always has a free block available
	next_die = ssd_die.front();
	ssd_die.pop_front();
	ssd_die.push_back(next_die);
	assert(next_die!=0);


	// Initially the plane in use is NULL
	next_plane = die_plane[next_die].front();
	die_plane[next_die].pop_front();
	die_plane[next_die].push_back(next_plane);

	assert(next_plane!=0);

	if(next_plane->get_free_blks()==0){
		cout<<"no block available in plane: "<<next_plane->get_id()<<" die: "<<next_die->get_id()<<endl;
		assert(1<0);
	}

	// Initially no block is written, which gives null
	next_block = next_plane->get_blkInuse();
	if(next_block == NULL){
		next_block = plane_blk[next_plane].front();
	}

	assert(next_block!=0);

	if(next_block->get_free_pgs()==0){
		cout<<"There is no free pages on the block: "<<next_block->get_id()<<" plane:"<<next_plane->get_id()<<" die:"<<next_die->get_id()<<endl;
		assert(1<0);
	}

	next_plane->set_blkInuse(next_block);

	next_page = block_page[next_block].front();

	if(next_page->get_page_stat()!=0){
		cout<<"page is not free"<<endl;
		assert(1<0);
	}

	block_page[next_block].pop_front();
	block_page[next_block].push_back(next_page);

	assert(next_page!=0);

	loc->die = next_die;
	loc->plane = next_plane;
	loc->block = next_block;
	loc->page = next_page;


}

int SSD::random_gen(int min, int max){
	
	return min + rand() % (max - min);
}


bool SSD::external_read(uint64_t lpn, int IOSize, double time){

	Die *die_r=NULL;
/*
	int pos = random_gen(1,lpn_ppn.size());

	auto it = lpn_ppn.begin();
	advance(it,pos);
	lpn = it->first;

	die_r = lpn_ppn[lpn].die;

	if(gc_flag[die_r].write>0){
		gc_flag[die_r].blocked++;
	}else{
		read_bt_gc_flag_zero++;
	}
	ur++;
*/
	auto it = lpn_ppn.find(lpn);
	if(it!=lpn_ppn.end()){
		die_r = lpn_ppn[lpn].die;

		if(gc_flag[die_r].write>0){
			gc_flag[die_r].blocked++;
		}else{
			read_bt_gc_flag_zero++;
		}
	}

	ur++;

	if((uw + ur + gcw)%100000==0 && gcw>0){
		stat();
	}

	return true;
}
/*
void SSD::overwrite_parity(Block* blk){
	Block *blk_x;
	
	//map<int,Block*>::iterator it = block_m.find(blk);

	if(blk->get_die_id()==0){
		
	}

	int offset = block_m.size()/(N_dies*N_planes);

	N_dies - blk->get_die_id();

	blk_x = block_m[blk->get_id() + offset];

}
*/

bool SSD::external_write(uint64_t lpn, int IOSize, double time){


	Die *die_i=NULL;
	Plane *pln_i = NULL;
	Block *blk_i = NULL;
	Page *page_i = NULL;

	Plane *plane_w;
	Block *blk_w;
	Page *page_w;

	Block *temp_blk = NULL;

	write_loc loc;

	invalid_page = false;

	auto it = lpn_ppn.find(lpn);
	if(it!=lpn_ppn.end()){
		die_i = lpn_ppn[lpn].die;
		pln_i = lpn_ppn[lpn].plane;
		blk_i = lpn_ppn[lpn].block;
		page_i = lpn_ppn[lpn].page;

		lpn_ppn.erase(lpn);
		invalid_page = true;
		iw++;

		blk_i->remove_page(page_i, time);

		//overwrite_parity(blk_i);
	}

	get_write_loc(&loc, die_i, pln_i);

	update_gc_flag();

	if(time==t_page){uw=gcw;}

	page_w = loc.page;
	blk_w = loc.block;
	plane_w = loc.plane;

	blk_w->write_page(lpn,page_w, time);
	
	uw++;

	lpn_ppn[lpn].die = loc.die;
	lpn_ppn[lpn].plane = plane_w;
	lpn_ppn[lpn].block = blk_w;
	lpn_ppn[lpn].page = page_w;


	//cout<<"free pages in the current block "<<write_blk->get_free_pgs()<<endl;
	if(blk_w->get_free_pgs()==0){
		//cout<<"time to push the block into filled block"<<endl;
		temp_blk = plane_blk[plane_w].front();
		plane_w->push_block(temp_blk);

		plane_blk[plane_w].pop_front();
		plane_w->reduce_free_blks();

		xoring_block(blk_w, time);

		plane_w->set_blkInuse(plane_blk[plane_w].front());

		
	}

	int inf_gc = 0;
	if(plane_w->get_block_size()>=High_water_mark){


		while(plane_w->get_block_size() >= Low_water_mark){
			garbage_collect(loc.die,plane_w,time);
			if(inf_gc>1){break;}
			inf_gc++;
			//if(uw==1363585){
/*			if(gcw>4244430){
				cout<<"gcw: "<<gcw<<" blks_in_a_pln: "<<plane_w->get_block_size()<<endl;
				if(gcw>4246430){assert(1<0);}
			}
*/			//if(plane_w->get_block_size()==990){	assert(1<0);}

		}
		gc_flag_set(loc.die);
	}

	if((uw + ur + gcw)%100000==0 && gcw>0){
		stat();
	}

	return true;
}

bool SSD::gc_flag_set(Die* di_id){

	gc_flag[di_id].write = 8;
	gc_flag[di_id].read = 60;

	return true;
}

bool SSD::update_gc_flag(){

	for (map<Die*,iops>::iterator it=gc_flag.begin(); it!=gc_flag.end(); ++it){
		if(it->second.write>0){
			it->second.write--; 
			it->second.read = it->second.read - 8;
		}
	}

	

	return true;
}


bool SSD::check_distance(int dis1, int dis2, int dis3){

	if(dis3-dis2 == dis2-dis1){
		//cout<<"equi distant, xor possible"<<endl;

		return true;
	}else{

		cout<<"xor not possible"<<endl;
		cout<<"ids 1:"<<dis1<<" 2:"<<dis2<<" 3:"<<dis3<<endl;
		return false;
	}
}


// Xor of blocks
bool SSD::xoring_block(Block* blk, double time){

	Plane *x_plane;

	Block *first, *second, *third, *x_blk;
	Page *page, *page1, *page2, *x_page;
	int dis1, dis2, dis3;
	int xo;

//	if(blks.size()!=0){
		
//		assert(1<0);
		tmp_blks.push_back(blk);
		if(tmp_blks.size()==3){
			
			x_plane=die_plane[xor_die].front();

			first = tmp_blks.front();
			dis1 = first->get_id();
			tmp_blks.pop_front();
	
			second = tmp_blks.front();
			dis2 = second->get_id();
			tmp_blks.pop_front();

			third = tmp_blks.front();
			dis3 = third->get_id();
			tmp_blks.pop_front();

			if(!check_distance(dis1,dis2,dis3)){
				cout<<"the blocks are unaligned for xor"<<endl;
				assert(1<0);
			}
			x_blk = plane_blk[x_plane].front();

			//cout<<"dis1:"<<dis1<<" dis2:"<<dis2<<" dis3:"<<dis3<<" dis4:"<<dis3+offset<<endl;
			//assert(1<0);

			for(int i=0; i<N_pages; i++){
				page = first->get_page_at(i);
				page1 = second->get_page_at(i);
				page2 = third->get_page_at(i);

				uint64_t flags[] = {(page->get_lba()),page1->get_lba(),page2->get_lba()};

				xo = accumulate(flags,end(flags),0,bit_xor<int>());
				
				x_page = block_page[x_blk].front();

				if(x_page->get_page_stat()!=0){
					cout<<"page is not free status:"<<x_page->get_page_stat()<<endl;
					cout<<"xblk_id:"<<x_blk->get_die_id()<<endl;
					assert(1<0);
				}

				block_page[x_blk].pop_front();
				block_page[x_blk].push_back(x_page);

				x_blk->write_xor(xo, x_page, i, time);
				xor_w++;
			}

			if(x_plane->get_id()!=x_blk->get_plane_id()){
				cout<<"the planes conflict after the xor"<<endl;
				assert(1<0);
			}

			if(plane_blk[x_plane].front()!=x_blk){
				cout<<"serious problem!!"<<endl;
				assert(1<0);
			}

			plane_blk[x_plane].pop_front();
			x_plane->push_block(x_blk);
			x_plane->reduce_free_blks();
			
			if(x_plane->get_block_size()>=High_water_mark){
				garbage_collect(xor_die,x_plane, time);

			}
			die_plane[xor_die].pop_front();
			die_plane[xor_die].push_back(x_plane);

			tmp_blks.clear();
		}
//	}


	return true;
}

bool SSD::internal_write(Die* die, uint64_t lpn, Plane* plane, long long time){

	Block *next_block=NULL;
	Page *next_page=NULL;

	Block *temp_blk;

	assert(lpn!=0);

	plane = die_plane[die].front();
	die_plane[die].pop_front();
	die_plane[die].push_back(plane);

	// Initially no block is written, which gives null
	next_block = plane->get_blkInuse();
	if(next_block == NULL){
		next_block = plane_blk[plane].front();
	}

	assert(next_block!=0);
	if(next_block->get_free_pgs()==0){
		cout<<"There is no free pages on the block: "<<next_block->get_id()<<" plane:"<<next_block->get_plane_id()<<" die:"<<next_block->get_die_id()<<endl;
		assert(1<0);
	}

	next_page = block_page[next_block].front();

	block_page[next_block].pop_front();
	block_page[next_block].push_back(next_page);
	//next_page = block_page[next_block].front();
/*	if(gcw>4244430){
		cout<<"before write"<<endl;
		cout<<"plane blk size: "<<plane_blk[plane].size()<<endl;
		cout<<"pln: "<<plane<<endl;
		for (auto it = plane_blk.begin(); it != plane_blk.end(); it++)
		{
			cout << it->first << ":";
			for (auto lit = it->second.begin(); lit  != it->second.end(); lit++)
		        cout << " " << (*lit)->get_free_pgs();
				cout << "\n";
		}
	}
*/
	next_block->write_page(lpn,next_page, time);
	//next_block->write_page(next_page,time);
	
	if(next_block->get_free_pgs()==0){
		//cout<<"time to push the block into filled block"<<endl;
		temp_blk = plane_blk[plane].front();
		plane->push_block(temp_blk);

		plane_blk[plane].pop_front();
		plane->reduce_free_blks();

		plane->set_blkInuse(plane_blk[plane].front());
/*		if(gcw>4244430){
			cout<<"after write"<<endl;
			cout<<"plane blk size: "<<plane_blk[plane].size()<<endl;
			cout<<"pln: "<<plane<<endl;
			for (auto it = plane_blk.begin(); it != plane_blk.end(); it++)
			{
				cout << it->first << ":";
				for (auto lit = it->second.begin(); lit  != it->second.end(); lit++)
			        cout << " " << (*lit)->get_free_pgs();
					cout << "\n";
			}
		}
*/	}

	lpn_ppn[lpn].die = die;
	lpn_ppn[lpn].plane = plane;
	lpn_ppn[lpn].block = next_block;
	lpn_ppn[lpn].page = next_page;

	return true;
}

void SSD::garbage_collect(Die* die, Plane *plane, long long time){

	Block *victim_block;
	Page* reset_page;

	list<Page*>::iterator it;

	victim_block = plane->get_victim_block(time);
/*	if(victim_block->get_dirty_pgs()==0){
		cout<<"victim blk does not have a dirty page "<<endl;
		debug(plane);
		assert(1<0);
	}
*/
	for(it = block_page[victim_block].begin(); it!=block_page[victim_block].end(); it++){
		reset_page = *it;

		if(reset_page->get_page_stat() == 1){
			// shift the write to active block please
			if(reset_page->get_lba()!=0){
				//internal_write(die,reset_page->get_lba(),plane, time);
				lpn_ppn.erase(reset_page->get_lba());
			}
			gcw++;
		}
		reset_page->free_page();
	}

	victim_block->erase_block(time);
	plane_blk[plane].push_back(victim_block);
	plane->add_free_blks();
	die->inc_erase_count();
}


void SSD::stat(){
	
	cout<<"mp_t:"<<lpn_ppn.size();
	
	cout<<" uw:"<<uw<<" ur(%):"<<100*double(ur)/double(uw+ur)<<" wa:"<<double(uw+xor_w+gcw)/double(uw);
	cout<<"| dies";
	for (map<Die*,iops>::iterator it=gc_flag.begin(); it!=gc_flag.end(); ++it){
			cout<<"|"<<it->second.blocked<<"/"<<(it->first)->get_erase_count();
			//cout<<"|"<<double((it->first)->get_erase_count())/double(it->second.blocked);
			(it->first)->reset_erase_count();
			gc_flag[it->first].blocked=0;
			
	}

	//cout<<" r_m:"<<read_missed<<" : "<<read_bt_gc_flag_zero;
	cout<<'\n';
}


void SSD::debug(Plane* plane){

	Block* victim_block;
	int dirty=0;
	int valid=0;

	int ful_blks = plane->get_block_size();

	for(int i=0; i<ful_blks; i++){
		victim_block = plane->get_victim_block(1);
		if(victim_block->get_dirty_pgs() == 0){
			dirty++;
		}
		if(victim_block->get_valid_pgs()==256){
			valid++;
		}
	}

	cout<<"ful_blk: "<<ful_blks<<" dirty: "<<dirty<<" valid: "<<valid<<endl;
}
