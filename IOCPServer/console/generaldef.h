// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2009-01 ~ 2009-03
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
#ifndef _GENERAL_DEF_H_
#define _GENERAL_DEF_H_

//当编译器为VC且版本不低于VC6.0时，本文件在编译时只被包含一次
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
	//参数上下文类型定义
	typedef std::map<std::string, std::string> SS_CTX;

	//设备类型定义
	typedef enum
	{
		SIPStack = 0,	//SIP Server
		Unknown			//未知
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

    //返回值类型定义
    typedef enum
    {
        SS_NoErr = 0,						//函数调用成功

		//服务器启动
		SS_ServerStartFail,					//服务器启动失败

		//数据库代理
		SS_InvalidDBConnection,				//数据库连接失效
		SS_UserNameOrPasswdError,			//用户名或密码错误
		SS_ExecFail,						//执行SQL语句失败
		SS_QueryFail,						//查询记录失败

		//通信模块
		SS_CreateServerFail,				//创建通讯器服务侧失败
		SS_InvalidClientConnection,			//客户端侧链接不可用
		SS_SendSyncSignalFail,				//客户端侧发送同步信令失败
		SS_SendAsyncSignalFail,				//客户端侧发送异步信令失败
		SS_SendSyncDataFail,				//客户端侧发送同步数据失败
		SS_SendAsyncDataFail,				//客户端侧发送异步数据失败

		//设备接入插件模块加载
		SS_LoadDeviceAccessModuleFail,		//加载失败
		SS_UnloadDeviceAccessModuleFail,	//卸载失败

		//获取设备接入插件模块指针
		SS_GetDeviceAccessModuleFail,		//获取设备接入插件指针失败

		//UA操作
		SS_InsertUAFail,					//插入UA失败
		SS_InsertMediaDataFail,				//插入媒体数据到队列失败

		//SIPStack接入
		SS_ConnectSIPStackFail,				//连接SIPStack失败
		SS_DisconnectSIPStackFail,				//断开SIPStack失败

		//硬件设备巡检
		SS_DeviceInspectionFail,			//硬件设备巡检失败

		//系统会议线程
		SS_StartConferenceThreadFail,		//启动会议线程失败

		//任务线程
		SS_StartTaskThreadFail,			//启动任务线程失败
		SS_AddTaskFail,					//添加任务失败

		//任务队列
		SS_InvalidTask,					//错误任务

		//查询任务执行结果
		SS_QueryTaskResultFail,			//查询任务执行结果失败

		//更新任务
		SS_SyncTaskFail,					//更新任务失败

		//事件数据操作
		SS_FetchUserAgentFail,				//获取事件数据失败

		//RTP传输
		SS_RTPSendSessionUnAvailable,		//RTP发送Session不可用
		SS_RTPRecvSessionUnAvailable,		//RTP接收Session不可用
		SS_AddRTPHeaderFail,				//添加RTP包头失败
		SS_SendPacketFail,					//发送RTP包失败

		//会议控制
		SS_ConferenceControlFail,			//会议控制失败
		SS_UnknownParticipant,				//未知的与会人
		SS_ParticipantsExisted,				//重复添加与会人员

		//SIP消息
		SS_ErrorSIPMessage,					//错误的SIP消息

		//虚拟UA
		SS_StartVirtualUAThreadFail,					//启动虚拟UA线程失败


        SS_Unknown_Error					//未知错误    
    }SS_Error;

    /*typedef struct 
    {
        SS_Error ret_code;
		std::string message;
    }SS_Error_Info;

	const std::string DEFAULT_SS_ERROR_MESSAGE = "未知错误";

    const SS_Error_Info global_ds_error_info_list[] = {
        {SS_NoErr,						"调用成功"},
        {SS_InvalidDBConnection,			"用户名或密码错误"},
        {SS_ExecFail,					"执行SQL语句失败"},
		{SS_QueryFail,					"查询记录失败"},

        {SS_Unknown_Error,				"未知错误"}
    };*/

	//线程类型
	typedef enum
	{
		CONFERENCE_THREAD = 0,//会议线程
		TASK_THREAD,//任务线程
		SIGNAL_THREAD,//SIP信令处理线程
		COMMUNICATOR_THREAD,//ICE信令发送线程
		WEB_THREAD,//Web查询响应线程
		MAINTANCE_THREAD//维护线程
	}
	THREAD_TYPE;

	//与会人员参数///////////////////////////////////////////////////////////////////////
	//与会人员状态
	/*typedef enum
	{
		UnInvited = 0,//还未向其发出邀请
		Inviting,//邀请中
		Answered,//接收邀请，与会中
		Rejected,//拒绝邀请
		Exception,//异常退出
		Quit,//正常退出（主动/被剔除）
		UnRegister//未登记到本服务器
	}
	PARTICIPANT_STATUS;*/

	//与会人员角色
	typedef enum
	{
		Speaker = 0,//发言人
		Audience,//听众
		Observer//旁观者，例如曾经与会后被剔除者
	}
	PARTICIPANT_ROLE;

	//会议类型
	typedef enum
	{
		Common_Conference = 0,//普通会议
		SOS_Conference,//SOS会议
		PC_Conference//PC发起会议
	}
	CONFERENCE_TYPE;

	//RTP包类型
	typedef enum
	{
		AMRNBRTPPacket = 0,//音频包
		PCMRTPPacket,//音频包
		H264RTPPacket,//视频包
		UnknownRTPPacket//未知
	}
	RTP_PACKET_TYPE;

	//UA状态,只能在VUA发送或处理收到的SIP消息时更改!!!
	typedef enum
	{
		//空闲
		UA_STATUS_IDLE = 0,
		//呼叫或被叫中
		UA_STATUS_CALLING,
		//通话中
		UA_STATUS_IN_CALL
	}
	USER_AGENT_STATUS;

	//根据会议任务需对UA执行的动作
	typedef enum
	{
		//什么都不做
		UA_ACTION_NOTHING = 0,
		//呼叫
		UA_ACTION_CALL,
		//重新呼叫
		UA_ACTION_RECALL,
		//挂断或放弃呼叫
		UA_ACTION_HANG_UP
	}
	USER_AGENT_ACTION;

	//UA类型
	typedef enum
	{
		UA_MobilePhone = 0,//手机
		UA_Radio,//350M手台
		UA_FixedPhone,//固话
		UA_Unknown//未知
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

	//UA网络类型
	typedef enum
	{
		UA_NETWORK_TYPE_0 = 0,//GPRS
		UA_NETWORK_TYPE_1,//EDGE/WIFI
		UA_NETWORK_TYPE_2//HSDPA/HSUPA/3G
	}
	USER_AGENT_NETWORK_TYPE;

	//UA网络状态
	typedef enum
	{
		UA_NETWORK_STATUS_0 = 0,//极差
		UA_NETWORK_STATUS_1,//较差
		UA_NETWORK_STATUS_2//正常
	}
	USER_AGENT_NETWORK_STATUS;
}

#endif // _GENERAL_DEF_H_
