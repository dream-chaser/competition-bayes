#pragma once
#include <vector>
#include <string>
#include "label_info.h"
#include "fast_map.h"

class uid_info
{
public:
	uid_info();
	~uid_info();
	void add(std::string label, const char* geo, int time);
	void cal_label_prob();
	void find_top_3(const char* geo_s, int min_time, std::string& dst_1st, std::string& dst_2nd, std::string& dst_3rd);

	fast_map<std::string, label_info> labels;
};

