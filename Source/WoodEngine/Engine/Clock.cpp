#include "..\stdafx.h"

#include<sstream>
#include<string>

#include "Clock.hpp"
#include "..\Engine\DevConsole.hpp"

namespace woodman
{
	// static functions
	//

	Clock* Clock::p_MasterClock = nullptr;

	LARGE_INTEGER g_Frequency;
	LARGE_INTEGER g_CurrentClockFrame, g_PrevClockFrame;
	double ClockMasterDeltaTime = 0.0;
	double ClockMasterTotalTime = 0.0;
	//---------------------------------------------------------------------------------------------
	
	void Clock::InitializeTimeSystem( EventSystem* eventSystem )
	{
		QueryPerformanceFrequency(&g_Frequency);
		QueryPerformanceCounter(&g_CurrentClockFrame);
		g_PrevClockFrame = g_CurrentClockFrame;
		p_MasterClock = new Clock(HashedString("Master Clock"), eventSystem  );
	}

	Clock* Clock::MasterClock()
	{
		return p_MasterClock;
	}


	//---------------------------------------------------------------------------------------------

	Clock::Clock(const HashedString& name, EventSystem* eventsystem) 
		:m_currentTime(0.), 
		m_timeScale(1.), 
		m_lastTimeDelta(0.),
		m_paused(false),
		m_name(name),
		m_maxTimeDelta( 1. ),
		EventRecipient(eventsystem)
	{
		registerForEvents();
	}

	//---------------------------------------------------------------------------------------------

	Clock::Clock( const HashedString& name, double timeSacle, EventSystem* eventsystem )
		:m_currentTime(0.), 
		m_timeScale(timeSacle),
		m_name(name),
		m_lastTimeDelta(0.),
		m_paused(false),
		m_maxTimeDelta( 1. ),
		EventRecipient(eventsystem)
	{
		if(p_MasterClock == nullptr)
			InitializeTimeSystem(eventsystem);

		m_parent = p_MasterClock;
		m_parent->addChild(this);
		m_currentTime = m_parent->m_currentTime;
		registerForEvents();
	}

	//---------------------------------------------------------------------------------------------

	Clock::Clock( const HashedString& name, double timeSacle, Clock* parent )
		: m_currentTime(0.),
		m_name(name),
		m_timeScale(timeSacle), 
		m_lastTimeDelta(0.),
		m_parent( parent ),
		m_paused(false),
		m_maxTimeDelta( 1. ),
		EventRecipient(parent->p_eventSystem)
	{
		parent->addChild(this);
		registerForEvents();
	}
	
	//---------------------------------------------------------------------------------------------

	Clock::~Clock()
	{
		if(m_parent)
		{
			while( !m_children.empty() )
			{
				(*m_children.begin())->setParent(m_parent);
			}
		}

		m_parent->removeChild(this);
	}

	//---------------------------------------------------------------------------------------------

	void Clock::registerForEvents()
	{
		p_eventSystem->RegisterObjectForEvent( this, &Clock::catchRollCall, "ClockOut"); 
		p_eventSystem->RegisterObjectForEvent( this, &Clock::catchSetTimeScale, "SetTimeScale"); 
	}
	
	//---------------------------------------------------------------------------------------------

	double Clock::getTimeSeconds()
	{
		return m_currentTime;
	}

	//---------------------------------------------------------------------------------------------

	double Clock::getLastTimeDeltaSeconds()
	{
		return m_lastTimeDelta;
	}
	//---------------------------------------------------------------------------------------------

	void Clock::setParent( Clock* newParent)
	{
		if( m_parent != nullptr)
		{
			m_parent->removeChild(this);
		}
		m_parent = newParent;
		m_parent->addChild(this);
	}

	//---------------------------------------------------------------------------------------------

	void Clock::addChild(Clock* childToAdd )
	{
		m_children.insert(childToAdd);
	}

	//---------------------------------------------------------------------------------------------

	void Clock::removeChild(Clock* childToRemove )
	{
		std::set<Clock*>::iterator it = m_children.find(childToRemove);

		if(it != m_children.end() )
		{
			m_children.erase(it);
		}
	}

	//---------------------------------------------------------------------------------------------

	void Clock::advanceTime(double deltaTime )
	{
		if(m_paused)
		{
			m_lastTimeDelta = 0;
		}
		else
		{
			m_lastTimeDelta = deltaTime * m_timeScale;
		}

		if( m_lastTimeDelta > m_maxTimeDelta)
			m_lastTimeDelta = m_maxTimeDelta;

		m_currentTime += m_lastTimeDelta;

		while( !m_alarms.empty() && m_currentTime > (*m_alarms.begin())->m_timeToCall )
		{
			p_eventSystem->FireEvent( (*m_alarms.begin())->m_EventToFire.m_string, (*m_alarms.begin() )->m_parameters );
			m_alarms.erase(m_alarms.begin());
		}

		std::set<Clock*>::iterator it;
		for( it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->advanceTime(m_lastTimeDelta);
		}
	}

	//---------------------------------------------------------------------------------------------

	void Clock::catchRollCall( NamedPropertyContainer& parameters)
	{

		DevConsole* console;
		parameters.getNamedData(HASHED_STRING_Console, console);

		std::stringstream ss;
		ss << m_name.m_string << ": " << m_currentTime;

		console->postMessage( MESSAGE_TYPE_INFO, ss.str() );
	}

	//---------------------------------------------------------------------------------------------

	void Clock::catchSetTimeScale( NamedPropertyContainer& parameters)
	{
		DevConsole* console;
		parameters.getNamedData(HASHED_STRING_Console, console);

		std::string name;
		parameters.getNamedData(HASHED_STRING_Name, name);
		
		if(_stricmp(name.c_str(), m_name.m_string.c_str() ) != 0)
			return;


		std::string timescale;
		parameters.getNamedData(HASHED_STRING_Scale, timescale);


		m_timeScale = atof(timescale.c_str() );
		std::stringstream ss;
		ss << m_name.m_string << ": setting time scale - " << timescale;
		console->postMessage( MESSAGE_TYPE_INFO, ss.str() );
	}

	//---------------------------------------------------------------------------------------------

	void Clock::setName( HashedString name)
	{
		m_name = name;
	}

	//---------------------------------------------------------------------------------------------

	void Clock::setAlarm(std::shared_ptr<Alarm> newAlarm)
	{
		newAlarm->m_timeToCall += m_currentTime; // set alarms time to synch with the clock
		m_alarms.insert(newAlarm);
	}

	//---------------------------------------------------------------------------------------------

	void Clock::getTimeRemainingOnAlarmSeconds(HashedString alarmName )
	{
		//auto it = m_
	}

	//---------------------------------------------------------------------------------------------

	void Clock::update()
	{
		QueryPerformanceCounter(&g_CurrentClockFrame);
		ClockMasterDeltaTime = (float) ( (double)(g_CurrentClockFrame.QuadPart - g_PrevClockFrame.QuadPart) / (double)g_Frequency.QuadPart );
		ClockMasterTotalTime += ClockMasterDeltaTime;
		g_PrevClockFrame = g_CurrentClockFrame;
		
		advanceTime(ClockMasterDeltaTime);
	}

	//---------------------------------------------------------------------------------------------

	double Clock::GetAbsoluteTimeSeconds()
	{
		QueryPerformanceCounter(&g_CurrentClockFrame);
		double delta = (float) ( (double)(g_CurrentClockFrame.QuadPart - g_PrevClockFrame.QuadPart) / (double)g_Frequency.QuadPart );

		return ClockMasterTotalTime + delta;
	}
}