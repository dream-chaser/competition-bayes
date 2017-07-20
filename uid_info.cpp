#include "uid_info.h"


uid_info::uid_info()
{
}


uid_info::~uid_info()
{
}

void uid_info::add(std::string label, const char* geo, int time)
{

	if (labels.count(label) > 0)
	{
		labels[label].add(geo, time);
	}
	else
	{
		label_info new_label;
		new_label.add(geo, time);
		labels.add(label, new_label);
	}
}

void uid_info::cal_label_prob()
{
	double label_total_num = 0.0;
	for (int i = 0; i < labels.size(); i++)
	{
		label_total_num += labels.getValue(i).total_num;
	}
	for (int i = 0; i < labels.size(); i++)
	{
		labels.getValue(i).p_label = labels.getValue(i).total_num / label_total_num;
	}
}

void uid_info::find_top_3(const char* geo_s, int min_time, std::string& dst_1st, std::string& dst_2nd, std::string& dst_3rd)
{
	int lb1st = -1, lb2nd = -1, lb3rd = -1;
	double p1st = -1, p2nd = -1, p3rd = -1;
	int label_size = labels.size();
	for (int i = 0; i < label_size; i++)
	{
		double p = labels.getValue(i).cal_prob(geo_s, min_time);
		if (p > p1st)
		{
			p3rd = p2nd; lb3rd = lb2nd;
			p2nd = p1st; lb2nd = lb1st;
			p1st = p;    lb1st = i;
		}
		else if (p > p2nd)
		{
			p3rd = p2nd; lb3rd = lb2nd;
			p2nd = p;    lb2nd = i;
		}
		else if (p > p3rd)
		{
			p3rd = p;    lb3rd = i;
		}
	}
	if (lb1st != -1) dst_1st = labels.getKey(lb1st);
	if (lb2nd != -1) dst_2nd = labels.getKey(lb2nd);
	if (lb3rd != -1) dst_3rd = labels.getKey(lb3rd);
}