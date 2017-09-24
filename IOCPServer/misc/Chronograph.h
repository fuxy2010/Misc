// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _CHRONOGRAPH_H_       
#define _CHRONOGRAPH_H_

//΢�뼶��ʱ��
class CChronograph
{
public:
	CChronograph(unsigned long initial_timestamp, unsigned long interval)	:
	_timestamp(initial_timestamp),
	_interval(interval)
	{
	}

	virtual ~CTimeConsuming() {}

public:
	bool time_is_up()
	{
		unsigned long cur_timestamp = timeGetTime();

		//��ֹtimeGetTime()����ֵ����ֵ�����޺����
		unsigned long durance = (_timestamp > cur_timestamp) ? (0xFFFFFFFF - _timestamp + cur_timestamp) : (cur_timestamp - _timestamp);

		return (_interval <= durance);
	}

	void set_timestamp(unsigned long timestamp)
	{
		_timestamp = timestamp;
	}

	void set_timestamp()
	{
		_timestamp = timeGetTime();
	}

private:
	unsigned _timestamp;
	unsigned _interval;
};

#endif