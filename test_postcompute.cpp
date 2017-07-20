#include "test_postcompute.h"
#include "geo_info.h"
#include "Constant.h"
#include <time.h>

const double PI = (4 * atan(1));
const double geo_sigma = GEO_SIGMA;
const double time_sigma = TIME_SIGMA;
const double geo_exp_coeff = -0.5 / (geo_sigma*geo_sigma);
const double  geo_coeff = 1 / (sqrt(2 * PI)*geo_sigma);
const double  time_exp_coeff = -0.5 / (time_sigma*time_sigma);
const double  time_coeff = TIME_COEFF / (sqrt(2 * PI)*time_sigma);

//#define PRINT_PROBA
#ifdef PRINT_PROBA
FILE* TMP_FILE;
#endif

test_postcompute::test_postcompute()
{
	for (int i = 0; i < TIME_INTERVAL_PER_DAY; i++)
		time_p[i] = NULL;
	
	for (int i = 0; i < TIME_INTERVAL_PER_DAY; i++)
		for (int j = 0; j < TOP_K; j++)
			lb_top_k[i][j] = NULL;
	train_data = NULL;
	start_loc_sz = 0;
#ifdef PRINT_PROBA
	TMP_FILE = fopen("tmp", "w");
#endif
}

test_postcompute::~test_postcompute()
{
	for (int i = 0; i < TIME_INTERVAL_PER_DAY; i++)
		if (time_p[i])
			delete[] time_p[i];

	for (int i = 0; i < TIME_INTERVAL_PER_DAY; i++)
		for (int j = 0; j < TOP_K; j++)
			if (lb_top_k[i][j])
				delete[] lb_top_k[i][j];
#ifdef PRINT_PROBA
	fclose(TMP_FILE);
#endif
}

void test_postcompute::compute_time_p()
{
	printf("Computing the conditional probability of time ...\n");
	int lsz = train_data->labels.size();
	for (int i = 0; i < TIME_INTERVAL_PER_DAY; i++)
	{
		time_p[i] = new double[lsz];
		for (int j = 0; j < lsz; j++)
		{
			time_p[i][j] = 0.0;
			int tsz = train_data->labels.getValue(j).time_cnt.size();
			for (int k = 0; k < tsz; k++)
			{
				int t = train_data->labels.getValue(j).time_cnt.getKey(k);
				int t_dist = abs(i - t);
				t_dist = t_dist >(TIME_INTERVAL_PER_DAY >> 1) ? t_dist - TIME_INTERVAL_PER_DAY : t_dist;
				time_p[i][j] += train_data->labels.getValue(j).time_cnt.getValue(k) * time_coeff * exp(time_exp_coeff * t_dist * t_dist);
			}
			time_p[i][j] /= train_data->labels.getValue(j).total_num;
		}
	}
}

void test_postcompute::add(int oid, int t, std::string geo_s, int top_k, std::string* dst)
{
	start_loc_info new_info(oid, t, top_k, dst);
	int start_loc_index = -1;
	if (test_start_loc.count(geo_s) <= 0)
	{
		test_start_loc.add(geo_s, start_loc_sz);
		std::vector<start_loc_info> new_start_loc;
		start_loc_infos.push_back(new_start_loc);
		start_loc_index = start_loc_sz;
		start_loc_sz++;
	}
	else
		start_loc_index = test_start_loc[geo_s];
	start_loc_infos[start_loc_index].push_back(new_info);
}

void test_postcompute::compute_geo_p(geo_info geo, double* geo_p)
{
	int lsz = train_data->labels.size();
	for (int k = 0; k < lsz; k++)
	{
		double p_geo_lat = 0, p_geo_lon = 0;
		for (int i = 0; i < train_data->labels.getValue(k).geo_cnt.size(); i++)
		{
			double lat = train_data->labels.getValue(k).geo_cnt.getKey(i).lat;
			double lon = train_data->labels.getValue(k).geo_cnt.getKey(i).lon;
			p_geo_lat += train_data->labels.getValue(k).geo_cnt.getValue(i) * geo_coeff * exp(geo_exp_coeff * (geo.lat - lat) * (geo.lat - lat));
			p_geo_lon += train_data->labels.getValue(k).geo_cnt.getValue(i) * geo_coeff * exp(geo_exp_coeff * (geo.lon - lon) * (geo.lon - lon));
		}
		p_geo_lat /= train_data->labels.getValue(k).total_num;
		p_geo_lon /= train_data->labels.getValue(k).total_num;
		geo_p[k] = p_geo_lat * p_geo_lon;
		//fprintf(TMP_FILE, "%e %e %e\n", p_geo_lat, p_geo_lon, geo_p[k]);
	}
}

void test_postcompute::compute_print(FILE* outFile, uid_info* _train_data)
{
	train_data = _train_data;
	compute_time_p();
	for (int i = 0; i < TIME_INTERVAL_PER_DAY; i++)
	for (int j = 0; j < TOP_K; j++)
	{
		lb_top_k[i][j] = new int[start_loc_sz];
	}
	int lsz = train_data->labels.size();
	clock_t start = 0, cur = 0;
	start = clock();
	for (int k = 0; k < start_loc_sz; k++)
	{
		if (k % 100 == 0)
		{
			cur = clock();
			double percent = (double)k / start_loc_sz;
			double use_time = ((double)(cur - start) / CLOCKS_PER_SEC) / 3600;
			printf("%.2f%% %d/%d Use Time:%.2fh Left Time:%.2fh\n", 100 * percent, k, start_loc_sz, use_time, use_time*(1 - percent) / percent);
		}
		geo_info start_geo(test_start_loc.getKey(k).c_str());
		double* geo_p = new double[lsz];
		compute_geo_p(start_geo, geo_p);
		for (unsigned int i = 0; i < start_loc_infos[k].size(); i++)
		{
			int lb[3] = { -1, -1, -1 };
			double pmax[3] = { -1, -1, -1 };
			std::string out_dst[3] = { "", "", "" };
			for (int j = 0; j < lsz; j++)
			{
				double p = geo_p[j] * time_p[start_loc_infos[k][i].t][j] * train_data->labels.getValue(j).p_label;
#ifdef PRINT_PROBA
				fprintf(TMP_FILE, "%s: %e %e %e %e\n", train_data->labels.getKey(j).c_str(), geo_p[j], time_p[start_loc_infos[k][i].t][j], train_data->labels.getValue(j).p_label, p);
#endif
				if (p > pmax[0])
				{
					pmax[2] = pmax[1]; lb[2] = lb[1];
					pmax[1] = pmax[0]; lb[1] = lb[0];
					pmax[0] = p;       lb[0] = j;
				}
				else if (p > pmax[1])
				{
					pmax[2] = pmax[1]; lb[2] = lb[1];
					pmax[1] = p;       lb[1] = j;
				}
				else if (p > pmax[2])
				{
					pmax[2] = p; lb[2] = j;
				}
			}
			for (int j = 0; j < TOP_K - start_loc_infos[k][i].top_k; j++)
				out_dst[j] = start_loc_infos[k][i].dst[j];
			int dst_index = TOP_K - start_loc_infos[k][i].top_k;
			for (int j = 0; j < TOP_K; j++)
			{
				if (lb[j] != -1)
				{
					std::string tmp_str = train_data->labels.getKey(lb[j]);
					bool repeated = false;
					for (int s = 0; s < TOP_K - start_loc_infos[k][i].top_k; s++)
					{
						if (tmp_str == out_dst[s])
						{
							repeated = true;
							break;
						}
					}
					if (!repeated)
					{
						out_dst[dst_index] = tmp_str;
						dst_index++;
						if (dst_index == TOP_K) break;
					}
				}
				else
					printf("error!\n");
			}
			fprintf(outFile, "%d,%s,%s,%s\n", start_loc_infos[k][i].oid, out_dst[0].c_str(), out_dst[1].c_str(), out_dst[2].c_str());
		}
		delete[] geo_p;
	}
}