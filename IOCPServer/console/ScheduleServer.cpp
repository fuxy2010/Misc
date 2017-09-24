// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
// ScheduleServer.cpp : Defines the entry point for the console application.
//
#include "ScheduleServer.h"
#include "MySQLProxy.h"//#include "SQLServerProxy.h"
#include "json.h"
#include "TimeConsuming.h"

using namespace ScheduleServer;

double CTimeConsuming::_clock_frequency = 0;

std::string CScheduleServer::_ver = "20131018";

//unsigned short CScheduleServer::_audio_payload_type = 96;
//unsigned short CScheduleServer::_video_payload_type = 97;

CScheduleServer::CScheduleServer() :
_db_proxy(NULL),
_cur_path(""),
#ifdef SINGLE_CONFERENCE_THREAD
_conference_thread(NULL),
#endif
_enalble(false)//,
//_rtp_recv_session(NULL),
//_audio_send_session(NULL),
//_video_send_session(NULL),
//_rtp_recv_thread_num(0),
//_rtp_recv_base_port(0)//,
//_rtsp_server(NULL)
{
}

CScheduleServer::~CScheduleServer()
{
	if(NULL != _db_proxy)
	{
		_db_proxy->shutdown();
		delete _db_proxy;
		_db_proxy = NULL;
	}
}

SS_Error CScheduleServer::start(std::string path)
{
	double d = MiscTools::parse_string_to_type<double>("123.123456789");
	string t = MiscTools::parse_type_to_string<double>(d);
	//////////////////////////////////////////////////////////////////////////
	/*for (float y = 1.5f; y > -1.5f; y -= 0.1f) {
		for (float x = -1.5f; x < 1.5f; x += 0.05f) {
			float a = x * x + y * y - 1;
			putchar(a * a * a - x * x * y * y * y <= 0.0f ? '*' : ' ');
		}
		putchar('\n');
	}
	for (float y = 1.5f; y > -1.5f; y -= 0.1f) {
		for (float x = -1.5f; x < 1.5f; x += 0.05f) {
			float z = x * x + y * y - 1;
			float f = z * z * z - x * x * y * y * y;
			putchar(f <= 0.0f ? ".:-=+*#%@"[(int)(f * -8.0f)] : ' ');
		}
		putchar('\n');
	}*/
	if(false)
	{
		char t = 0xe;
		char temp[8];
		sprintf(temp, "[%2x, %.2x, %xx] ", t, t, t);
		cout << temp;
		//string msg = "*HQ,2141028712,V1,105147,A,3036.5622,N,11411.1614,E,000.00,254,081214,FFFFFBFF#";
		string msg = "*HQ,2140912323,V1,000526,V,2217.0532,N,11333.4555,E,000.00,000,060180,FFE7FBFF#";
		if('*' == msg.at(0) && '#' == msg[msg.length() - 1])
		{
			string id = msg.substr(msg.find(',') + 1, msg.find("V1") - msg.find(',') - 2);
			//char tempchar = temp[1];
			string::size_type begin = msg.find("V1");
			begin += 12;//"V1,105147,A,"

			string gps = msg.substr(begin, msg.length() - begin);
			
			string::size_type end = string::npos;
			if(string::npos != gps.find_first_of("E"))
			{
				end = msg.find_first_of("E") - 1;
			}
			else if(string::npos != msg.find_first_of("W"))
			{
				end = msg.find_first_of("W") - 1;
			}

			gps = msg.substr(begin, end - begin);
			string latitude_str = gps.substr(0, gps.find_first_of(","));
			string longitude_str = gps.substr(gps.find_last_of(",") + 1, gps.length() - gps.find_last_of(","));

			//string decicmal;
			//string integer;
			//integer = latitude_str.substr(0, latitude_str.find(".") - 2);
			//decicmal = latitude_str.substr(latitude_str.find(".") - 2, latitude_str.length() - latitude_str.find(".") + 2);
			//double latitude_d = MiscTools::parse_string_to_type<double>(integer);
			//latitude_d += MiscTools::parse_string_to_type<double>(decicmal) / 59.9999;
			double latitude_d = MiscTools::parse_string_to_type<double>(latitude_str.substr(0, latitude_str.find(".") - 2));
			latitude_d += MiscTools::parse_string_to_type<double>(latitude_str.substr(latitude_str.find(".") - 2, latitude_str.length() - latitude_str.find(".") + 2)) / 59.9999;

			double longitude_d = MiscTools::parse_string_to_type<double>(longitude_str.substr(0, longitude_str.find(".") - 2));
			longitude_d += MiscTools::parse_string_to_type<double>(longitude_str.substr(longitude_str.find(".") - 2, longitude_str.length() - longitude_str.find(".") + 2)) / 59.9999;

			cout << gps << endl;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//有效期
	time_t now = ::time(NULL);
	struct tm time;
	::localtime_s(&time, &now);

	//if(false == (2014 == time.tm_year + 1900 && 1 == time.tm_mon + 1 && 30 > time.tm_mday))
	//if(2014 != time.tm_year + 1900)
	if(false == ((2014 == time.tm_year + 1900 && 12 == time.tm_mon + 1) || (2015 == time.tm_year + 1900 && 1 == time.tm_mon + 1)))
	{
		//std::cout << "超过有效期" << endl;
		//return SS_NoErr;
	}

	//设置当前程序路径
	_cur_path = path;

	//根据系统时间设置随机数
	::srand(timeGetTime());

	//初始化socket
	//for JRTP
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);

	//获取系统信息
	SYSTEM_INFO theSystemInfo;
	::GetSystemInfo(&theSystemInfo);

	//创建数据库代理////////////////////////////////////////////////////////////////////////
	DB_SERVER_INFO db_info;
	::ZeroMemory(&db_info, sizeof(DB_SERVER_INFO));

	db_info.server_ip = SINGLETON(CConfigBox).get_property("SQLServerIP", "127.0.0.1");
	db_info.server_port = SINGLETON(CConfigBox).get_property("SQLServerPort", "1433");
	db_info.db_name = SINGLETON(CConfigBox).get_property("SQLServerDBName", "PISDB");
	db_info.db_username = SINGLETON(CConfigBox).get_property("SQLServerUserName", "sa");
	db_info.db_password = SINGLETON(CConfigBox).get_property("SQLServerPassword", "123");

#if 1//1-normal, 0-test locally
	_db_proxy = new CMySQLProxy();//new CSQLServerProxy();
	_db_proxy->set_connection_info(db_info);
	if(SS_NoErr != _db_proxy->start())
	{
		std::cout << "<FAIL> Server fails in starting mysql proxy!" << endl;;//LOG_WRITE("<FAIL> Server fails in starting sql server proxy!", 1, true);
		//无法启动DB也启动服务 return SS_ServerStartFail;
	}
	else
	{
		std::cout << "Server succeed in starting mysql proxy!" << endl;//LOG_WRITE("Server succeed in starting sql server proxy!", 1, true);
	}
#endif

	//启动维护线程
	_maintance_thread.start(this);

	//启动RTP接收会话////////////////////////////////////////////////////////////////////////
	/*_rtp_recv_thread_num = !(theSystemInfo.dwNumberOfProcessors) ? 1 : theSystemInfo.dwNumberOfProcessors;
	_rtp_recv_thread_num = (1 == _rtp_recv_thread_num) ? 2 : _rtp_recv_thread_num;//必须确保有两个以上的RTP接收线程以分开音视频接收，在CVirtualUA::invite_ua中发送的音视频端口必须不同
	_rtp_recv_base_port = MiscTools::parse_string_to_type<unsigned short>(SINGLETON(CConfigBox).get_property("RTPRecvBasePort", "30000"));

	_rtp_recv_session = new CRTPRecvSession*[_rtp_recv_thread_num];

	for(unsigned short i = 0; i < _rtp_recv_thread_num; ++i)
	{
		_rtp_recv_session[i] = new CRTPRecvSession(_rtp_recv_base_port + 2 * i);
		_rtp_recv_session[i]->set_rtp_callback(CScheduleServer::on_recv_rtp_packet);
	}

	_audio_payload_type = MiscTools::parse_string_to_type<unsigned short>(SINGLETON(CConfigBox).get_property("AudioPayloadType", "96"));
	_video_payload_type = MiscTools::parse_string_to_type<unsigned short>(SINGLETON(CConfigBox).get_property("VideoPayloadType", "97"));

	//启动RTP发送会话
	unsigned short media_send_port = MiscTools::parse_string_to_type<unsigned short>(SINGLETON(CConfigBox).get_property("MediaSendBindPort", "12580"));
	_audio_send_session = new CRTPNATSession(media_send_port);
	//_audio_send_session->set_udp_callback(CScheduleServer::on_recv_nat_probe);
	_video_send_session = new CRTPNATSession(media_send_port + 10);
	//_video_send_session->set_udp_callback(CScheduleServer::on_recv_nat_probe);
	
	//初始化RTSPServer
	//_rtsp_server = new RTSPServerLib::CRTSPServer(MiscTools::parse_string_to_type<unsigned short>(SINGLETON(CConfigBox).get_property("RTSPPort", "1554")));
	//_rtsp_server->set_data_callback(CScheduleServer::on_recv_rtsp_client_data);*/

	//启动任务线程////////////////////////////////////////////////////////////////////////
	CTaskThreadPool::add_threads((!(theSystemInfo.dwNumberOfProcessors) ? 1 : theSystemInfo.dwNumberOfProcessors), this);

	_enalble = true;//服务可用

	return SS_NoErr;
}

SS_Error CScheduleServer::shutdown()
{
	_enalble = false;

	//关闭任务线程////////////////////////////////////////////////////////////////////////
	CTaskThreadPool::remove_threads();

	LOG_WRITE("Task threads have been removed!", 1, true);

	/*//关闭RTSPServer
	//delete _rtsp_server;
	//_rtsp_server = NULL;

	//关闭RTP发送会话////////////////////////////////////////////////////////////////////////
	delete _audio_send_session;
	_audio_send_session = NULL;
	delete _video_send_session;
	_video_send_session = NULL;

	//关闭RTP接收会话////////////////////////////////////////////////////////////////////////
	for(unsigned short i = 0; i < _rtp_recv_thread_num; ++i)
	{
		delete _rtp_recv_session[i];
		_rtp_recv_session[i] = NULL;
	}

	delete[] _rtp_recv_session;
	_rtp_recv_session = NULL;*/

	//关闭维护线程
	_maintance_thread.shutdown();

	//关闭数据库代理////////////////////////////////////////////////////////////////////////
	if(NULL != _db_proxy)
		_db_proxy->shutdown();

	//关闭socket////////////////////////////////////////////////////////////////////////
	WSACleanup();//for JRTP

	return SS_NoErr;
}

void CScheduleServer::wait_for_shutdown()
{
	//LOG_WRITE("CScheduleServer::wait_for_shutdown()", 1, false);

	// 循环处理命令行输入
	std::string input_str("");
	while (1)
	{
		getline(cin, input_str);

		input_str = MiscTools::trim_string(input_str);

		if (input_str.empty())
			continue;

		//LOG_WRITE("<console command> " + input_str, 1, false);

		if("quit" == input_str || "exit" == input_str)
			return;

		//处理控制台命令
		std::vector<std::string> cmd;
		MiscTools::split_string(input_str, " ", cmd);

		if(cmd.size())
		{
			console_command(cmd);
		}

	}

}

void CScheduleServer::write_log(std::string& log, int level, bool show_on_screen)
{
	SINGLETON(CLogger).trace_out(log, level, DEFAULT_TRACE_TYPE, show_on_screen);
}

void CScheduleServer::console_command(const std::vector<std::string>& cmd)
{
	//cout << "\ncommand: " << cmd << endl;
	if(true == MiscTools::compare_string_ignore_case(cmd[0], "test") ||
		true == MiscTools::compare_string_ignore_case(cmd[0], "t"))//测试
	{
		test();
	}
}

//data为含包头的RTP包，length为净荷加包头长度
void CScheduleServer::on_recv_rtp_packet(const unsigned char* data, const unsigned long& length,
	const unsigned short& sequence, const unsigned long& timestamp,
	const unsigned long& ssrc, const unsigned char& payload_type, const bool& mark)
{
	//std::cout << "Got packet " << payload_type << " from SSRC " << ssrc << " length " << length << " sequence " << sequence << std::endl;
	//unsigned long start = timeGetTime();
	//cout << "<R " << timeGetTime() - start << "> ";
}

void CScheduleServer::on_recv_rtcp_packet()
{

}

//data为去掉前四个字节的数据包，length为去掉前四个字节后的数据包长度
void CScheduleServer::on_recv_rtsp_client_data(const char* data, const unsigned long& length)
{
	/*RTPHeader* header = reinterpret_cast<RTPHeader*>(const_cast<char*>(data));

	on_recv_rtp_packet(reinterpret_cast<unsigned char*>(const_cast<char*>(data)), length,
					ntohs(header->sequencenumber),
					ntohl(header->timestamp), 
					ntohl(header->ssrc),
					header->payloadtype, header->marker);*/
}

void CScheduleServer::singleton_test()
{
	cout << "<ScheduleServer: " << reinterpret_cast<unsigned long>(this) << "> ";
}

void CScheduleServer::test()
{
}

void convert_number(unsigned long number, unsigned short radix)
{
	char c64[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',//无l，o
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',//无I，O
				'$', '%', '*', '#', '&', '@'};

	string str("");

	 while(number)
	 {
		 str.insert(0, 1, c64[number % radix]);
		 number /= radix;
	 }

	 cout << str << endl;
}

void sleep_test(unsigned long interval)
{
	LARGE_INTEGER litmp;
	LONGLONG quad_part1, quad_part2;
	double dif_minus, freq, dif_sec;

	QueryPerformanceFrequency(&litmp);
	freq = (double)litmp.QuadPart;// 获得计数器的时钟频率

	QueryPerformanceCounter(&litmp);
	quad_part1 = litmp.QuadPart;// 获得初始值

	Sleep(interval);

	QueryPerformanceCounter(&litmp);
	quad_part2 = litmp.QuadPart;//获得中止值

	dif_minus = (double)(quad_part2 - quad_part1);
	dif_sec = dif_minus / freq;// 获得对应的时间值，单位为秒

	cout << "sleep " << dif_sec * 1000 << endl;
}

void cout_test()
{
	LARGE_INTEGER litmp;
	LONGLONG quad_part1, quad_part2;
	double dif_minus, freq, dif_sec;

	QueryPerformanceFrequency(&litmp);
	freq = (double)litmp.QuadPart;// 获得计数器的时钟频率

	QueryPerformanceCounter(&litmp);
	quad_part1 = litmp.QuadPart;// 获得初始值

	cout << "<F1, 2> ";

	QueryPerformanceCounter(&litmp);
	quad_part2 = litmp.QuadPart;//获得中止值

	dif_minus = (double)(quad_part2 - quad_part1);
	dif_sec = dif_minus / freq;// 获得对应的时间值，单位为秒

	cout << "cout " << dif_sec * 1000 << endl;
}

SS_Error CScheduleServer::add_task(CTask* task, unsigned long index)
{
	if(false == _enalble)
		return SS_AddTaskFail;

	if(NULL  == task)
		return SS_InvalidTask;

	CTaskThread* task_thread = CTaskThreadPool::select_thread(index);

	if(NULL == task_thread)
		return SS_AddTaskFail;

	if(SS_NoErr != task_thread->add_task(task))
	{
		return SS_AddTaskFail;
	}

	return SS_NoErr;
}
