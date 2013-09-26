#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "HashedString.hpp"

namespace woodman
{

	enum NamedPropertyResult
	{
		NAMED_PROPERTY_RESULT_EMPTY_CONTAINER = 0,
		NAMED_PROPERTY_RESULT_MISMATCH_DATA_TYPE,
		NAMED_PROPERTY_RESULT_NAME_NOT_FOUND,
		NAMED_PROPERTY_RESULT_DATA_FOUND,
		NAMED_PROPERTY_RESULT_SIZE,
	};

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	struct NamedPropertyBase
	{
		virtual ~NamedPropertyBase() {}
	};


	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	struct NamedProperty : public NamedPropertyBase
	{
		NamedProperty( T_ObjType data) : m_data(data) {}
		T_ObjType m_data;
	};

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	class NamedPropertyContainer
	{
		
		std::map<HashedString, std::shared_ptr<NamedPropertyBase> > m_dataContainer;

		//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	public:

		template<typename T_ObjType>
		void insertNamedData(const HashedString hashedName, const T_ObjType& inData )
		{
			std::shared_ptr< NamedProperty<T_ObjType> > prop = std::make_shared< NamedProperty<T_ObjType> >( NamedProperty<T_ObjType>( inData ) );
			m_dataContainer[hashedName] = std::dynamic_pointer_cast<NamedPropertyBase>(prop);
		}

		//----------------------------------------------------------------------------------------------------------------------------------------------------------------

		template<typename T_ObjType>
		NamedPropertyResult getNamedData(const HashedString hashedName, T_ObjType& outData)
		{
			if(m_dataContainer.empty() )
				return NAMED_PROPERTY_RESULT_EMPTY_CONTAINER;

			std::map<HashedString, std::shared_ptr<NamedPropertyBase> >::iterator dataItr = m_dataContainer.find( hashedName );
			if( dataItr != m_dataContainer.end() )
			{
				// result found
				 std::shared_ptr<NamedProperty<T_ObjType> > property = std::dynamic_pointer_cast< NamedProperty<T_ObjType> >( (dataItr->second) );
				 if( property != nullptr )
				 {
					 outData = property->m_data;

					 return NAMED_PROPERTY_RESULT_DATA_FOUND;
				 }
				 else
				 {
					 return NAMED_PROPERTY_RESULT_MISMATCH_DATA_TYPE;
				 }
			}
			else
			{
				return NAMED_PROPERTY_RESULT_NAME_NOT_FOUND;
			}
		}

		//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	};

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	class RecipientInfoBase
	{
	public:
		virtual void ExecuteCallBack(NamedPropertyContainer& parameters) = 0;
	};

	template< typename T_ObjType >
	class RecipientInfo : public RecipientInfoBase
	{
	public:
		T_ObjType* m_object;
		typedef void (T_ObjType::*ObjFuncPtrType) (NamedPropertyContainer&);
		ObjFuncPtrType m_func;

		void ExecuteCallBack(NamedPropertyContainer& parameters)
		{
			(m_object->*m_func)( parameters );
		}
	};

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------


	class EventSystem
	{
		std::map< HashedString, std::vector< RecipientInfoBase* > > m_eventRecipients;
		
	public:

		void FireEvent( const std::string& eventName, NamedPropertyContainer& parameters )
		{
			HashedString nameHashed(eventName);
			std::map< HashedString, std::vector< RecipientInfoBase* > >::iterator recipientsEventIter = m_eventRecipients.find( nameHashed );
			if( recipientsEventIter != m_eventRecipients.end() )
			{
				std::vector<RecipientInfoBase*> recipients = recipientsEventIter->second;
				for( auto recipientsIter = recipients.begin(); recipientsIter != recipients.end(); ++recipientsIter)
				{
					RecipientInfoBase* recipient = (*recipientsIter);
					recipient->ExecuteCallBack(parameters);
				}

			}
		}

		//----------------------------------------------------------------------------------------------------------------------------------------------------------------

		template<typename T_ObjType, typename T_ObjFuncType >
		void RegisterObjectForEvent( T_ObjType* obj, T_ObjFuncType func, const std::string& eventName)
		{
			HashedString nameHashed(eventName);
			RecipientInfo<T_ObjType>* ri = new RecipientInfo<T_ObjType>();
			ri->m_object = obj;
			ri->m_func = func;
			std::map< HashedString, std::vector< RecipientInfoBase* > >::iterator recipientsEventIter = m_eventRecipients.find( nameHashed );
			if( recipientsEventIter != m_eventRecipients.end() )
			{
				recipientsEventIter->second.push_back(ri);
			}
			else
			{
				std::vector< RecipientInfoBase* > baseVec;
				baseVec.push_back(dynamic_cast<RecipientInfoBase*>(ri));
				m_eventRecipients.insert(std::pair<HashedString, std::vector< RecipientInfoBase* > >( nameHashed, baseVec ));
			}
		}

		//----------------------------------------------------------------------------------------------------------------------------------------------------------------

		template<typename T_ObjType>
		void unregisterObjectForAllEvents(T_ObjType* obj)
		{
			if( m_eventRecipients.empty() )
				return;

			std::map< HashedString, std::vector< RecipientInfoBase* > >::iterator recipientsEventIter = m_eventRecipients.begin();
			for(;recipientsEventIter != m_eventRecipients.end(); ++recipientsEventIter)
			{
				std::vector< RecipientInfoBase* >::iterator baseInfoIter = recipientsEventIter->second.begin();
				for(; baseInfoIter != recipientsEventIter->second.end(); baseInfoIter++)
				{
					RecipientInfo<T_ObjType>* pobj = dynamic_cast< RecipientInfo<T_ObjType>* >( (*baseInfoIter) );
					if(pobj != nullptr)
					{
						if( pobj->m_object == obj )
						{
							recipientsEventIter->second.erase(baseInfoIter);
							break;
						}
					}
				}
			}

		}
	};

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------


	template<typename T_ObjType>
	class EventRecipient
	{
	public:

		EventRecipient( EventSystem* eventsystem ) : p_eventSystem(eventsystem) 
		{
			
		}

		virtual ~EventRecipient()
		{
			p_eventSystem->unregisterObjectForAllEvents<T_ObjType>(reinterpret_cast<T_ObjType*>(this) );
		}

		EventSystem* p_eventSystem;
	private:
		EventRecipient() {}
	};

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// 	struct EventToCall
// 	{
// 		HashedString eventName;
// 		NamedPropertyContainer parameters;
// 	};

}