#pragma once
#include "fast_map.h"
#include "geo_info.h"

class label_info
{
public:
	label_info();
	~label_info();
	void add(const char* geo, int time);
	double cal_prob(const char* geo_s, int min_time);

	fast_map<geo_info, int, geo_info::geo_inner_hash> geo_cnt;
	fast_map<int, int> time_cnt;
	int total_num;
	double p_label;
};

