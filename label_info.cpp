#include "label_info.h"
#include "Constant.h"

const double PI = (4 * atan(1));
const double geo_sigma = GEO_SIGMA;
const double time_sigma = TIME_SIGMA;
const double geo_exp_coeff = -0.5 / (geo_sigma*geo_sigma);
const double  geo_coeff = 1 / (sqrt(2 * PI)*geo_sigma);
const double  time_exp_coeff = -0.5 / (time_sigma*time_sigma);
const double  time_coeff = TIME_COEFF / (sqrt(2 * PI)*time_sigma);

label_info::label_info()
{
}


label_info::~label_info()
{
}

double label_info::cal_prob(const char* geo_s, int min_time)
{
	geo_info geo(geo_s);
	double p_geo_lat = 0, p_geo_lon = 0, p_time = 0;
	for (int i = 0; i < geo_cnt.size(); i++)
	{
		double lat = geo_cnt.getKey(i).lat;
		double lon = geo_cnt.getKey(i).lon;
		p_geo_lat += geo_cnt.getValue(i) * geo_coeff * exp(geo_exp_coeff * (geo.lat - lat) * (geo.lat - lat));
		p_geo_lon += geo_cnt.getValue(i) * geo_coeff * exp(geo_exp_coeff * (geo.lon - lon) * (geo.lon - lon));
	}
	p_geo_lat /= total_num;
	p_geo_lon /= total_num;
	for (int i = 0; i < time_cnt.size(); i++)
	{
		int t = time_cnt.getKey(i);
		int t_dist = abs(min_time - t);
		t_dist = t_dist > 72 ? t_dist - 144 : t_dist;
		p_time += time_cnt.getValue(i) * time_coeff * exp(time_exp_coeff * t_dist * t_dist);
	}
	p_time /= total_num;
	return (p_geo_lat * p_geo_lon * p_time * p_label);
}

void label_info::add(const char* geo, int t)
{
	geo_info new_geo(geo);
	if (geo_cnt.count(new_geo) > 0)
		geo_cnt[new_geo]++;
	else
		geo_cnt.add(new_geo, 1);

	if (time_cnt.count(t) > 0)
		time_cnt[t]++;
	else
		time_cnt.add(t, 1);

	total_num++;
}