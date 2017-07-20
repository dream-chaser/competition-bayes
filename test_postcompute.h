#pragma once
#include <string>
#include <vector>
#include "uid_info.h"
#include "fast_map.h"
#define TIME_INTERVAL_PER_DAY 144
#define TOP_K 3

class test_postcompute
{
public:
	test_postcompute();
	~test_postcompute();
	void compute_print(FILE* outFile, uid_info* _train_data);
	void add(int oid, int t, std::string geo_s, int top_k, std::string* dst);

private:

	class start_loc_info {
	public:
		int oid, t;
		int top_k;
		std::string dst[TOP_K];
		start_loc_info(int oid, int t, int top_k, std::string* dst) {
			this->oid = oid;
			this->t = t;
			this->top_k = top_k;
			for (int i = 0; i < TOP_K; i++)
				this->dst[i] = std::string(dst[i]);
		}
	};

	uid_info* train_data;
	int* lb_top_k[TIME_INTERVAL_PER_DAY][TOP_K];
	double* time_p[TIME_INTERVAL_PER_DAY];

	int start_loc_sz;
	fast_map<std::string,int> test_start_loc;
	std::vector<std::vector<start_loc_info> > start_loc_infos;
	

	void compute_time_p();
	void compute_geo_p(geo_info start_geo, double* geo_p);
};

