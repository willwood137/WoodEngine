#include "..\stdafx.h"

#include "UIStyle.hpp"

#include "../libs/pugixml.hpp"

#include <sstream>

namespace woodman
{

	std::shared_ptr<UIStyle> UIStyle::DefaultUIStyle( new UIStyle() );

	void UIStyle::LoadDefaultUIStyle(const std::string& filePath)
	{
		
		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( (filePath).c_str() );
		pugi::xml_node topNode = doc->first_child();

		if(std::string(topNode.name()).compare(std::string("UIStyle") ) == 0 )
		{
			pugi::xml_node StyleNode = topNode.first_child();
			while(StyleNode)
			{
			
				if(std::string(StyleNode.name()).compare(std::string("Title") ) == 0)
				{
					DefaultUIStyle->TitleSize = StyleNode.attribute("size").as_float();
					DefaultUIStyle->TitleBuffer = StyleNode.attribute("buffer").as_float();
					std::stringstream ss( StyleNode.attribute("color").as_string());

					float R, G, B, A;

					ss >> R >> G >> B >> A;
					DefaultUIStyle->TitleColor = RGBA(R, G, B, A);
				}
				else if(std::string(StyleNode.name()).compare(std::string("SubTitle") ) == 0)
				{
					DefaultUIStyle->subTitleSize = StyleNode.attribute("size").as_float();
					DefaultUIStyle->subTitleBuffer = StyleNode.attribute("buffer").as_float();
					std::stringstream ss( StyleNode.attribute("color").as_string());

					float R, G, B, A;

					ss >> R >> G >> B >> A;
					DefaultUIStyle->subTitleColor = RGBA(R, G, B, A);
				}
				else if(std::string(StyleNode.name()).compare(std::string("NodeBox") ) == 0)
				{
					DefaultUIStyle->NodeBoxBorderLength = StyleNode.attribute("BorderLength").as_float();
					DefaultUIStyle->NodeBoxCornerSize = StyleNode.attribute("CornerSize").as_float();
				}
				else if(std::string(StyleNode.name()).compare(std::string("Line") ) == 0)
				{
					DefaultUIStyle->LineWidth = StyleNode.attribute("Width").as_float();
					DefaultUIStyle->NodeLineBuffer = StyleNode.attribute("Buffer").as_float();
				}
				else if(std::string(StyleNode.name()).compare(std::string("MouseMenu") ) == 0)
				{

					DefaultUIStyle->MouseMenuTextSize = StyleNode.attribute("TextSize").as_float();
					DefaultUIStyle->MouseMenuHeight = StyleNode.attribute("Height").as_float();
					DefaultUIStyle->MouseMenuWidth = StyleNode.attribute("width").as_float();
					DefaultUIStyle->MouseMenuLineWidth = StyleNode.attribute("lineWidth").as_float();
					
					float R, G, B, A;

					std::stringstream ssBack(StyleNode.attribute("backgroundColor").as_string() );
					ssBack >> R >> G >> B >> A;
					DefaultUIStyle->MouseMenuBackgroundColor = RGBA(R, G, B, A);


					std::stringstream ssline(StyleNode.attribute("lineColor").as_string() );
					ssline >> R >> G >> B >> A;
					DefaultUIStyle->MouseMenuLineColor= RGBA(R, G, B, A);


					std::stringstream ssText(StyleNode.attribute("TextColor").as_string() );
					ssText >> R >> G >> B >> A;
					DefaultUIStyle->MouseMenuTextColor = RGBA(R, G, B, A);

					
				}
				// go to next sibling
				StyleNode = StyleNode.next_sibling();
			}
		}


		DefaultUIStyle->TitleTextFont = Font::CreateOrGetFont(HASHED_STRING_tahoma);
		DefaultUIStyle->subTitleTextFont = Font::CreateOrGetFont(HASHED_STRING_tahoma);

	}
}