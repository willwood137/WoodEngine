#pragma once
#ifndef PROFILESECTION_H
#define PROFILESECTION_H

#include <vector>
#include "Clock.hpp"

namespace woodman
{
	enum PROFILE_CATEGORY
	{
		PROFILE_CATEGORY_FRAME,
		PROFILE_CATEGORY_RENDER,
		PROFILE_CATEGORY_GAME,
		PROFILE_CATEGORY_SIZE
	};


	struct ProfileSection
	{
		double m_totalTime;
		unsigned int m_numEntries;
	};

	ProfileSection Sections[PROFILE_CATEGORY_SIZE];

	class Profiler
	{
	public:
		
		Profiler( PROFILE_CATEGORY category) : m_category(category) 
		{
			m_timeBeganSeconds = Clock::GetAbsoluteTimeSeconds();
		}

		~Profiler()
		{
			Sections[m_category].m_numEntries++;
			Sections[m_category].m_totalTime += Clock::GetAbsoluteTimeSeconds() - m_timeBeganSeconds;
		}

	private:
		Profiler() {}

		double m_timeBeganSeconds;
		PROFILE_CATEGORY m_category;
	};

	void setupProfileSections()
	{
		for(size_t i = 0; i < PROFILE_CATEGORY_SIZE; i++)
		{
			Sections[i].m_numEntries =0;
			Sections[i].m_totalTime =0;
		}
	}
	void clearAllProfileSections()
	{
		for(size_t i = 0; i < PROFILE_CATEGORY_SIZE; i++)
		{
			Sections[i].m_numEntries =0;
			Sections[i].m_totalTime =0;
		}
	}
	void clearProfileSection(PROFILE_CATEGORY category)
	{
		Sections[category].m_numEntries = 0;
		Sections[category].m_totalTime = 0;
	
	}

	double getAverageTimeofProfileSection( PROFILE_CATEGORY category)
	{
		return Sections[category].m_totalTime / Sections[category].m_numEntries;
	}

	double getTotalTimeofProfileSection( PROFILE_CATEGORY category)
	{
		return Sections[category].m_totalTime;
	}
}

#endif