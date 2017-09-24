// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#include "MiscTool.h"

namespace MiscTools
{
	unsigned long get_file_size(std::string file_path)
	{
		struct stat file_stat;

		::ZeroMemory(&file_stat, sizeof(file_stat));

		if(0 > ::stat(file_path.c_str(), &file_stat))
			return 0;

		return static_cast<unsigned long>(file_stat.st_size);
	};

	std::string select_file_path(unsigned long file_size)
	{
		std::string disk_path = "C:\\";
		char selected_disk = 'C';
		ULARGE_INTEGER disk_available_size;
		ULARGE_INTEGER temp1;
		ULARGE_INTEGER temp2;
		ULARGE_INTEGER temp3;

		disk_available_size.QuadPart = 0;

		for(char disk = selected_disk; disk <= 'Z'; ++disk)
		{
			disk_path.replace(0, 1, 1, disk);

			if(DRIVE_FIXED != ::GetDriveType(disk_path.c_str()))
				continue;

			if( ::GetDiskFreeSpaceEx(disk_path.c_str(), &temp1, &temp2, &temp3))
			{
				if(temp1.QuadPart > disk_available_size.QuadPart)
				{
					disk_available_size = temp1;
					selected_disk = disk;
				}
			}
		}

		std::string selected_disk_path = "C:\\DetectionRecord\\";
		selected_disk_path.replace(0, 1, 1, selected_disk);

		::mkdir(selected_disk_path.c_str());

		return selected_disk_path;
	};

	std::string extract_file_name_from_path(const std::string& file_path)
	{
		std::string file_name = file_path;

		if(std::string::npos != file_name.find('/'))
			file_name.erase(file_name.begin(), file_name.begin() + file_name.find_last_of('/') + 1);

		if(std::string::npos != file_name.find('\\'))
			file_name.erase(file_name.begin(), file_name.begin() + file_name.find_last_of('\\') + 1);

		return file_name;
	};

	unsigned long hast_string_to_ul(const std::string& str)
	{
#if 0//���㸴�Ӷȸ�
		unsigned long hash = 1315423911;//nearly a prime - 1315423911 = 3 * 438474637

		for(int i = 0; i< str.size(); ++i)
		{
			hash ^= ((hash << 5) + str.at(i) + (hash >> 2));
		}

		return (hash & 0x7FFFFFFF);
#else
		unsigned long hash = 0;

		for(int i = 0; i < str.size(); ++i)
		{
			//equivalent to: hash = 65599*hash + (*str++);
			hash = str.at(i) + (hash << 6) + (hash << 16) - hash;
		}

		return (hash & 0x7FFFFFFF);
#endif
	};

	int get_cur_year()
	{
		time_t now = ::time(NULL);

		struct tm time;

		::localtime_s(&time, &now);

		return (time.tm_year + 1900);
	};

	unsigned long get_time_diff(const std::string& time1, const std::string& time2)
	{

		struct tm tm1,tm2;

		tm1.tm_year  = atoi(time1.substr(0,time1.find_first_of("-")).c_str())-1900;   //�õ�time1��

		tm1.tm_mon  =  atoi(time1.substr(time1.find_first_of("-")+1,time1.find_last_of("-")-time1.find_first_of("-")-1).c_str());    //�õ�time1��

		tm1.tm_mday =  atoi(time1.substr(time1.find_last_of("-")+1,time1.find_first_of(" ")-time1.find_last_of("-")-1).c_str());       //�õ�time1��

		tm1.tm_hour =  atoi(time1.substr(time1.find_first_of(" ")+1,time1.find_first_of(":")-time1.find_first_of(" ")-1).c_str());     //�õ�time1ʱ

		tm1.tm_min  =  atoi(time1.substr(time1.find_first_of(":")+1,time1.find_last_of(":")-time1.find_first_of(":")-1).c_str());  //�õ�time1��

		tm1.tm_sec  =  atoi(time1.substr(time1.find_last_of(":")+1,time1.length()-time1.find_last_of(":")-1).c_str());   //�õ�time1��

		tm2.tm_year =  atoi(time2.substr(0,time2.find_first_of("-")).c_str())-1900;   //�õ�time2��

		tm2.tm_mon  =  atoi(time2.substr(time2.find_first_of("-")+1,time2.find_last_of("-")-time2.find_first_of("-")-1).c_str());   //�õ�time2��

		tm2.tm_mday =  atoi(time2.substr(time2.find_last_of("-")+1,time2.find_first_of(" ")-time2.find_last_of("-")-1).c_str());       //�õ�time2��

		tm2.tm_hour =  atoi(time2.substr(time2.find_first_of(" ")+1,time2.find_first_of(":")-time2.find_first_of(" ")-1).c_str());     //�õ�time2ʱ

		tm2.tm_min  =  atoi(time2.substr(time2.find_first_of(":")+1,time2.find_last_of(":")-time2.find_first_of(":")-1).c_str());  //�õ�time2��

		tm2.tm_sec  =  atoi(time2.substr(time2.find_last_of(":")+1,time2.length()-time2.find_last_of(":")-1).c_str());   //�õ�time2��

		return static_cast<unsigned long>(fabs(difftime(mktime(&tm2), mktime(&tm1))));//�õ���������		
	}

	//Ĭ�ϵ��ַ����ָ��ʶ��
	const std::string Tokenizer::DEFAULT_DELIMITERS(" ");

	Tokenizer::Tokenizer(const std::string& str)
		: _string(str), _offset(0), _delimiters(DEFAULT_DELIMITERS) {}

	Tokenizer::Tokenizer(const std::string& str, const std::string& delimiters)
		: _string(str), _offset(0), _delimiters(delimiters) {}

	//������һ����ʶ
	bool Tokenizer::NextToken() 
	{
		return NextToken(_delimiters);
	}

	//������һ����ʶ
	bool Tokenizer::NextToken(const std::string& delimiters) 
	{
		// ������һ����ʶ��ʼ�ַ�λ��
		size_t i = _string.find_first_not_of(delimiters, _offset);
		if (i == std::string::npos) {
			_offset = _string.length();
			return false;
		}

		// ���ұ�ʶ����λ��
		size_t j = _string.find_first_of(delimiters, i);
		if (j == std::string::npos) {
			_token = _string.substr(i);
			_offset = _string.length();
			return true;
		}

		_token = _string.substr(i, j - i);
		_offset = j;
		return true;
	}

	//��ȡ��ʶ
	const std::string Tokenizer::GetToken(void) const 
	{
		return _token;
	}

	//���ñ�ʶλ��
	void Tokenizer::Reset(void) 
	{
		_offset = 0;
	}

	//�ַ�������
	// ɾ���ַ�����߿հ�
	std::string trim_string_left(const std::string& str)
	{
		std::string t = str;
		t.erase(0, t.find_first_not_of(" \t\n\r"));
		return t;
	};

	// ɾ���ַ����ұ߿հ�
	std::string trim_string_right(const std::string& str)
	{
		std::string t = str;
		t.erase(t.find_last_not_of(" \t\n\r") + 1);
		return t;
	};

	// ɾ���ַ����������߿հ�
	std::string trim_string(const std::string& str)
	{
		std::string t = str;
		t.erase(0, t.find_first_not_of(" \t\n\r"));
		t.erase(t.find_last_not_of(" \t\n\r") + 1);
		return t;
	};

	//���ַ���ת��ΪСд
	std::string string_to_lower(const std::string& str)
	{
		std::string t = str;
		transform(t.begin(), t.end(), t.begin(), ::tolower);
		return t;
	};

	//���ַ���ת��Ϊ��д
	std::string string_to_upper(const std::string& str)
	{
		std::string t = str;
		transform(t.begin(), t.end(), t.begin(), ::toupper);
		return t;
	};

	//�ַ���str�Ƿ���substr��ͷ
	bool is_string_start_with(const std::string& str, const std::string& substr)
	{
		return (!str.find(substr)) ? true : false;
	};

	//�ַ���str�Ƿ���substr��ͷ
	bool is_string_end_with(const std::string& str, const std::string& substr)
	{
		return (str.rfind(substr) == (str.length() - substr.length())) ? true : false;
	};

	//���Դ�Сд�Ƚ��ַ���
	bool compare_string_ignore_case(const std::string& str1, const std::string& str2)
	{
		return (string_to_lower(str1) == string_to_lower(str2)) ? true : false;
	};

	//�ַ���ת��Ϊbool����ֵ
	bool parse_string_to_bool(const std::string& str)
	{
		//bool value;
		//std::istringstream iss(str);
		//iss >> std::boolalpha >> value;
		//return value;

		return compare_string_ignore_case(trim_string(str), "true");
	};

	//���ݷָ���delimiters�ָ��ַ���
	void split_string(const std::string& str, const std::string& delimiters, std::vector<std::string>& str_vector)
	{
		str_vector.clear();

		Tokenizer tokenizer(str, delimiters);
		while (tokenizer.NextToken()) 
		{
			str_vector.push_back(tokenizer.GetToken());
		}
	};

	//��ʱ��ת��Ϊ����2010-01-01 12:30:30���ַ���
	std::string parse_time_to_string(const struct tm& time)
	{
		char str[32];

		::ZeroMemory(str, sizeof(str));

		sprintf_s(str, "%04d-%02d-%02d %02d:%02d:%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
		
		return str;
	};

	//����ǰʱ��ת��Ϊ����2010-01-01 12:30:30���ַ���
	std::string parse_now_to_string()
	{
		time_t now = ::time(NULL);

		struct tm time;

		::localtime_s(&time, &now);

		char str[32];

		::ZeroMemory(str, sizeof(str));

		sprintf_s(str, "%04d-%02d-%02d %02d:%02d:%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

		return str;
	}

	//������ת��Ϊ����2010-03-17 16:45:00���ַ���
	std::string parse_time_to_string(const time_t seconds)
	{
		struct tm time;

		localtime_s(&time, &seconds);

		char str[32];

		::ZeroMemory(str, sizeof(str));

		sprintf_s(str, "%04d-%02d-%02d %02d:%02d:%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

		return str;
	}

	struct tm parse_string_to_time(const std::string& str)
	{
		struct tm time;

		::ZeroMemory(&time, sizeof(struct tm));

		if(14 != str.size())
			return time;

		std::string temp = "";

		//year
		temp = str;
		temp.erase(4, 10);
		time.tm_year = ::atoi(temp.c_str()) - 1900;

		//month
		temp = str;
		temp.erase(0, 4);
		temp.erase(2, 8);
		time.tm_mon = ::atoi(temp.c_str()) - 1;

		//day
		temp = str;
		temp.erase(0, 6);
		temp.erase(2, 6);
		time.tm_mday = ::atoi(temp.c_str());

		//hour
		temp = str;
		temp.erase(0, 8);
		temp.erase(2, 4);
		time.tm_hour = ::atoi(temp.c_str());

		//minute
		temp = str;
		temp.erase(0, 10);
		temp.erase(2, 2);
		time.tm_min = ::atoi(temp.c_str());

		//second
		temp = str;
		temp.erase(0, 12);
		time.tm_sec = ::atoi(temp.c_str());

		return time;
	};

	unsigned long parse_string_to_ip(const char* str)
	{
		return inet_addr(str);
	}

	char* parse_ip_to_string(unsigned long ip)
	{
		return inet_ntoa(*(reinterpret_cast<struct in_addr*>(&ip)));
	}

}

