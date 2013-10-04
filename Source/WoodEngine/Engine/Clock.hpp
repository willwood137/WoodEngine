#pragma once
#include <vector>
#include <set>

#include "EventSystem.hpp"
#include "HashedString.hpp"

namespace woodman
{
	class Clock : public EventRecipient<Clock>
	{

	public:
		struct Alarm
		{
			HashedString m_name;
			double m_timeToCall;
			HashedString m_EventToFire;
			NamedPropertyContainer m_parameters;
		};

		struct AlarmPointerCompare
		{
			bool operator()( const std::shared_ptr<Alarm> lhs, const std::shared_ptr<Alarm> rhs ) const
			{
				return lhs->m_timeToCall < rhs->m_timeToCall;
			}
		};

		Clock(const HashedString& name, EventSystem* eventsystem);
		Clock( const HashedString& name, double timeSacle, EventSystem* eventsystem );
		Clock( const HashedString& name, double timeScale, Clock* parent );
		~Clock();

		double getTimeScale(){ return m_timeScale;}
		void registerForEvents();
		void advanceTime(double deltaTime );
		void setPaused(bool pause);
		void setTimeScale(double timeScale);
		void setParent( Clock* newParent);
		void removeChild(Clock* childToRemove );
		void addChild(Clock* childToAdd );
		void setName(HashedString name);
		void update();

		void setAlarm(std::shared_ptr<Alarm> newAlarm);
		void getTimeRemainingOnAlarmSeconds(HashedString alarmName );

		double getTimeSeconds();
		double getLastTimeDeltaSeconds();

		void catchRollCall( NamedPropertyContainer& parameters);
		void catchSetTimeScale( NamedPropertyContainer& parameters);

		//static functions
		static double GetAbsoluteTimeSeconds();
		static void InitializeTimeSystem( EventSystem* eventsystem);
		static Clock* MasterClock();

	private:

		Clock();
		static Clock* p_MasterClock;

		HashedString m_name;


		double m_currentTime;
		double m_lastTimeDelta;
		double m_maxTimeDelta;
		double m_timeScale;

		Clock* m_parent;
		std::set< Clock* > m_children;

		std::multiset<std::shared_ptr<Alarm>, AlarmPointerCompare > m_alarms;

		bool m_paused;
	};
}