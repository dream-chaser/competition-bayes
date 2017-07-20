#pragma once
#include <string>
#include <unordered_map>
#include <functional>

// must call initialize() firstly
class geo_info
{
public:
	geo_info();
	geo_info(const char* geo_str);
	~geo_info();
	static void initialize();
	bool operator < (const geo_info& g) const;
	bool operator == (const geo_info& g) const;

	std::string geo_string;
	double lat, lon;

	struct geo_inner_hash
	{
		std::hash<std::string> inner_str_hash;
		size_t operator()(const geo_info& geo) const
		{
			return inner_str_hash(geo.geo_string);
		}
	};

private:
	static std::unordered_map<char, int> __decodemap;
	static void geohash_decode(const char* geo_str, double& lat, double& lon);
};

