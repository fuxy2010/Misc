// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _COMMAND_CONTEX_PARSE_H_
#define _COMMAND_CONTEX_PARSE_H_

#include <string>
#include <vector>
#include <map>

//�������ݽ�����
class CCommandContextParse
{
public:
	CCommandContextParse(const std::string& command_context, const std::string& interval_flag = ";", const std::string& equal_flag = ",");
    virtual ~CCommandContextParse();

    virtual std::string get_value(const std::string& key);
    virtual void get_values_with_prefix(const std::string& key_prefix, std::vector<std::string>& values);	
    virtual void get_values_map_with_prefix(const std::string& key_prefix, std::map<std::string, std::string>& values_map);

private:
    // ɾ���ַ���ǰ��ո�TAB���ͻس����з�
    std::string trim(const std::string& str);

private:
	std::string _interval_flag;//�������

	std::string _equal_flag;//���ڷ���

    std::vector<std::string> _command_line; // ������Ϣ�б�
};

#endif // _COMMAND_CONTEX_PARSE_H_
