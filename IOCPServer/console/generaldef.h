// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2009-01 ~ 2009-03
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _GENERAL_DEF_H_
#define _GENERAL_DEF_H_

//��������ΪVC�Ұ汾������VC6.0ʱ�����ļ��ڱ���ʱֻ������һ��
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "stdafx.h"

#ifdef _DEBUG
//#include "..\\utility\\VLD1.9H\\vld.h"
#endif

#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <set>

//#ifndef	PERFORMANCE_ANALYSIS
//#define	PERFORMANCE_ANALYSIS
//#endif

#ifndef _SENDING_DATA_BLOCK_SIZE_
#define _SENDING_DATA_BLOCK_SIZE_	1400
#endif

namespace ScheduleServer
{
	//�������������Ͷ���
	typedef std::map<std::string, std::string> SS_CTX;

	//�豸���Ͷ���
	typedef enum
	{
		SIPStack = 0,	//SIP Server
		Unknown			//δ֪
	}Device_Type;

	typedef struct tagTIME_INFO
	{
		unsigned long year;		//[1900,--]
		unsigned long month;	//[1, 12]
		unsigned long day;		//[1, 31]
		unsigned long hour;		//[0, 23]
		unsigned long minute;	//[0, 59]
		unsigned long second;	//[0, 59]

		tagTIME_INFO() : year(0), month(0), day(0), hour(0), minute(0), second(0)
		{
		}
	}
	TIME_INFO;

    //����ֵ���Ͷ���
    typedef enum
    {
        SS_NoErr = 0,						//�������óɹ�

		//����������
		SS_ServerStartFail,					//����������ʧ��

		//���ݿ����
		SS_InvalidDBConnection,				//���ݿ�����ʧЧ
		SS_UserNameOrPasswdError,			//�û������������
		SS_ExecFail,						//ִ��SQL���ʧ��
		SS_QueryFail,						//��ѯ��¼ʧ��

		//ͨ��ģ��
		SS_CreateServerFail,				//����ͨѶ�������ʧ��
		SS_InvalidClientConnection,			//�ͻ��˲����Ӳ�����
		SS_SendSyncSignalFail,				//�ͻ��˲෢��ͬ������ʧ��
		SS_SendAsyncSignalFail,				//�ͻ��˲෢���첽����ʧ��
		SS_SendSyncDataFail,				//�ͻ��˲෢��ͬ������ʧ��
		SS_SendAsyncDataFail,				//�ͻ��˲෢���첽����ʧ��

		//�豸������ģ�����
		SS_LoadDeviceAccessModuleFail,		//����ʧ��
		SS_UnloadDeviceAccessModuleFail,	//ж��ʧ��

		//��ȡ�豸������ģ��ָ��
		SS_GetDeviceAccessModuleFail,		//��ȡ�豸������ָ��ʧ��

		//UA����
		SS_InsertUAFail,					//����UAʧ��
		SS_InsertMediaDataFail,				//����ý�����ݵ�����ʧ��

		//SIPStack����
		SS_ConnectSIPStackFail,				//����SIPStackʧ��
		SS_DisconnectSIPStackFail,				//�Ͽ�SIPStackʧ��

		//Ӳ���豸Ѳ��
		SS_DeviceInspectionFail,			//Ӳ���豸Ѳ��ʧ��

		//ϵͳ�����߳�
		SS_StartConferenceThreadFail,		//���������߳�ʧ��

		//�����߳�
		SS_StartTaskThreadFail,			//���������߳�ʧ��
		SS_AddTaskFail,					//�������ʧ��

		//�������
		SS_InvalidTask,					//��������

		//��ѯ����ִ�н��
		SS_QueryTaskResultFail,			//��ѯ����ִ�н��ʧ��

		//��������
		SS_SyncTaskFail,					//��������ʧ��

		//�¼����ݲ���
		SS_FetchUserAgentFail,				//��ȡ�¼�����ʧ��

		//RTP����
		SS_RTPSendSessionUnAvailable,		//RTP����Session������
		SS_RTPRecvSessionUnAvailable,		//RTP����Session������
		SS_AddRTPHeaderFail,				//���RTP��ͷʧ��
		SS_SendPacketFail,					//����RTP��ʧ��

		//�������
		SS_ConferenceControlFail,			//�������ʧ��
		SS_UnknownParticipant,				//δ֪�������
		SS_ParticipantsExisted,				//�ظ���������Ա

		//SIP��Ϣ
		SS_ErrorSIPMessage,					//�����SIP��Ϣ

		//����UA
		SS_StartVirtualUAThreadFail,					//��������UA�߳�ʧ��


        SS_Unknown_Error					//δ֪����    
    }SS_Error;

    /*typedef struct 
    {
        SS_Error ret_code;
		std::string message;
    }SS_Error_Info;

	const std::string DEFAULT_SS_ERROR_MESSAGE = "δ֪����";

    const SS_Error_Info global_ds_error_info_list[] = {
        {SS_NoErr,						"���óɹ�"},
        {SS_InvalidDBConnection,			"�û������������"},
        {SS_ExecFail,					"ִ��SQL���ʧ��"},
		{SS_QueryFail,					"��ѯ��¼ʧ��"},

        {SS_Unknown_Error,				"δ֪����"}
    };*/

	//�߳�����
	typedef enum
	{
		CONFERENCE_THREAD = 0,//�����߳�
		TASK_THREAD,//�����߳�
		SIGNAL_THREAD,//SIP������߳�
		COMMUNICATOR_THREAD,//ICE������߳�
		WEB_THREAD,//Web��ѯ��Ӧ�߳�
		MAINTANCE_THREAD//ά���߳�
	}
	THREAD_TYPE;

	//�����Ա����///////////////////////////////////////////////////////////////////////
	//�����Ա״̬
	/*typedef enum
	{
		UnInvited = 0,//��δ���䷢������
		Inviting,//������
		Answered,//�������룬�����
		Rejected,//�ܾ�����
		Exception,//�쳣�˳�
		Quit,//�����˳�������/���޳���
		UnRegister//δ�Ǽǵ���������
	}
	PARTICIPANT_STATUS;*/

	//�����Ա��ɫ
	typedef enum
	{
		Speaker = 0,//������
		Audience,//����
		Observer//�Թ��ߣ��������������޳���
	}
	PARTICIPANT_ROLE;

	//��������
	typedef enum
	{
		Common_Conference = 0,//��ͨ����
		SOS_Conference,//SOS����
		PC_Conference//PC�������
	}
	CONFERENCE_TYPE;

	//RTP������
	typedef enum
	{
		AMRNBRTPPacket = 0,//��Ƶ��
		PCMRTPPacket,//��Ƶ��
		H264RTPPacket,//��Ƶ��
		UnknownRTPPacket//δ֪
	}
	RTP_PACKET_TYPE;

	//UA״̬,ֻ����VUA���ͻ����յ���SIP��Ϣʱ����!!!
	typedef enum
	{
		//����
		UA_STATUS_IDLE = 0,
		//���л򱻽���
		UA_STATUS_CALLING,
		//ͨ����
		UA_STATUS_IN_CALL
	}
	USER_AGENT_STATUS;

	//���ݻ����������UAִ�еĶ���
	typedef enum
	{
		//ʲô������
		UA_ACTION_NOTHING = 0,
		//����
		UA_ACTION_CALL,
		//���º���
		UA_ACTION_RECALL,
		//�Ҷϻ��������
		UA_ACTION_HANG_UP
	}
	USER_AGENT_ACTION;

	//UA����
	typedef enum
	{
		UA_MobilePhone = 0,//�ֻ�
		UA_Radio,//350M��̨
		UA_FixedPhone,//�̻�
		UA_Unknown//δ֪
	}
	USER_AGENT_TYPE;

#ifndef PC_UA_BASE_ID
#define PC_UA_BASE_ID 0xC0000000
#endif

#ifndef VIRTUAL_UA_BASE_ID
#define VIRTUAL_UA_BASE_ID 0xD0000000
#endif

#ifndef RADIO_UA_BASE_ID
#define RADIO_UA_BASE_ID 0xE0000000
#endif

#ifndef PHONE_UA_BASE_ID
#define PHONE_UA_BASE_ID 0xF0000000
#endif

	//UA��������
	typedef enum
	{
		UA_NETWORK_TYPE_0 = 0,//GPRS
		UA_NETWORK_TYPE_1,//EDGE/WIFI
		UA_NETWORK_TYPE_2//HSDPA/HSUPA/3G
	}
	USER_AGENT_NETWORK_TYPE;

	//UA����״̬
	typedef enum
	{
		UA_NETWORK_STATUS_0 = 0,//����
		UA_NETWORK_STATUS_1,//�ϲ�
		UA_NETWORK_STATUS_2//����
	}
	USER_AGENT_NETWORK_STATUS;
}

#endif // _GENERAL_DEF_H_
