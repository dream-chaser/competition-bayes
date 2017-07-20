#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <algorithm> 

#include "fast_map.h"
#include "geo_info.h"
#include "uid_info.h"
#include "label_info.h"
#include "test_postcompute.h"

using namespace std;

unordered_map<int, uid_info>* uids;
uid_info all_uid_info;

int get_weekday(int year, int month, int day)
{
	if (month < 3)
	{
		year -= 1;
		month += 12;
	}
	int c = int(year / 100), y = year - 100 * c;
	int w = int(c / 4) - 2 * c + y + int(y / 4) + (26 * (month + 1) / 10) + day - 1;
	return ((w % 7 + 7) % 7);
}

void get_filenames(string& trainFilename, string& testFilename, string& outFilename)
{
	int part;
	printf("Which part:\n");
	scanf("%d", &part);
	char fbuf[30];
	sprintf(fbuf, "%d.csv", part);
	string part_name(fbuf);

	// need to initialize
	string train;
	string test;
	string out;
	
	test += part_name;
	out += part_name;

	trainFilename = train;
	testFilename = test;
	outFilename = out;
}

int main()
{
	geo_info::initialize();
	string trainFilename, testFilename, outFilename;
	get_filenames(trainFilename, testFilename, outFilename);
	
	FILE* trainFile = fopen(trainFilename.c_str(), "r");
	char buf[100];
	int oid, uid, bid, btp, year, month, day, hour, minute, second;
	char geo_s[10], geo_d[10];
	int line = 0;

	
	uids = new unordered_map<int, uid_info>();
	while (fgets(buf, 100, trainFile) != NULL)
	{
		line++;
		if (line % 100000 == 0) printf("%d\n", line);
		if (strlen(buf) > 2 && buf[0] != 'o')
		{
			sscanf(buf, "%d,%d,%d,%d,%d-%d-%d %d:%d:%d,%7s,%7s",
				&oid, &uid, &bid, &btp, &year, &month, &day, &hour, &minute, &second, geo_s, geo_d);
			string label_string(geo_d);
			if (uids->count(uid) > 0)
			{
				(*uids)[uid].add(label_string, geo_s, hour * 6 + minute / 10); // ÒÔ10·ÖÖÓÎªµ¥Î»
			}
			else
			{
				uid_info new_uid;
				new_uid.add(label_string, geo_s, hour * 6 + minute / 10);
				(*uids)[uid] = new_uid;
			}
			//all_uid_info.add(label_string, geo_s, hour * 6 + minute / 10);
		}
	}
	fclose(trainFile);

	unordered_map<int, uid_info>::iterator uid_iter;
	for (uid_iter = uids->begin(); uid_iter != uids->end(); uid_iter++)
		uid_iter->second.cal_label_prob();
	//all_uid_info.cal_label_prob();

	test_postcompute tp;
	FILE* testFile = fopen(testFilename.c_str(), "r");
	FILE* outFile = fopen(outFilename.c_str(), "w");
	line = 0;
	while (fgets(buf, 100, testFile) != NULL)
	{
		line++;
		if (line % 10000 == 0)
		{
			printf("%d\n", line);
		}
		if (strlen(buf) > 2 && buf[0] != 'o')
		{
			sscanf(buf, "%d,%d,%d,%d,%d-%d-%d %d:%d:%d.0,%7s",
				&oid, &uid, &bid, &btp, &year, &month, &day, &hour, &minute, &second, geo_s);
			string dst[3] = { "","","" };
			string geo_str(geo_s);
			int min_time = hour * 6 + minute / 10;

			if (uids->count(uid) > 0)
			{
				(*uids)[uid].find_top_3(geo_s, min_time, dst[0], dst[1], dst[2]);
			}
			else
			{
				tp.add(oid, min_time, geo_str, 3, dst);
				continue;
			}
			if (dst[1] == "")
			{
				tp.add(oid, min_time, geo_str, 2, dst);
				continue;
			}
			if (dst[2] == "")
			{
				tp.add(oid, min_time, geo_str, 1, dst);
				continue;
			}
			fprintf(outFile, "%d,%s,%s,%s\n", oid, dst[0].c_str(), dst[1].c_str(), dst[2].c_str());
		}
	}
	fclose(testFile);
	delete uids;
	printf("Uids(labels >= 3) have finished computing.\n");

	trainFile = fopen(trainFilename.c_str(), "r");
	line = 0;
	while (fgets(buf, 100, trainFile) != NULL)
	{
		line++;
		if (line % 100000 == 0) printf("%d\n", line);
		if (strlen(buf) > 2 && buf[0] != 'o')
		{
			sscanf(buf, "%d,%d,%d,%d,%d-%d-%d %d:%d:%d,%7s,%7s",
				&oid, &uid, &bid, &btp, &year, &month, &day, &hour, &minute, &second, geo_s, geo_d);
			string label_string(geo_d);
			all_uid_info.add(label_string, geo_s, hour * 6 + minute / 10);
		}
	}
	fclose(trainFile);
	all_uid_info.cal_label_prob();
	tp.compute_print(outFile, &all_uid_info);
	fclose(outFile);

	system("pause");
	return 0;
}

//int main()
//{
//	const double PI = (4 * atan(1));
//	const double time_sigma = 48;
//	const double  time_exp_coeff = -0.5 / (time_sigma*time_sigma);
//	const double  time_coeff = 1 / (sqrt(2 * PI)*time_sigma);
//	double det = 0.0001;
//	double sum = 0;
//	for (double i = -72; i <= 72; i += det)
//	{
//		sum += time_coeff * exp(time_exp_coeff * i * i) * det;
//	}
//	printf("%f\n", time_coeff * exp(time_exp_coeff * 0 * 0) / sum);
//	printf("%f\n", time_coeff * exp(time_exp_coeff * 72 * 72) / sum);
//	printf("%.10f\n", sum);
//	system("pause");
//	return 0;
//}
