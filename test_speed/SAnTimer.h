#ifndef SAN_TIMER
#define SAN_TIMER

/*
	Copyright 2008 Sukhinov Anton
	E-mail: Soukhinov@gmail.com

	Version 1.0

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cassert>
#include <windows.h>

class CFrequency; //Forward-����������� ������ CFrequency

class CTimer
{ //����� CTimer ��������� ����� ������� �������, ������� ����� ��������� � ����������
public:
	typedef long long tTime;

private:
	static CFrequency frequency; //������� ������� ��� �������� tTime � �������
	
	bool state; //���� true, �� ������ �������. ����� -- ���
	
	tTime timeTotal;    //������� ������� �������� �� ��� ���������� ���������
	tTime timeLastStart; //������ ������ ����������, ��� �������������� ��������� (���� �������)
	
	static inline tTime Now(void); //������ ������� �������� ������� � "�����"

public:
	inline CTimer(bool start = false); //����������� ��-���������
	inline CTimer(CTimer const &T);    //����������� �����
	inline ~CTimer(void);              //����������
	inline CTimer &operator=(CTimer const &T); //�������� ������������

	inline double Get(void) const; //������ ������� �������� ������� � ��������
	inline tTime GetTickCount(void) const; //������ ������� �������� ������� � "�����" (������������ ��������)
	static inline tTime Frequency(void); //������ ������� ������� (����� "�����" � �������)

	inline void Start(void); //��������� ������
	inline double StartGet(void); //��������� ������ � ������ �������� �� ������ �������

	inline void Pause(void); //����� �������
	inline double PauseGet(void); //����� ������� � ������� ��������

	inline void Stop(void); //��������� �������. ���������� �� ����� ���������� ��������
	inline double StopGet(void); //��������� �������. ������������ �������� �� ������ ���������, �� ���������
};

//***********************************************************************************************************

class CFrequency
{ //����� ��� ��������� � �������� ������� �������
	friend class CTimer;
public:
	typedef CTimer::tTime tTime;

private:
	tTime value; //������� ������� ��� �������� tTime � �������

	inline CFrequency(CFrequency const &); //������� ����������� �����
	inline CFrequency &operator=(CFrequency const &); //������� �������� ������������
public:
	inline CFrequency(void); //����������� ��-���������
	inline ~CFrequency(void); //����������
};

//***********************************************************************************************************

CFrequency CTimer::frequency; //������� ������� ��� �������� tTime � �������; �������� �������

inline CTimer::tTime CTimer::Now(void)
{ //������ ������� �������� ������� � "�����"
	CTimer::tTime timeNow;
	BOOL const res( QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER *>(&timeNow) ) );
	assert( res );
	return timeNow;
}

inline CTimer::CTimer(bool const start): state(false),
                                         timeTotal(0),
                                         timeLastStart(0)
{ //����������� ��-���������
	if(start) Start();
} 

inline CTimer::CTimer(CTimer const &T): state(T.state),
                                        timeTotal(T.timeTotal),
                                        timeLastStart(T.timeLastStart)
{ //����������� �����
	;
}

inline CTimer::~CTimer(void)
{ //����������
	;
}

inline CTimer &CTimer::operator=(CTimer const &T)
{ //�������� ������������
	state         = T.state;
	timeTotal     = T.timeTotal;
	timeLastStart = T.timeLastStart;
	return *this;
}

inline double CTimer::Get(void) const
{ //������ ������� �������� ������� � ��������
	if(state) //������ �����
		return static_cast<double>( timeTotal + ( Now() - timeLastStart ) ) / frequency.value;
	else //������ ����������
		return static_cast<double>( timeTotal ) / frequency.value;
}

inline CTimer::tTime CTimer::GetTickCount(void) const
{ //������ ������� �������� ������� � "�����"
	if(state) //������ �����
		return timeTotal + ( Now() - timeLastStart );
	else //������ ����������
		return timeTotal;
}

inline CTimer::tTime CTimer::Frequency(void)
{ //������ ������� ������� (����� "�����" � �������)
	return frequency.value;
}

inline void CTimer::Start(void)
{ //��������� ������
	assert( !state );
	state = true;
	timeLastStart = Now(); //������ �������
}

inline double CTimer::StartGet(void)
{ //��������� ������ � ������ �������� �� ������ �������
	assert( !state );
	double const res( Get() ); //����� �����. ����� ������� �� �������� ������, ������� Get() �� ������
	state = true;
	timeLastStart = Now(); //������ �������
	return res;
}

inline void CTimer::Pause(void)
{ //����� �������
	assert( state );
	state = false;
	timeTotal += Now()-timeLastStart; //����� �������
}

inline double CTimer::PauseGet(void)
{ //����� ������� � ������� ��������
	assert( state );
	state = false;
	timeTotal += Now()-timeLastStart; //����� �������
	return Get();
}

inline void CTimer::Stop(void)
{ //��������� �������. ���������� �� ����� ���������� ��������
	assert( state );
	state = false;
	timeTotal = 0; //��������� �������
}

inline double CTimer::StopGet(void)
{ //��������� �������. ������������ �������� �� ������ ���������, �� ���������
	assert( state );
	state = false;
	timeTotal += Now()-timeLastStart; //����� �������
	double const res( Get() ); //����� ������� ���������� ����� �� ������ ���������
	timeTotal = 0; //������� ���������� �����
	return res; //��������� ����� �� ���������
}

//***********************************************************************************************************

inline CFrequency::CFrequency(void): value(0)
{ //����������� ��-���������
	if( !QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER *>(&value) ) ) value = 0;
	assert( value );
}

inline CFrequency::~CFrequency(void)
{ //����������
	;
}

#endif //#ifndef SAN_TIMER