// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
#include "ParseTask.h"
#include "MiscTool.h"
#include "TimeConsuming.h"
#include <math.h>
#include <vector>

using namespace ScheduleServer;

CParseTask::CParseTask(PARSE_TASK_INFO& task_info) :
_status(ParseTask_Begin),
_latitude_wgs(0.0),
_longitude_wgs(0.0),
_latitude_str(""),
_longitude_str(""),
_dev_id("")
{
	_task_info = task_info;
}

CParseTask::~CParseTask()
{
	_task_info.clear();
}

SS_Error CParseTask::run()
{
	CTimeConsuming tc('W', 5.0);

	if(true == parse())
	{
		if(true == wgs2mars())
		{
			process();
		}
	}

	on_done();

	return SS_NoErr;
}

string CParseTask::parse_id(const char* content)
{
	string dev_id = "";

	if('*' == content[0])
	{
		//"*HQ,2141028712,V1,105147,A,3036.5622,N,11411.1614,E,000.00,254,081214,FFFFFBFF#"
		string msg = content;

		if('*' == msg.at(0) && '#' == msg[msg.length() - 1])
		{
			dev_id = msg.substr(msg.find(',') + 1, msg.find("V1") - msg.find(',') - 2);
		}
	}
	else if('$' == content[0])
	{
		char temp[8];
		::memset(temp, 0, sizeof(temp));
		unsigned char c;

		//id
		c = *(content + 0x01);
		sprintf_s(temp, "%.2x",c);
		dev_id += string(temp);

		c = *(content + 0x02);
		sprintf_s(temp, "%.2x",c);
		dev_id += string(temp);

		c = *(content + 0x03);
		sprintf_s(temp, "%.2x",c);
		dev_id += string(temp);

		c = *(content + 0x04);
		sprintf_s(temp, "%.2x",c);
		dev_id += string(temp);

		c = *(content + 0x05);
		sprintf_s(temp, "%.2x",c);
		dev_id += string(temp);
	}

	return dev_id;
}

bool CParseTask::parse()
{
	//cout << "\n\r" << MiscTools::parse_now_to_string() << "=========================== " << endl;
	LOG_WRITE(MiscTools::parse_now_to_string() << "Recv: " << _task_info.msg, 1, false);

	if('*' == _task_info.msg[0])
	{
		//"*HQ,2141028712,V1,105147,A,3036.5622,N,11411.1614,E,000.00,254,081214,FFFFFBFF#"

		string msg = _task_info.msg;
		cout << "Recv Txt: " << msg << " length: " << msg.length() << endl;

		if('*' != msg.at(0) || '#' != msg[msg.length() - 1])
		{
			return false;
		}

		_dev_id = msg.substr(msg.find(',') + 1, msg.find("V1") - msg.find(',') - 2);

		string::size_type begin = msg.find("V1");
		begin += 12;//"V1,105147,A,"

		string gps = msg.substr(begin, msg.length() - begin);

		string::size_type end = string::npos;
		if(string::npos != gps.find_first_of("E"))
		{
			end = msg.find_first_of("E") - 1;
		}
		else if(string::npos != msg.find_first_of("W"))
		{
			end = msg.find_first_of("W") - 1;
		}

		gps = msg.substr(begin, end - begin);
		_latitude_str = gps.substr(0, gps.find_first_of(","));
		_longitude_str = gps.substr(gps.find_last_of(",") + 1, gps.length() - gps.find_last_of(","));
	}
	else if('$' == _task_info.msg[0])
	{
		cout << "Recv Binary: " << _task_info.msg << endl;

		char temp[8];
		::memset(temp, 0, sizeof(temp));
		unsigned char c;

		//id
		c = *(_task_info.msg + 0x01);
		sprintf_s(temp, "%.2x",c);
		_dev_id += string(temp);

		c = *(_task_info.msg + 0x02);
		sprintf_s(temp, "%.2x",c);
		_dev_id += string(temp);

		c = *(_task_info.msg + 0x03);
		sprintf_s(temp, "%.2x",c);
		_dev_id += string(temp);

		c = *(_task_info.msg + 0x04);
		sprintf_s(temp, "%.2x",c);
		_dev_id += string(temp);

		c = *(_task_info.msg + 0x05);
		sprintf_s(temp, "%.2x",c);
		_dev_id += string(temp);

		//latitude
		c = *(_task_info.msg + 0x0C);
		sprintf_s(temp, "%.2x",c);
		_latitude_str += string(temp);

		c = *(_task_info.msg + 0x0D);
		sprintf_s(temp, "%.2x",c);
		_latitude_str += string(temp);

		_latitude_str += ".";

		c = *(_task_info.msg + 0x0E);
		sprintf_s(temp, "%.2x",c);
		_latitude_str += string(temp);

		c = *(_task_info.msg + 0x0F);
		sprintf_s(temp, "%.2x",c);
		_latitude_str += string(temp);

		//longitude
		c = *(_task_info.msg + 0x11);
		sprintf_s(temp, "%.2x",c);
		_longitude_str += string(temp);

		c = *(_task_info.msg + 0x12);
		sprintf_s(temp, "%.2x",c);
		_longitude_str += string(temp);

		c = *(_task_info.msg + 0x13);
		sprintf_s(temp, "%.2x",c);
		_longitude_str += string(temp);

		c = *(_task_info.msg + 0x14);
		sprintf_s(temp, "%.2x",c);
		_longitude_str += string(temp);

		c = *(_task_info.msg + 0x15);
		sprintf_s(temp, "%.2x",c);
		_longitude_str += string(temp);

		cout << "LNG: " << _longitude_str << endl;

		if(5 <= _longitude_str.length())
		{
			_longitude_str = _longitude_str.substr(0, _longitude_str.length() - 1);
			_longitude_str.insert(_longitude_str.length() - 4, ".");
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	_latitude_wgs = MiscTools::parse_string_to_type<double>(_latitude_str.substr(0, _latitude_str.find(".") - 2));
	_latitude_wgs += MiscTools::parse_string_to_type<double>(_latitude_str.substr(_latitude_str.find(".") - 2, _latitude_str.length() - _latitude_str.find(".") + 2)) / 59.9999;

	_longitude_wgs = MiscTools::parse_string_to_type<double>(_longitude_str.substr(0, _longitude_str.find(".") - 2));
	_longitude_wgs += MiscTools::parse_string_to_type<double>(_longitude_str.substr(_longitude_str.find(".") - 2, _longitude_str.length() - _longitude_str.find(".") + 2)) / 59.9999;

	cout << _dev_id << " GPS str: [" << _latitude_str << ", " << _longitude_str << "]" << endl;
	LOG_WRITE(MiscTools::parse_now_to_string() << " GPS str: [" << _latitude_str << ", " << _longitude_str << "]", 1, false);


	char temp[16];
	::memset(temp, 0, sizeof(temp));
	sprintf_s(temp, "%.8f", _latitude_wgs);
	_latitude_str = temp;
	::memset(temp, 0, sizeof(temp));
	sprintf_s(temp, "%.8f", _longitude_wgs);
	_longitude_str = temp;


	cout << _dev_id << " GPS (WGS) double: <" << _latitude_str << ", " << _longitude_str << ">" << endl;
	LOG_WRITE(MiscTools::parse_now_to_string() << " GPS (WGS) double: <" << _latitude_str << ", " << _longitude_str << ">", 1, false);

	return true;
}

const double pi = 3.14159265358979324;
const double a = 6378245.0;
const double ee = 0.00669342162296594323;

bool out_of_china(double lat, double lon)
{
	if (lon < 72.004 || lon > 137.8347)
		return true;
	if (lat < 0.8293 || lat > 55.8271)
		return true;
	return false;
}

double transform_latitude(double x, double y)
{
	double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(abs(x));
	ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
	ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
	ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
	return ret;
}

double transform_longitude(double x, double y)
{
	double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs(x));
	ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
	ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
	ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
	return ret;
}

bool CParseTask::wgs2mars()
{
	if(true == out_of_china(_latitude_wgs, _longitude_wgs))
		return false;

	double dLat = transform_latitude(_longitude_wgs - 105.0, _latitude_wgs - 35.0);
	double dLon = transform_longitude(_longitude_wgs - 105.0, _latitude_wgs - 35.0);
	double radLat = _latitude_wgs / 180.0 * pi;
	double magic = sin(radLat);
	magic = 1 - ee * magic * magic;
	double sqrtMagic = sqrt(magic);
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
	dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
	_latitude_mars = _latitude_wgs + dLat;
	_longitude_mars = _longitude_wgs + dLon;

	char temp[16];
	::memset(temp, 0, sizeof(temp));
	sprintf_s(temp, "%.8f", _latitude_mars);
	_latitude_str = temp;
	::memset(temp, 0, sizeof(temp));
	sprintf_s(temp, "%.8f", _longitude_mars);
	_longitude_str = temp;

	cout << _dev_id << " GPS(MARS) double: <" << _latitude_str << ", " << _longitude_str << ">" << endl;
	LOG_WRITE(MiscTools::parse_now_to_string() << " GPS(MARS) double: <" << _latitude_str << ", " << _longitude_str << ">", 1, false);

	return true;
}

typedef struct tagGPSPoint
{
	double X;//longitude
	double Y;//latitude

	tagGPSPoint() : X(0.0), Y(0.0)
	{
	}
}
GPSPoint;

//返回1在多边形内；-1不在多边形内；0在多边形的边上
int point_in_polygon(GPSPoint point, vector<GPSPoint> polygon)
{
	int point_count = polygon.size();

	if(0 >= point_count)
		return -1;

	bool on_beside = false;

	double maxX;
	double maxY;
	double minX;
	double minY;

	maxX = polygon[0].X;
	minX = polygon[0].X;
	maxY = polygon[0].Y;
	minY = polygon[0].Y;

	for(int j = 1; j < point_count; j++)
	{
		if(polygon[j].X >= maxX)
			maxX = polygon[j].X;
		else if (polygon[j].X <= minX)
			minX = polygon[j].X;

		if (polygon[j].Y >= maxY)
			maxY = polygon[j].Y;
		else if (polygon[j].Y <= minY)
			minY = polygon[j].Y;
	}

	if ((point.X > maxX) || (point.X < minX) || (point.Y > maxY) || (point.Y < minY))
		return -1;
	
	int cross_num = 0;

	for(int i = 0; i < point_count; i++)
	{
		GPSPoint p1 = polygon[i];
		GPSPoint p2 = polygon[(i + 1) % point_count];

		// 求解 y=p.y 与 p1p2 的交点
		if (p1.Y == p2.Y) // p1p2 与 y=p0.y平行 
		{
			if (point.Y == p1.Y && point.X >= min(p1.X, p2.X) && point.X <= max(p1.X, p2.X))
			{
				on_beside = true;
				continue;
			}
		}

		if (point.Y < min(p1.Y, p2.Y) || point.Y > max(p1.Y, p2.Y)) // 交点在p1p2延长线上 
			continue;


		// 求交点的 X 坐标 -------------------------------------------------------------- 
		double x = (double)(point.Y - p1.Y) * (double)(p2.X - p1.X) / (double)(p2.Y - p1.Y) + p1.X;

		if (x > point.X)
			cross_num++; // 只统计单边交点 
		else if (x == point.X)
			on_beside = true;
	}

	if (true == on_beside)
		return 0;//多边形边上
	else if(1 == (cross_num % 2))// 单边交点为偶数，点在多边形之外 --- 
		return 1;//多边形内

	return -1;//多边形外
}

const double EARTH_RADIUS = 6378.137;//地球半径
double rad(double d)
{
	return d * pi / 180.0;
}

double gps_distance(double lat1, double lng1, double lat2, double lng2)
{
	double radLat1 = rad(lat1);
	double radLat2 = rad(lat2);

	double a = radLat1 - radLat2;
	double b = rad(lng1) - rad(lng2);

	double s = 2 * asin(sqrt(pow(sin(a/2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b/2), 2)));
	s = s * EARTH_RADIUS;
	//s = round(s * 10000) / 10000;
	return s;
}

bool CParseTask::process()
{
	string sql = "";
	SS_CTX ctx;

	//write into db
	sql = "insert into sd_db_gps (time, did, longitude, latitude, speed, bearing, state) values (";
	sql += "'" + MiscTools::parse_now_to_string() + "', ";
	sql += "'" + _dev_id + "', ";
	sql += "'" + _longitude_str + "', ";
	sql += "'" + _latitude_str + "', ";
	sql += "'0.0', '0.0', '')";

	ctx.clear();	
	SINGLETON(CScheduleServer).get_db_proxy()->execute_sql(sql, ctx);

	//alarm?
	//围栏
	sql = "select aid from sd_ar_enclosure where did='" + _dev_id + "' and actived='1'";
	ctx.clear();	
	SINGLETON(CScheduleServer).get_db_proxy()->query_record(sql, ctx);
	if("true" == ctx["success"])
	{
		int count = MiscTools::parse_string_to_type<int>(ctx["records_num"]);

		string sql_temp = "";
		SS_CTX ctx_temp;
		vector<GPSPoint> polygon;

		for(int i = 0; i < count; ++i)
		{
			//id
			string pid = ctx["aid_" + MiscTools::parse_type_to_string<int>(i)];

			//direction 0-alarm due to out, 1-alarm due to in
			sql_temp = "select direction from sd_ad_enclosure where id='" + pid + "'";
			ctx_temp.clear();
			SINGLETON(CScheduleServer).get_db_proxy()->query_record(sql_temp, ctx_temp);
			int direction = MiscTools::parse_string_to_type<int>(ctx_temp["direction_0"]);

			//polygon
			sql_temp = "select longitude, latitude from sd_ad_encdetail where pid='" + pid + "'";
			ctx_temp.clear();
			SINGLETON(CScheduleServer).get_db_proxy()->query_record(sql_temp, ctx_temp);

			int count_temp = MiscTools::parse_string_to_type<int>(ctx_temp["records_num"]);
			for(int j = 0; j < count_temp; ++j)
			{
				GPSPoint pt;
				pt.X = MiscTools::parse_string_to_type<double>(ctx_temp["longitude_" + MiscTools::parse_type_to_string<int>(j)]);
				pt.Y = MiscTools::parse_string_to_type<double>(ctx_temp["latitude_" + MiscTools::parse_type_to_string<int>(j)]);

				polygon.push_back(pt);
			}

			GPSPoint pos;
			pos.X = _longitude_mars;
			pos.Y = _latitude_mars;
			int res = point_in_polygon(pos, polygon);

			//alarm
			if((-1 == res && 0 == direction) || (1 == res && 1 == direction))
			{
				sql_temp = "insert into sd_db_alarm (time, aid, did, message, type, state) values (";
				sql_temp += "'" + MiscTools::parse_now_to_string() + "', ";
				sql_temp += "'" + pid + "', ";
				sql_temp += "'" + _dev_id + "', ";
				sql_temp += "'围栏报警', ";
				sql_temp += "'1', '0')";

				ctx_temp.clear();	
				SINGLETON(CScheduleServer).get_db_proxy()->execute_sql(sql_temp, ctx_temp);

				LOG_WRITE(_dev_id << " 围栏报警", 1, true);
			}
		}
	}

	//距离
	sql = "select aid from sd_ar_distance where did='" + _dev_id + "' and actived='1'";
	ctx.clear();	
	SINGLETON(CScheduleServer).get_db_proxy()->query_record(sql, ctx);
	if("true" == ctx["success"])
	{
		int count = MiscTools::parse_string_to_type<int>(ctx["records_num"]);

		string sql_temp = "";
		SS_CTX ctx_temp;

		for(int i = 0; i < count; ++i)
		{
			//id
			string pid = ctx["aid_" + MiscTools::parse_type_to_string<int>(i)];

			//direction 0-alarm due to out, 1-alarm due to in
			sql_temp = "select direction, longitude, latitude, distance from sd_ad_distance where id='" + pid + "'";
			ctx_temp.clear();
			SINGLETON(CScheduleServer).get_db_proxy()->query_record(sql_temp, ctx_temp);

			int direction = MiscTools::parse_string_to_type<int>(ctx_temp["direction_0"]);
			double longitude = MiscTools::parse_string_to_type<double>(ctx_temp["longitude_0"]);
			double latitude = MiscTools::parse_string_to_type<double>(ctx_temp["latitude_0"]);
			double distance = MiscTools::parse_string_to_type<double>(ctx_temp["distance_0"]);

			//distance
			double real_distance = gps_distance(latitude, longitude, _latitude_mars, _longitude_mars);

			if((real_distance > distance && 0 == direction) || (real_distance < distance && 1 == direction))
			{
				sql_temp = "insert into sd_db_alarm (time, aid, did, message, type, state) values (";
				sql_temp += "'" + MiscTools::parse_now_to_string() + "', ";
				sql_temp += "'" + pid + "', ";
				sql_temp += "'" + _dev_id + "', ";
				sql_temp += "'距离报警', ";
				sql_temp += "'2', '0')";

				ctx_temp.clear();	
				SINGLETON(CScheduleServer).get_db_proxy()->execute_sql(sql_temp, ctx_temp);

				LOG_WRITE(_dev_id << " 距离报警 " << real_distance << ", " << distance, 1, true);
			}
		}
	}

	return true;
}

SS_Error CParseTask::on_done()
{
	_task_info.clear();

	_is_done = true;

	return SS_NoErr;
}

SS_Error CParseTask::on_exception()
{
	return SS_NoErr;
}
