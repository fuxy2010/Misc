// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _I_MODULE_H_
#define _I_MODULE_H_

#include <string>
#include <vector>
#include <map>

#include "GeneralDef.h"

// DLL�������
#ifdef DLL_EXPORTS
#define MODULE_API __declspec (dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

namespace ScheduleServer
{
	class IModule;

	//�豸����ģ�����ӿں�����
	#define DLL_EXPORT_FUNCTION_NAME "ModuleInstance"

	//�豸����ģ�����ӿں������������ú����Ķ�����ÿ�������ͷ�ļ���
	#define DECLARE_EXPORT_FUNCTION() \
		MODULE_API IModule* ModuleInstance();

	//�豸����ģ�����ӿں����Ķ��壬�ú����Ķ�����ÿ�������cpp�ļ���
	#define IMPLEMENT_EXPORT_FUNCTION(class_name) \
		IModule* ModuleInstance() \
		{\
			return new class_name##();\
		}

	//�豸����ģ�����ӿں�������
	typedef IModule* (*PLUG_IN_MODULE_API)();

	typedef struct tagMODULE_DESCRIPTION
	{
		//std::string name;//ģ������
		//std::string version;//ģ��汾
		std::string file_path;//���ģ���Ӧ�Ķ�̬���ӿ��ļ�����·���������豸������ģ����Ч
		bool available;//ģ�����ʹ��״̬

		// ȱʡ���캯��
		tagMODULE_DESCRIPTION() : file_path(""), available(false)
		{
		}
	}
	MODULE_DESCRIPTION, *MODULE_DESCRIPTION_PTR;

	//ģ�����
	class IModule
	{
	public:
		// ȱʡ���캯��
		IModule() {};

		// ȱʡ��������
		virtual ~IModule() {};

		// ��ȡģ����Ϣ
		MODULE_DESCRIPTION& get_module_description() {return _module_description;};

		// ģ���ʼ��
		virtual SS_Error start() = 0;

		// ģ��ж��
		virtual SS_Error shutdown() = 0;

		bool is_available() { return _module_description.available; };

	private:
		MODULE_DESCRIPTION _module_description;   // ģ����Ϣ
	};

}

#endif // _I_MODULE_H_