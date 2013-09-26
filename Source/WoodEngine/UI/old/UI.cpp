#include "..\stdafx.h"

#include "UI.hpp"
#include "pugixml.hpp"
#include "UIButtonBar.hpp"
#include "..\Math\AABB2D.hpp"

namespace shadow
{

	UIWindow* parseWindow(pugi::xml_node& node);
	UIWindow* parseButtonBar(pugi::xml_node& node);

	UI::UI( const std::string& filepath, EventSystem* eventsystem )
		: m_filePath(filepath),
		EventRecipient(eventsystem)
	{
		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( (m_filePath).c_str() );
		pugi::xml_node topNode = doc->first_child();

		if(std::string(topNode.name()).compare(std::string("UI") ) == 0 )
		{
			pugi::xml_node UINode = topNode.first_child();
			while(UINode)
			{
				if( _stricmp(UINode.name(), "Window") == 0) // This is a new Window
				{
					std::string type(UINode.attribute("type").as_string());
					std::string name(UINode.attribute("name").as_string());

					if(_stricmp(type.c_str(), "buttonbar") == 0)
					{
						UIWindow* window = new UIButtonBar(name, p_eventSystem);
						window->setParent(nullptr);
						m_Windows.push_back(window->parseWindow(UINode));
					}
				}
				UINode = UINode.next_sibling();
			}
		}

		delete doc;
	}

	void UI::initialize()
	{
		size_t numWindows = m_Windows.size();
		for(size_t i = 0; i < numWindows; ++i)
		{
			m_Windows[i]->initialize( AABB2D( Vector2f(-1.f, -1.f), Vector2f(1.f, 1.f) ) );
		}
	}

	void UI::render()
	{
		size_t numWindows = m_Windows.size();
		float z = .95f;
		for(size_t i = 0; i < numWindows; ++i)
		{
			z = m_Windows[i]->render( z );
		}
	}

	void UI::update(const Vector2f& cursorPosition)
	{
		size_t numWindows = m_Windows.size();
		Vector2f cursorPositionInViewport(cursorPosition);

		GLint viewport[4];

		glGetIntegerv( GL_VIEWPORT, viewport );

		mapValueToRange(cursorPosition.x, 0.f, static_cast<float>(viewport[2]), -1.f, 1.f, cursorPositionInViewport.x );
		mapValueToRange(cursorPosition.y, 0.f, static_cast<float>(viewport[3]), -1.f, 1.f, cursorPositionInViewport.y );

		for(size_t i = 0; i < numWindows; ++i)
		{
			m_Windows[i]->update(cursorPositionInViewport);
		}
	}

	bool UI::checkMouseClick(const Vector2f& cursorPosition)
	{
		Vector2f cursorPositionInViewport(cursorPosition);

		GLint viewport[4];

		glGetIntegerv( GL_VIEWPORT, viewport );

		mapValueToRange(cursorPosition.x, 0.f, static_cast<float>(viewport[2]), -1.f, 1.f, cursorPositionInViewport.x );
		mapValueToRange(cursorPosition.y, 0.f, static_cast<float>(viewport[3]), -1.f, 1.f, cursorPositionInViewport.y );

		size_t numWindows = m_Windows.size();
		bool collision = false;
		for(size_t i = 0; i < numWindows; ++i)
		{
			if(m_Windows[i]->checkMouseClick( cursorPositionInViewport) )
				collision = true;
		}
		return collision;
	}
}