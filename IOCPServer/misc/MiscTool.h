// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _MISC_TOOL_H_
#define _MISC_TOOL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <windows.h>
#include <direct.h>
#include <time.h>
#include <math.h>
#include <iomanip>

//���ù���
namespace MiscTools
{
	//�ļ�����
	//��ȡ�ļ���С
	unsigned long get_file_size(std::string file_path);

	//ѡ���ļ����·��,Ĭ���ļ���СΪ20MB
	std::string select_file_path(unsigned long file_size = (20480*1024));

	//���ļ�����·���г�ȡ�ļ���
	std::string extract_file_name_from_path(const std::string& file_path);

	//�����ַ�����ϣ��һ��10λ�޷��ų���������
	unsigned long hast_string_to_ul(const std::string& str);

	//�ַ�������
	//ɾ���ַ�����߿հ�
	std::string trim_string_left(const std::string& str);

	//ɾ���ַ����ұ߿հ�
	std::string trim_string_right(const std::string& str);

	//ɾ���ַ����������߿հ�
	std::string trim_string(const std::string& str);

	//���ַ���ת��ΪСд
	std::string string_to_lower(const std::string& str);

	//���ַ���ת��Ϊ��д
	std::string string_to_upper(const std::string& str);

	//�ַ���str�Ƿ���substr��ͷ
	bool is_string_start_with(const std::string& str, const std::string& substr);

	//�ַ���str�Ƿ���substr��ͷ
	bool is_string_end_with(const std::string& str, const std::string& substr);

	//���Դ�Сд�Ƚ��ַ���
	bool compare_string_ignore_case(const std::string& str1, const std::string& str2);

	//��ʱ��ת��Ϊ����2010-01-01 12:30:30���ַ���
	std::string parse_time_to_string(const struct tm& time);

	//������ת��Ϊ����2010-03-17 16:45:00���ַ���
	std::string parse_time_to_string(const time_t seconds);

	//����ǰʱ��ת��Ϊ����2010-01-01 12:30:30���ַ���
	std::string parse_now_to_string();

	//������20100101123030���ַ���ת��Ϊʱ��
	struct tm parse_string_to_time(const std::string& str);

	//�ַ���ת��Ϊbool����ֵ
	bool parse_string_to_bool(const std::string& str);

	//�ַ�����ʶ������
	class Tokenizer
	{
	public:
		static const std::string DEFAULT_DELIMITERS;
		Tokenizer(const std::string& str);
		Tokenizer(const std::string& str, const std::string& delimiters);

		//������һ����ʶ
		bool NextToken(void);
		bool NextToken(const std::string& delimiters);

		//��ȡ��ʶ�ַ���
		const std::string GetToken(void) const;

		//���ñ�ʶλ��
		void Reset(void);

	protected:
		size_t _offset;
		const std::string _string;
		std::string _token;
		std::string _delimiters;
	};

	//���ݷָ���delimiters�ָ��ַ���
	void split_string(const std::string& str, const std::string& delimiters, std::vector<std::string>& str_vector);

	//���ַ�����ת��Ϊ��������
	template<class T> T parse_string_to_type(const std::string& str)
	{
		T value;
		istringstream iss(str);
		iss >> value;
		return value;
	};

	//��ʮ�������ַ�����ת��Ϊ��������
	template<class T> T parse_hex_string_to_type(const std::string& str)
	{
		T value;
		istringstream iss(str);
		iss >> hex >> value;
		return value;
	};

	//����������ת��Ϊ�ַ���
	template<class T> std::string parse_type_to_string(const T& value)
	{
		ostringstream oss;
		oss << value;
		return oss.str();
	};

	//����������ת��Ϊʮ�������ַ���
	template<class T> std::string parse_type_to_hex_string(const T& value, int width)
	{
		ostringstream oss;
		oss << hex;
		if (width > 0) {
			oss << setw(width) << setfill('0');
		}
		oss << value;
		return oss.str();
	};

	//ʱ�����
	//�õ���ǰ�����
	int get_cur_year();

	//��������2010-6-1 12:30:01���ַ��������ʱ�����ʱ������
	unsigned long get_time_diff(const std::string& time1, const std::string& time2);

	//��IP�ַ���ת��Ϊ�����ֽ���IP��ֵ
	unsigned long parse_string_to_ip(const char* str);

	//�������ֽ���IP��ֵת��Ϊ�ַ���
	char* parse_ip_to_string(unsigned long ip);

	//��ǰ��ʱ����last_timestamp�Ƿ񳬹���INTERVAL
	inline bool time_is_up(unsigned long INTERVAL, unsigned long last_timestamp)
	{
		unsigned long cur_timestamp = timeGetTime();

		//��ֹtimeGetTime()����ֵ����ֵ�����޺����
		unsigned long durance = (last_timestamp > cur_timestamp) ? (0xFFFFFFFF - last_timestamp + cur_timestamp) : (cur_timestamp - last_timestamp);

		return (INTERVAL <= durance);
	}
};

#endif // _MISC_TOOL_H_
