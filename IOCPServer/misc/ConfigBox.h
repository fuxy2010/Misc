#ifndef _CCONFIGBOX_H_
#define _CCONFIGBOX_H_

#include <string>
#include <vector>
#include <map>

//
// ͨ��������Ϣ��д��
//
class CConfigBox
{
public:
    CConfigBox();
    virtual ~CConfigBox();

    // ���������ļ�
    virtual void load(const std::string& config_file);

    // ���������ļ�
    virtual void save();

    // ��ȡ������Ϣ
    virtual std::string get_property(const std::string& key, const std::string& default_property);
    virtual int get_property_as_int(const std::string& key, int default_property);
    virtual void get_properties_with_prefix(const std::string& key_prefix, std::vector<std::string>& properties);	
    virtual void get_properties_map_with_prefix(const std::string& key_prefix, std::map<std::string, std::string>& properties_map);

    // д������Ϣ
    virtual void set_property(const std::string& key, const std::string& property);	
    virtual void set_property_as_int(const std::string& key, int property);	

private:
    // ɾ���ַ���ǰ��ո�TAB���ͻس����з�
    std::string trim(const std::string& str);

private:
    bool _is_open;                          // �����ļ��Ƿ�򿪳ɹ���־
    bool _is_modified;                      // ������Ϣ�Ƿ��޸ı�־
    std::string _config_file;               // �����ļ���
    std::vector<std::string> _config_lines; // ������Ϣ�б�
};

#endif // _CCONFIGBOX_H_
