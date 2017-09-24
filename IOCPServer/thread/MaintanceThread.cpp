// **********************************************************************
// ����: FYM
// �汾: 1.0
// ����: 2010-01 ~ 2010-03
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#include "GeneralDef.h"
#include "MaintanceThread.h"
#include "ScheduleServer.h"
#include "ParseTask.h"

using namespace ScheduleServer;

//string CMaintanceThread::_imsi = "";
//string CMaintanceThread::_latitude = "";
//string CMaintanceThread::_longitude = "";
unsigned short CMaintanceThread::_listen_port = 30001;
//unsigned long CMaintanceThread::_restart_interval = 180000;

//////////////////////////////////////////////////////////////////////////
/**
 * �ṹ�����ƣ�PER_IO_DATA
 * �ṹ�幦�ܣ��ص�I/O��Ҫ�õ��Ľṹ�壬��ʱ��¼IO����
 **/
const int DataBuffSize  = 2 * 1024;
typedef struct
{
	OVERLAPPED overlapped;
	WSABUF databuff;
	char buffer[ DataBuffSize ];
	int BufferLen;
	int operationType;
}PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA, *LPPER_IO_DATA, PER_IO_DATA;

/**
 * �ṹ�����ƣ�PER_HANDLE_DATA
 * �ṹ��洢����¼�����׽��ֵ����ݣ��������׽��ֵı������׽��ֵĶ�Ӧ�Ŀͻ��˵ĵ�ַ��
 * �ṹ�����ã��������������Ͽͻ���ʱ����Ϣ�洢���ýṹ���У�֪���ͻ��˵ĵ�ַ�Ա��ڻطá�
 **/
typedef struct
{
	SOCKET socket;
	SOCKADDR_STORAGE ClientAddr;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// ����ȫ�ֱ���
//const int DefaultPort = 30001;		
vector < PER_HANDLE_DATA* > clientGroup;		// ��¼�ͻ��˵�������

HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

//HANDLE _iocp_main_thread_handle = ::CreateEvent(NULL, TRUE/*���Զ���λ*/, FALSE/*��ʼ״̬*/, NULL);
//CSSMutex _iocp_mutext;//���뻥����
//bool _restarting_iocp_server = false;
//int _iocp_server_work_thread_num = 0;

void parse_gps(char* content, int length1, int length2)
{
	string latitude_str = "";
	string longitude_str = "";
	string id = "";

	cout << "\n\r" << MiscTools::parse_now_to_string() << "=========================== " << length1 << " " << length2 << endl;
	//LOG_WRITE(MiscTools::parse_now_to_string() << "Recv: " << content, 1, false);

	if('*' == content[0])
	{
		string msg = content;
		cout << "Recv Txt: " << msg << " length: " << msg.length() << endl;

		if('*' != msg.at(0) || '#' != msg[msg.length() - 1])
		{
			return;
		}

		id = msg.substr(msg.find(',') + 1, msg.find("V1") - msg.find(',') - 2);

		//"*HQ,2141028712,V1,105147,A,3036.5622,N,11411.1614,E,000.00,254,081214,FFFFFBFF#"
		string::size_type begin = msg.find("V1");
		begin += 12;//"V1,105147,A,"

		string::size_type end = string::npos;
		if(string::npos != msg.find_last_of("E"))
		{
			end = msg.find_last_of("E") - 1;
		}
		else if(string::npos != msg.find_last_of("W"))
		{
			end = msg.find_last_of("W") - 1;
		}

		string gps = msg.substr(begin, end - begin);
		latitude_str = gps.substr(0, gps.find_first_of(","));
		longitude_str = gps.substr(gps.find_last_of(",") + 1, gps.length() - gps.find_last_of(","));
	}
	else if('$' == content[0])
	{
		cout << "Recv Binary: " << content << endl;

		char temp[8];
		::memset(temp, 0, sizeof(temp));
		unsigned char c;

		//id
		c = *(content + 0x01);
		sprintf_s(temp, "%.2x",c);
		id += string(temp);

		c = *(content + 0x02);
		sprintf_s(temp, "%.2x",c);
		id += string(temp);

		c = *(content + 0x03);
		sprintf_s(temp, "%.2x",c);
		id += string(temp);

		c = *(content + 0x04);
		sprintf_s(temp, "%.2x",c);
		id += string(temp);

		c = *(content + 0x05);
		sprintf_s(temp, "%.2x",c);
		id += string(temp);

		//latitude
		c = *(content + 0x0C);
		sprintf_s(temp, "%.2x",c);
		latitude_str += string(temp);

		c = *(content + 0x0D);
		sprintf_s(temp, "%.2x",c);
		latitude_str += string(temp);

		latitude_str += ".";

		c = *(content + 0x0E);
		sprintf_s(temp, "%.2x",c);
		latitude_str += string(temp);

		c = *(content + 0x0F);
		sprintf_s(temp, "%.2x",c);
		latitude_str += string(temp);

		//longitude
		c = *(content + 0x11);
		sprintf_s(temp, "%.2x",c);
		longitude_str += string(temp);

		c = *(content + 0x12);
		sprintf_s(temp, "%.2x",c);
		longitude_str += string(temp);

		c = *(content + 0x13);
		sprintf_s(temp, "%.2x",c);
		longitude_str += string(temp);

		c = *(content + 0x14);
		sprintf_s(temp, "%.2x",c);
		longitude_str += string(temp);

		c = *(content + 0x15);
		sprintf_s(temp, "%.2x",c);
		longitude_str += string(temp);

		if(5 <= longitude_str.length())
		{
			longitude_str = longitude_str.substr(0, longitude_str.length() - 1);
			longitude_str.insert(longitude_str.length() - 4, ".");
		}
		else
		{
			return;
		}

		/*for(int i = 0; i < 32; i++)
		{
			//cout << content[i] << " ";
			printf("[%x, %.2x, %xx] ", i, content[i], content[i]);
		}*/
	}

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

	cout << id << " GPS str: [" << latitude_str << ", " << longitude_str << "]" << endl;
	LOG_WRITE(MiscTools::parse_now_to_string() << " GPS str: [" << latitude_str << ", " << longitude_str << "]", 1, false);


	char temp[16];
	::memset(temp, 0, sizeof(temp));
	sprintf_s(temp, "%.8f", latitude_d);
	latitude_str = temp;
	::memset(temp, 0, sizeof(temp));
	sprintf_s(temp, "%.8f", longitude_d);
	longitude_str = temp;


	cout << "GPS double: <" << latitude_str << ", " << longitude_str << ">" << endl;
	LOG_WRITE(MiscTools::parse_now_to_string() << " GPS double: <" << latitude_str << ", " << longitude_str << ">", 1, false);

	/*string sql = "update RealTimePosition set CollectionTime='" + MiscTools::parse_now_to_string() +"', ";
	sql += "UpdateTime='" + MiscTools::parse_now_to_string() + "', ";
	sql += "Latitude='" + latitude_str + "', ";
	sql += "Longitude='" + longitude_str + "' ";
	sql += "where IMSI='" + CMaintanceThread::_imsi + "'";

	SS_CTX ctx;
	ctx.clear();
	SINGLETON(CScheduleServer).get_db_proxy()->query_record(sql, ctx);

	cout << "Update GPS " << ctx["success"] << ": " << sql << endl;*/
}

// ��ʼ�������̺߳���
DWORD WINAPI ServerWorkThread(LPVOID IpParam)
{
	HANDLE CompletionPort = (HANDLE)IpParam;
	DWORD BytesTransferred;
	LPOVERLAPPED IpOverlapped;
	LPPER_HANDLE_DATA PerHandleData = NULL;
	LPPER_IO_DATA PerIoData = NULL;
	DWORD RecvBytes;
	DWORD Flags = 0;
	BOOL bRet = false;

	while(true)//false == _restarting_iocp_server)
	{
		bRet = GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&PerHandleData, (LPOVERLAPPED*)&IpOverlapped, INFINITE);
		if(bRet == 0)
		{
			cerr << "GetQueuedCompletionStatus Error: " << GetLastError() << endl;
			break;
		}
		PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(IpOverlapped, PER_IO_DATA, overlapped);

		// ������׽������Ƿ��д�����
		if(0 == BytesTransferred){
			closesocket(PerHandleData->socket);
			GlobalFree(PerHandleData);
			GlobalFree(PerIoData);
			break;
		}

		// ��ʼ���ݴ����������Կͻ��˵�����
		WaitForSingleObject(hMutex,INFINITE);
		//////////////////////////////////////////////////////////////////////////
		//cout << "A Client says: " << PerIoData->databuff.buf << endl;
#if 0
		parse_gps(PerIoData->databuff.buf, PerIoData->databuff.len, PerIoData->BufferLen);
#else
		PARSE_TASK_INFO task_info;
		task_info.task_id = timeGetTime();
		task_info.msg = new char[PerIoData->databuff.len];
		memcpy(task_info.msg, PerIoData->databuff.buf, PerIoData->databuff.len);
		task_info.length = PerIoData->databuff.len;
		
		CParseTask* task = new CParseTask(task_info);

		if(SS_NoErr != SINGLETON(CScheduleServer).add_task(task, task_info.task_id))
		{
			delete task;
			task = NULL;
		}
#endif
		//////////////////////////////////////////////////////////////////////////
		ReleaseMutex(hMutex);

		// Ϊ��һ���ص����ý�����I/O��������
		ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED)); // ����ڴ�
		PerIoData->databuff.len = 1024;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->operationType = 0;	// read
		WSARecv(PerHandleData->socket, &(PerIoData->databuff), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
	}

	{
		//CSSLocker lock(&_iocp_mutext);
		//--_iocp_server_work_thread_num;
	}
	return 0;
}


// ������Ϣ���߳�ִ�к���
DWORD WINAPI ServerSendThread(LPVOID IpParam)
{
	while(0)
	{
		char talk[200];
		gets(talk);
		int len;
		for (len = 0; talk[len] != '\0'; ++len){
			// �ҳ�����ַ���ĳ���
		}
		talk[len] = '\n';
		talk[++len] = '\0';
		printf("I Say:");
		cout << talk;
		WaitForSingleObject(hMutex, INFINITE);
		for(int i = 0; i < clientGroup.size(); ++i){
			send(clientGroup[i]->socket, talk, 200, 0);	// ������Ϣ
		}
		ReleaseMutex(hMutex); 
	}

	int count = 3;
	while(--count)
	{
		//string msg = "*HQ,2141028712,S17,003800,60#";
		string msg = "*HQ,0000000000,D1,004800,60,1#";//string msg = "*HQ,2141028712,D1,004800,60,1#";
		WaitForSingleObject(hMutex,INFINITE);
		for(int i = 0; i < clientGroup.size(); ++i)
		{
			send(clientGroup[i]->socket, msg.c_str(), msg.length(), 0);	// ������Ϣ
			cout << "--------- send " << msg << endl;
		}
		Sleep(5000);
		ReleaseMutex(hMutex); 
	}
	return 0;
}

// ��ʼ������
DWORD WINAPI IOCPThread(LPVOID IpParam)
{
// ����socket��̬���ӿ�
	WORD wVersionRequested = MAKEWORD(2, 2); // ����2.2�汾��WinSock��
	WSADATA wsaData;	// ����Windows Socket�Ľṹ��Ϣ
	DWORD err = WSAStartup(wVersionRequested, &wsaData);

	if (0 != err){	// ����׽��ֿ��Ƿ�����ɹ�
		cerr << "Request Windows Socket Library Error!\n";
		system("pause");
		return -1;
	}
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){// ����Ƿ�����������汾���׽��ֿ�
		WSACleanup();
		cerr << "Request Windows Socket Version 2.2 Error!\n";
		system("pause");
		return -1;
	}

// ����IOCP���ں˶���
	/**
	 * ��Ҫ�õ��ĺ�����ԭ�ͣ�
	 * HANDLE WINAPI CreateIoCompletionPort(
     *    __in   HANDLE FileHandle,		// �Ѿ��򿪵��ļ�������߿վ����һ���ǿͻ��˵ľ��
     *    __in   HANDLE ExistingCompletionPort,	// �Ѿ����ڵ�IOCP���
     *    __in   ULONG_PTR CompletionKey,	// ��ɼ���������ָ��I/O��ɰ���ָ���ļ�
     *    __in   DWORD NumberOfConcurrentThreads // ��������ͬʱִ������߳�����һ���ƽ���CPU������*2
     * );
	 **/
	HANDLE completionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == completionPort){	// ����IO�ں˶���ʧ��
		cerr << "CreateIoCompletionPort failed. Error:" << GetLastError() << endl;
		system("pause");
		return -1;
	}

// ����IOCP�߳�--�߳����洴���̳߳�

	// ȷ���������ĺ�������
	SYSTEM_INFO mySysInfo;
	GetSystemInfo(&mySysInfo);

	// ���ڴ������ĺ������������߳�
	//_restarting_iocp_server = false;
	{
		//CSSLocker lock(&_iocp_mutext);

		for(DWORD i = 0; i < (mySysInfo.dwNumberOfProcessors * 2); ++i)
		{
			// �����������������̣߳�������ɶ˿ڴ��ݵ����߳�
			HANDLE ThreadHandle = CreateThread(NULL, 0, ServerWorkThread, completionPort, 0, NULL);

			if(NULL == ThreadHandle)
			{
				cerr << "Create Thread Handle failed. Error:" << GetLastError() << endl;
				system("pause");
				return -1;
			}
			CloseHandle(ThreadHandle);

			//++_iocp_server_work_thread_num;
		}
	}
	

// ������ʽ�׽���
	SOCKET srvSocket = socket(AF_INET, SOCK_STREAM, 0);

// ��SOCKET������
	SOCKADDR_IN srvAddr;
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(CMaintanceThread::_listen_port);
	int bindResult = bind(srvSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));
	if(SOCKET_ERROR == bindResult)
	{
		cerr << "Bind failed. Error:" << GetLastError() << endl;
		system("pause");
		return -1;
	}

// ��SOCKET����Ϊ����ģʽ
	int listenResult = listen(srvSocket, 10);
	if(SOCKET_ERROR == listenResult){
		cerr << "Listen failed. Error: " << GetLastError() << endl;
		system("pause");
		return -1;
	}
	
// ��ʼ����IO����
	cout << "IOCP server is ready!" << endl;;

	// �������ڷ������ݵ��߳�
	HANDLE sendThread = CreateThread(NULL, 0, ServerSendThread, 0, 0, NULL);

	unsigned long start = timeGetTime();
	while(true)
	{
		PER_HANDLE_DATA * PerHandleData = NULL;
		SOCKADDR_IN saRemote;
		int RemoteLen;
		SOCKET acceptSocket;

		// �������ӣ���������ɶˣ����������AcceptEx()
		RemoteLen = sizeof(saRemote);
		acceptSocket = accept(srvSocket, (SOCKADDR*)&saRemote, &RemoteLen);
		if(SOCKET_ERROR == acceptSocket){	// ���տͻ���ʧ��
			cerr << "Accept Socket Error: " << GetLastError() << endl;
			break;
		}
		
		// �����������׽��ֹ����ĵ����������Ϣ�ṹ
		PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));	// �ڶ���Ϊ���PerHandleData����ָ����С���ڴ�
		PerHandleData -> socket = acceptSocket;
		memcpy (&PerHandleData -> ClientAddr, &saRemote, RemoteLen);
		clientGroup.push_back(PerHandleData);		// �������ͻ�������ָ��ŵ��ͻ�������

		// �������׽��ֺ���ɶ˿ڹ���
		CreateIoCompletionPort((HANDLE)(PerHandleData -> socket), completionPort, (DWORD)PerHandleData, 0);

		
		// ��ʼ�ڽ����׽����ϴ���I/Oʹ���ص�I/O����
		// ���½����׽�����Ͷ��һ�������첽
		// WSARecv��WSASend������ЩI/O������ɺ󣬹������̻߳�ΪI/O�����ṩ����	
		// ��I/O��������(I/O�ص�)
		LPPER_IO_OPERATION_DATA PerIoData = NULL;
		PerIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATEION_DATA));
		ZeroMemory(&(PerIoData -> overlapped), sizeof(OVERLAPPED));
		PerIoData->databuff.len = 1024;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->operationType = 0;	// read

		DWORD RecvBytes;
		DWORD Flags = 0;
		WSARecv(PerHandleData->socket, &(PerIoData->databuff), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);

		/*if(CMaintanceThread::_restart_interval <= (timeGetTime() - start))
		{
			break;
		}*/
	}

	//_restarting_iocp_server = true;

	return 0;
}
//////////////////////////////////////////////////////////////////////////

void CMaintanceThread::entry()
{
	unsigned long start = timeGetTime();//�����߳�ѭ��ִ����ʼʱ��

#if 0
	if(true == MiscTools::time_is_up(2000, _performance_statistics_timestamp))
	{
		/*//��¼��������Դռ��ͳ����Ϣ
		SINGLETON(CScheduleServer)._process_info.performance_statistics();

		_performance_statistics_timestamp = timeGetTime();

		RTSPServerLib::CRTSPServer* server = SINGLETON(CScheduleServer).get_rtsp_server();

		if(NULL != server)
		{
			server->query_relay_source();
		}*/

		_performance_statistics_timestamp = timeGetTime();

		//string imsi = SINGLETON(CConfigBox).get_property("IMSI", "52428");
		//string latitude = SINGLETON(CConfigBox).get_property("LATITUDE", "30.524493393377888");
		//string longitude = SINGLETON(CConfigBox).get_property("LONGITUDE", "114.36252885448127");

		_sql = "select PersonID from RealTimePosition where IMSI = '" + _imsi + "'";

		_sql_ctx.clear();
		SINGLETON(CScheduleServer).get_db_proxy()->query_record(_sql, _sql_ctx);

		if("true" != _sql_ctx["success"])//insert
		{
			_sql = "insert into RealTimePosition (IMSI, PersonID, PersonName, UserNumber, RightLevel, PersonType, OrgID, OrgName, Accuracy, Height, Speed, GPSType, TerminalRegistrationStatus, Longitude, Latitude, CollectionTime, UpdateTime)";
			_sql += " values ('52428', '222111', '����', '13811111111', '1', '1', '7c43e87f-7ee8-4148-bdba-5613d27fb8c4', 'һ���', '100', '100', '0', '1', '2', ";
			_sql += "'" + _longitude + "', '" + _latitude + "', ";
			//_sql += "'114.36252885448127', '30.524493393377888', ";
			_sql += "'" + MiscTools::parse_now_to_string() + "', '" + MiscTools::parse_now_to_string() + "')";
		}
		else//update
		{
			cout << "<" << _latitude << ", " << _longitude << ">" << endl;
			_latitude = MiscTools::parse_type_to_string<double>(MiscTools::parse_string_to_type<double>(_latitude) + 0.0001);//((double)(rand() % 1000 ) / 10000));
			_longitude = MiscTools::parse_type_to_string<double>(MiscTools::parse_string_to_type<double>(_longitude) + 0.0005);//((double)(rand() % 1000) / 10000));
			cout << "[" << _latitude << ", " << _longitude << "]" << endl;
			_sql = "update RealTimePosition set CollectionTime='" + MiscTools::parse_now_to_string() +"', ";
			_sql += "UpdateTime='" + MiscTools::parse_now_to_string() + "', ";
			_sql += "Latitude='" + _latitude + "', ";
			_sql += "Longitude='" + _longitude + "' ";
			_sql += "where IMSI='" + _imsi + "'";

			if(!(_times++ % 20))
			{
				_latitude = SINGLETON(CConfigBox).get_property("LATITUDE", "30.524493393377888");
				_longitude = SINGLETON(CConfigBox).get_property("LONGITUDE", "114.36252885448127");
			}
		}

		std::cout << "execute sql: " << _sql << endl;

		_sql_ctx.clear();	
		SINGLETON(CScheduleServer).get_db_proxy()->execute_sql(_sql, _sql_ctx);

		std::cout << "result: " << _sql_ctx["success"] << endl;

		{
			_sql = "update TerminalStatus set LastRegisterTime='" + MiscTools::parse_now_to_string() +"', TerminalRegistrationStatus='2' ";
			_sql += "where IMSI='" + _imsi + "'";

			_sql_ctx.clear();	
			SINGLETON(CScheduleServer).get_db_proxy()->execute_sql(_sql, _sql_ctx);
		}
	}
#else
	/*string sql = "update TerminalStatus set LastRegisterTime='" + MiscTools::parse_now_to_string() +"', TerminalRegistrationStatus='2' ";
	sql += "where IMSI='" + _imsi + "'";

	SS_CTX ctx;
	ctx.clear();	
	SINGLETON(CScheduleServer).get_db_proxy()->execute_sql(sql, ctx);*/
	/*{
		//CSSLocker lock(&_iocp_mutext);

		if(!_iocp_server_work_thread_num && true == _restarting_iocp_server)
		{
			HANDLE handle = CreateThread(NULL, 0, IOCPThread, 0, 0, NULL);
			if(NULL == handle)
			{
				cerr << "Create IOCP Thread Handle failed. Error:" << GetLastError() << endl;
				system("pause");
				return;
			}
			CloseHandle(handle);

			cout << "+++++++++++++++++++++++++++ new iocp main thread!" << endl;
		}

	}*/
#endif

	if(timeGetTime() == start)
		Sleep(1);
}

void CMaintanceThread::on_start()
{
	/*_ua_statistics_timestamp = 0;

	_performance_statistics_timestamp = 0;

	_check_sip_server_timestamp = 0;

	//_imsi = SINGLETON(CConfigBox).get_property("IMSI", "52428");
	//_latitude = SINGLETON(CConfigBox).get_property("LATITUDE", "30.524493393377888");
	//_longitude = SINGLETON(CConfigBox).get_property("LONGITUDE", "114.36252885448127");
	//_times = 0;

	_listen_port = MiscTools::parse_string_to_type<unsigned short>(SINGLETON(CConfigBox).get_property("TCPPort", "30001"));
	//_restart_interval = MiscTools::parse_string_to_type<unsigned long>(SINGLETON(CConfigBox).get_property("RestarInterval", "180000"));

	//_restarting_iocp_server = false;

	HANDLE handle = CreateThread(NULL, 0, IOCPThread, 0, 0, NULL);
	if(NULL == handle)
	{
		cerr << "Create IOCP Thread Handle failed. Error:" << GetLastError() << endl;
		system("pause");
		return;
	}
	CloseHandle(handle);*/
}


void CMaintanceThread::on_close()
{
	_ua_statistics_timestamp = 0;

	_performance_statistics_timestamp = 0;

	_check_sip_server_timestamp = 0;
}
