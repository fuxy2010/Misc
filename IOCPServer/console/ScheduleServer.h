// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _SCHEDULE_SERVER_H_       
#define _SCHEDULE_SERVER_H_

#include "stdafx.h"
#include "GeneralDef.h"
#include <boost/pool/detail/singleton.hpp>//#include "singleton.h"
#include "Logger.h"
#include "MiscTool.h"
#include "ConfigBox.h"
#include "IDataBaseProxy.h"
#include "TaskThread.h"
#include "ProcessInfo.h"
#include "MaintanceThread.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#define TQ_ID	52428

//�Ƿ��ڻ�����������Ӳ������ֵ�UA
#ifndef PLAY_MUSIC
#define PLAY_MUSIC
#define MUSIC_UA_ID			0xFFFFFFFF
#define MUSIC_PACKET_SIZE	960
#endif

//���ƶ��ն�UAÿ��������಻����200
#ifndef MAX_TEMP_UA_NUM
#define MAX_TEMP_UA_NUM	200
#endif

namespace ScheduleServer
{
	class CScheduleServer
	{
	public:
		CScheduleServer();

		~CScheduleServer();

		SS_Error start(std::string path);

		SS_Error shutdown();

		void write_log(std::string& log, int level, bool show_on_screen);

		void wait_for_shutdown();

		void set_cur_path(const std::string& path) { _cur_path = path; };

		std::string get_cur_path() { return _cur_path; };

		IDataBaseProxy* get_db_proxy() { return _db_proxy; };

		//RTSPServerLib::CRTSPServer* get_rtsp_server() { return _rtsp_server; };

		void singleton_test();

		//��������
		void enable() { _enalble = true; }

		//�رշ���
		void disable()
		{
			_enalble = false;
		}

	public:
		//����RTSP�ͻ��˷��͵����ݰ�
		static void CALLBACK on_recv_rtsp_client_data(const char* data, const unsigned long& length);

		static void CALLBACK on_recv_rtp_packet(const unsigned char* data, const unsigned long& length,
											const unsigned short& sequence, const unsigned long& timestamp,
											const unsigned long& ssrc, const unsigned char&payload_type, const bool& mark);
		static void CALLBACK on_recv_rtcp_packet();

	private:
		void console_command(const std::vector<std::string>& cmd);

		//����̨����
		void test();

	public:
		//��������
		//indexΪ0�������ѡ�������̣߳�����ѡ��indexģ�����߳���ȡ��������߳�
		SS_Error add_task(CTask* task, unsigned long index);

	public:
		CMaintanceThread _maintance_thread;//ά���߳�

		//����CPUռ���ʼ��ڴ�ռ��ͳ��
		CProcessInfo _process_info;

	private:
		/*volatile */bool _enalble;//�����Ƿ����

		//���������ļ��о���·��
		std::string _cur_path;

		IDataBaseProxy* _db_proxy;//SQL Server���ݿ����

		//CRTPRecvSession** _rtp_recv_session;//ý�����RTP Session��ָ������
		//unsigned short _rtp_recv_thread_num;//ý�����RTP Session������
		//unsigned short _rtp_recv_base_port;//ý�����RTP Session�ļ�����׼�˿�

		//static unsigned short _audio_payload_type;
		//static unsigned short _video_payload_type;

		//RTSPServerLib::CRTSPServer* _rtsp_server;

		//CRTPNATSession* _audio_send_session;//������Ƶ���ݵ�RTP session
		//CRTPNATSession* _video_send_session;//������Ƶ���ݵ�RTP session

	public:
		static std::string _ver;
	};
}

#endif//_SCHEDULE_SERVER_H_