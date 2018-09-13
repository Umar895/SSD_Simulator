#include "die.hh"

Die::Die(const int & planes, const int & blocks, int new_id){

	/* Set die id */
	id = new_id;

	free_planes = planes;

	//plane_ptrs.resize(planes);

	//for(int i=0; i<planes; i++){
	//	plane_ptrs[i] = NULL;
	//}

	pln_inuse = NULL;

	free_blks = blocks;

	erase_count=0;
}

int Die::get_id(){
	return id;
}

bool Die::set_time(long long time_zero){
	t_zero = time_zero;
	return true;
}

long long Die::get_time(){
	return t_zero;
}

bool Die::set_plnInuse(Plane* plane_id){
	pln_inuse = plane_id;
	return true;
}

Plane* Die::get_plnInuse(){
	return pln_inuse;
}

int Die::get_free_blks(){
	return free_blks;
}

bool Die::inc_erase_count(){
	erase_count++;
	return true;
}

int Die::get_erase_count(){
	return erase_count;
}

bool Die::reset_erase_count(){
	erase_count = 0;
	return true;
}
