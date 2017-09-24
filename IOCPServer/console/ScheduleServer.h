// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
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

//是否在会议混音中增加播放音乐的UA
#ifndef PLAY_MUSIC
#define PLAY_MUSIC
#define MUSIC_UA_ID			0xFFFFFFFF
#define MUSIC_PACKET_SIZE	960
#endif

//非移动终端UA每种数量最多不超过200
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

		//开启服务
		void enable() { _enalble = true; }

		//关闭服务
		void disable()
		{
			_enalble = false;
		}

	public:
		//接收RTSP客户端发送的数据包
		static void CALLBACK on_recv_rtsp_client_data(const char* data, const unsigned long& length);

		static void CALLBACK on_recv_rtp_packet(const unsigned char* data, const unsigned long& length,
											const unsigned short& sequence, const unsigned long& timestamp,
											const unsigned long& ssrc, const unsigned char&payload_type, const bool& mark);
		static void CALLBACK on_recv_rtcp_packet();

	private:
		void console_command(const std::vector<std::string>& cmd);

		//控制台命令
		void test();

	public:
		//增加任务
		//index为0则是随机选择任务线程，否则选择index模任务线程数取余的任务线程
		SS_Error add_task(CTask* task, unsigned long index);

	public:
		CMaintanceThread _maintance_thread;//维护线程

		//进程CPU占用率及内存占用统计
		CProcessInfo _process_info;

	private:
		/*volatile */bool _enalble;//服务是否可用

		//程序所在文件夹绝对路径
		std::string _cur_path;

		IDataBaseProxy* _db_proxy;//SQL Server数据库代理

		//CRTPRecvSession** _rtp_recv_session;//媒体接收RTP Session的指针数组
		//unsigned short _rtp_recv_thread_num;//媒体接收RTP Session的数量
		//unsigned short _rtp_recv_base_port;//媒体接收RTP Session的监听基准端口

		//static unsigned short _audio_payload_type;
		//static unsigned short _video_payload_type;

		//RTSPServerLib::CRTSPServer* _rtsp_server;

		//CRTPNATSession* _audio_send_session;//发送音频数据的RTP session
		//CRTPNATSession* _video_send_session;//发送音频数据的RTP session

	public:
		static std::string _ver;
	};
}

#endif//_SCHEDULE_SERVER_H_