#include "ConfigBox.h"
#include <iostream>
#include <fstream>	
#include <sstream>

using namespace std;

// ----------------------------------------------------------------------
// ������: CConfigBox::CConfigBox
// ����: chenqg
// ����: CConfigBox��Ĺ��캯������Ա��ʼ��
// ����ֵ: ��
// ����: 
//   [void]
// ----------------------------------------------------------------------
CConfigBox::CConfigBox() :
    _is_open(false),
    _is_modified(false),
    _config_file("")
{
    _config_lines.clear();
}

// ----------------------------------------------------------------------
// ������: CConfigBox::~CConfigBox
// ����: chenqg
// ����: CConfigBox�����������
// ����ֵ: ��
// ����: 
//   [void]
// ----------------------------------------------------------------------
CConfigBox::~CConfigBox()
{
	_config_lines.clear();
}

// ----------------------------------------------------------------------
// ������: CConfigBox::load
// ����: chenqg
// ����: �������ļ�����ȡ������Ϣ�б�
// ����ֵ: [void]
// ����: 
//   [const std::string& config_file] ���������ļ���  
// ----------------------------------------------------------------------
void CConfigBox::load(const std::string& config_file)
{
    // ���ifstream���ļ�����·��������
    // ���ô���ҳΪ�������ģ�936�Ǽ������ĵĴ���ҳ��
    std::locale loc1 = std::locale::global(std::locale(".936"));

    // �������ļ�
    ifstream ifs;
    ifs.open(config_file.c_str());
    _is_open = ifs.is_open();

    //�ָ�ԭ���Ĵ���ҳ
    std::locale::global(std::locale(loc1));

    if (_is_open)
    {
        // ���������ļ�
        _config_file = config_file;
        
        // ��ȡ������Ϣ
        _config_lines.clear();
        char buffer[1024];
        while(!ifs.eof())
        {
            memset(buffer, '\0', 1024);
            ifs.getline(buffer, 1024);
            string line = buffer;
            _config_lines.push_back(line);
        }

        _is_modified = false;
    }

	ifs.close();
}

// ----------------------------------------------------------------------
// ������: CConfigBox::save
// ����: chenqg
// ����: ����������Ϣ
// ����ֵ: [void]
// ����: 
//   [void]
// ----------------------------------------------------------------------
void CConfigBox::save()
{
    // ��������ļ���ʧ�ܻ�������Ϣδ�޸ģ���������
    if (!_is_open || !_is_modified) return;

    ofstream ofs;
    ofs.open(_config_file.c_str());
    
    vector<string>::iterator iter = _config_lines.begin();
    while (iter != _config_lines.end())
    {
        if (iter != _config_lines.begin()) ofs << endl;
        ofs << *iter;
        iter++;
    }

    ofs.close();
    _is_modified = false;
}

// ----------------------------------------------------------------------
// ������: CConfigBox::get_property
// ����: chenqg
// ����: ���ݼ�ֵ��ȡ�ַ�������ֵ
// ����ֵ: [std::string] �ַ�������ֵ
// ����: 
//   [const std::string& key] �����ֵ  
//   [const std::string& default_property] ����ȱʡ����ֵ  
// ----------------------------------------------------------------------
std::string CConfigBox::get_property(const std::string& key, const std::string& default_property)
{
    string result = default_property;

    string::size_type pos = -1;
    vector<string>::iterator iter;
    for (iter =_config_lines.begin(); iter != _config_lines.end(); iter++)
    {
        // ȥ���ַ���ǰ��ո�
        string line = trim(*iter);

        // ���Կ���
        if (line.empty()) continue; 

        // ����ע����
        if (line.at(0) == '#') continue; 

        pos = line.find('=');
        if (pos == string::npos) continue;

        // �жϼ�ֵ
        string line_key = trim(line.substr(0, pos)); 
        if (line_key == key)
        {
            result = trim(line.substr(pos + 1));
            break;
        }
    }

    return result;
}

// ----------------------------------------------------------------------
// ������: CConfigBox::get_property_as_int
// ����: chenqg
// ����: ���ݼ�ֵ��ȡ������������ֵ
// ����ֵ: [int] ������������ֵ
// ����: 
//   [const std::string& key] �����ֵ  
//   [int default_property] ����ȱʡ����ֵ  
// ----------------------------------------------------------------------
int CConfigBox::get_property_as_int(const std::string& key, int default_property)
{
    int result = default_property;

    string str = get_property(key, "");
    if (!str.empty())
    {
        result = atoi(str.c_str());
    }

    return result;
}

// ----------------------------------------------------------------------
// ������: CSingleLogger::GetPropertiesWithPrefix
// ����: chenqg
// ����: ���ݼ�ֵǰ׺��ȡ�����б�
// ����ֵ: [void]
// ����: 
//   [const std::string& key_prefix] �����ֵǰ׺  
//   [std::vector<std::string>& properties] ��������б�  
// ----------------------------------------------------------------------
void CConfigBox::get_properties_with_prefix(const std::string& key_prefix, std::vector<std::string>& properties)
{
    string::size_type pos = -1;
    vector<string>::iterator iter;
    for (iter =_config_lines.begin(); iter != _config_lines.end(); iter++)
    {
        // ȥ���ַ���ǰ��ո�
        string line = trim(*iter);

        // ���Կ���
        if (line.empty()) continue; 

        // ����ע����
        if (line.at(0) == '#') continue; 

        pos = line.find('=');
        if (pos == string::npos) continue;

        // �жϼ�ֵǰ׺�Ƿ���ͬ
        string line_key = trim(line.substr(0, pos));
        if (line_key.find(key_prefix) == string::npos) continue;

        string line_key_prefix = line_key.substr(0, key_prefix.size()); 
        string line_key_suffix = trim(line_key.substr(key_prefix.size(), line_key.size()));
        if (line_key_prefix == key_prefix)
        {
            properties.push_back(trim(line.substr(pos + 1)));
        }
    }
}

// ----------------------------------------------------------------------
// ������: CConfigBox::get_properties_map_with_prefix
// ����: chenqg
// ����: ���ݼ�ֵǰ׺��ȡ�����б�
// ����ֵ: 
//   [void] 
// ����: 
//   [const std::string& key_prefix] �����ֵǰ׺   
//   [std::map<std::string, std::string>& properties_map] ��������б�
// ----------------------------------------------------------------------
void CConfigBox::get_properties_map_with_prefix(const std::string& key_prefix, std::map<std::string, std::string>& properties_map)
{
    string::size_type pos = -1;
    vector<string>::iterator iter;
    for (iter =_config_lines.begin(); iter != _config_lines.end(); iter++)
    {
        // ȥ���ַ���ǰ��ո�
        string line = trim(*iter);

        // ���Կ���
        if (line.empty()) continue; 

        // ����ע����
        if (line.at(0) == '#') continue; 

        pos = line.find('=');
        if (pos == string::npos) continue;

        // �жϼ�ֵǰ׺�Ƿ���ͬ
        string line_key = trim(line.substr(0, pos));
        if (line_key.find(key_prefix) == string::npos) continue;

        string line_key_prefix = line_key.substr(0, key_prefix.size()); 
        string line_key_suffix = trim(line_key.substr(key_prefix.size(), line_key.size()));
        if (line_key_prefix == key_prefix)
        {
			properties_map[key_prefix + line_key_suffix] = trim(line.substr(pos + 1));
        }
    }
}

// ----------------------------------------------------------------------
// ������: CSingleLogger::set_property
// ����: chenqg
// ����: ���ݼ�ֵ��������
// ����ֵ: [void]
// ����: 
//   [const std::string& key] �����ֵ  
//   [const std::string& property] ��������  
// ----------------------------------------------------------------------
void CConfigBox::set_property(const std::string& key, const std::string& property)	
{
    bool is_key_found = false;
    ostringstream oss;
    oss << key << "=" << property;

    string::size_type pos = -1;
    vector<string>::iterator iter;
    for (iter =_config_lines.begin(); iter != _config_lines.end(); iter++)
    {
        // ȥ���ַ���ǰ��ո�
        string line = trim(*iter);

        // ���Կ���
        if (line.empty()) continue; 

        // ����ע����
        if (line.at(0) == '#') continue; 

        pos = line.find('=');
        if (pos == string::npos) continue;

        // �жϼ�ֵ
        string line_key = trim(line.substr(0, pos)); 
        if (line_key == key)
        {
            is_key_found = true;
            *iter = oss.str();
            break;
        }
    }

    if (!is_key_found)
    {
        _config_lines.push_back(oss.str());
    }

    _is_modified = true;
}

// ----------------------------------------------------------------------
// ������: CSingleLogger::set_property_as_int
// ����: chenqg
// ����: ���ݼ�ֵ��������
// ����ֵ: [void]
// ����: 
//   [const std::string& key] �����ֵ  
//   [int property] ��������  
// ----------------------------------------------------------------------
void CConfigBox::set_property_as_int(const std::string& key, int property)
{
    ostringstream oss;
    oss << property;
    set_property(key, oss.str());
}

// ----------------------------------------------------------------------
// ������: CSingleLogger::trim
// ����: chenqg
// ����: ɾ���ַ���ǰ��ո�TAB���ͻس����з�
// ����ֵ: [std::string] �ַ���
// ����: 
//   [const std::string& str] �����ַ���  
// ----------------------------------------------------------------------
std::string CConfigBox::trim(const std::string& str) 
{
    string t = str;
    t.erase(0, t.find_first_not_of(" \t\n\r"));
    t.erase(t.find_last_not_of(" \t\n\r") + 1);
    return t;
}
