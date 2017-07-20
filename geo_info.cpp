#include "geo_info.h"

std::unordered_map<char, int> geo_info::__decodemap;

void geo_info::initialize()
{
	char __base32[40] = "0123456789bcdefghjkmnpqrstuvwxyz";
	int __base32_len = strlen(__base32);
	for (int i = 0; i < __base32_len; i++)
		__decodemap[__base32[i]] = i;
}

geo_info::geo_info()
{
}


geo_info::~geo_info()
{
}

geo_info::geo_info(const char* geo_str)
{
	this->geo_string = geo_str;
	geohash_decode(geo_str, this->lat, this->lon);
}

bool geo_info::operator < (const geo_info& g) const
{
	return this->geo_string < g.geo_string;
}

bool geo_info::operator == (const geo_info& g) const
{
	return this->geo_string == g.geo_string;
}

void geo_info::geohash_decode(const char* geo_str, double& lat, double& lon)
{
	double lat_interval[2] = { -90.0, 90.0 };
	double lon_interval[2] = { -180.0, 180.0 };
	double lat_err = 90.0, lon_err = 180.0;
	bool is_even = true;
	int masks[5] = { 16, 8, 4, 2, 1 };
	int len = strlen(geo_str);

	for (int i = 0; i < len; i++)
	{
		char c = geo_str[i];
		int cd = __decodemap[c];
		for (int j = 0; j < 5; j++)
		{
			int mask = masks[j];
			if (is_even)
			{
				lon_err /= 2;
				if (cd & mask)
					lon_interval[0] = (lon_interval[0] + lon_interval[1]) / 2;
				else
					lon_interval[1] = (lon_interval[0] + lon_interval[1]) / 2;
			}
			else
			{
				lat_err /= 2;
				if (cd & mask)
					lat_interval[0] = (lat_interval[0] + lat_interval[1]) / 2;
				else
					lat_interval[1] = (lat_interval[0] + lat_interval[1]) / 2;
			}
			is_even = !is_even;
		}
	}
	lat = (lat_interval[0] + lat_interval[1]) / 2;
	lon = (lon_interval[0] + lon_interval[1]) / 2;
}