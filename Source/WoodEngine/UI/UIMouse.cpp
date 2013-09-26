#include "../stdafx.h"

#include "UIMouse.hpp"

#include "../Engine/Font.hpp"
#include "../Math/AABB2D.hpp"

namespace woodman
{
	void UIMouse::render()
	{
		renderMenu(MainMenu, prevRClickPosition, UIStyle::DefaultUIStyle);
		
	}

	void UIMouse::renderMenu(std::shared_ptr<MouseMenu> menu, const Vector2f& menuPosition, std::shared_ptr<UIStyle> style )
	{
		//--------------------------
		// Render the box
		//--------------------------
		mouseMenuShader->load();

		Vector2f boxSize = Vector2f(style->MouseMenuWidth, -style->MouseMenuHeight * static_cast<float>(menu->subMenus.size()));

		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		glUniform2f(mouseMenuShader->getUniformID(HASHED_STRING_u_position),menuPosition.x, menuPosition.y );
		glUniform2f(mouseMenuShader->getUniformID(HASHED_STRING_u_size), boxSize.x, boxSize.y);
		glUniform1f(mouseMenuShader->getUniformID(HASHED_STRING_u_boxHeight), style->MouseMenuHeight);
		glUniform1f(mouseMenuShader->getUniformID(HASHED_STRING_u_lineWidth), style->MouseMenuLineWidth);
		glUniform4fv(mouseMenuShader->getUniformID(HASHED_STRING_u_backgroundColor), 1, reinterpret_cast<GLfloat*>( &(style->MouseMenuBackgroundColor) ) );
		glUniform4fv(mouseMenuShader->getUniformID(HASHED_STRING_u_lineColor), 1, reinterpret_cast<GLfloat*>( &(style->MouseMenuLineColor) ) );
		glUniform2f(mouseMenuShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

		mouseMenuShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

		glDrawArrays( GL_QUADS, 0, 4);

		mouseMenuShader->disableAttribute(HASHED_STRING_in_position);

		size_t numSubMenus = menu->subMenus.size();
		for(size_t i = 0; i < numSubMenus; i++)
		{
			Font::DrawTextToScreen(menu->subMenus[i]->name, style->subTitleTextFont, style->MouseMenuTextColor, menuPosition + Vector2f(style->MouseMenuWidth * .5f, -style->MouseMenuHeight * (i + .8f) ), style->MouseMenuTextSize, ALIGNMENT_CENTER );
			
			if(menu->subMenus[i]->open)
			{
				Vector2f newPos(menuPosition.x + style->MouseMenuWidth, menuPosition.y - i * style->MouseMenuHeight );
				renderMenu(menu->subMenus[i], newPos, style);
			}
		}
	}


	bool UIMouse::clickMenu( std::shared_ptr<MouseMenu> menu, const Vector2f& menuPosition, EventSystem* eventSystem)
	{
		Vector2f menuSize(UIStyle::DefaultUIStyle->MouseMenuWidth, -(menu->subMenus.size() * UIStyle::DefaultUIStyle->MouseMenuHeight));

		AABB2D menuBox(Vector2f(menuPosition.x, menuPosition.y + menuSize.y), Vector2f(menuPosition.x + menuSize.x, menuPosition.y) );

		if(menuBox.isPointInsideBounds(prevClickPosition))
		{
			int menuItem = floor( ( menuPosition.y - prevClickPosition.y) / UIStyle::DefaultUIStyle->MouseMenuHeight );

			menuItem = clamp<int>(menuItem, 0, menu->subMenus.size() - 1);
			if(menu->subMenus[menuItem]->subMenus.size() > 0)
			{
				//there is a sub menu
				menu->subMenus[menuItem]->open = !menu->subMenus[menuItem]->open;

				//close the other menus
				size_t numSubMenus = menu->subMenus.size();
				for(size_t i = 0; i < numSubMenus; i++)
				{
					if(i != menuItem)
					{
						menu->subMenus[i]->open = false;
					}
				}
			}
			else
			{
				//Fire That Event!
				//
				// TODO
				eventSystem->FireEvent(menu->subMenus[menuItem]->EventToFire, menu->subMenus[menuItem]->Parameters );

				size_t numSubMenus = menu->subMenus.size();
				for(size_t i = 0; i < numSubMenus; i++)
				{
					menu->subMenus[i]->open = false;
				}

				menuOpen = false;
				return true;
			}
		}
		else
		{
			//check if things are open!
			//
			bool clickedMenu = false;
			size_t numSubMenus = menu->subMenus.size();
			for(size_t i = 0; i < numSubMenus; i++)
			{
				if(menu->subMenus[i]->open)
				{
					Vector2f newPos(menuPosition.x + UIStyle::DefaultUIStyle->MouseMenuWidth, menuPosition.y - i * UIStyle::DefaultUIStyle->MouseMenuHeight );
					clickedMenu = clickedMenu || clickMenu(menu->subMenus[i], newPos, eventSystem);
				}
			}

			return clickedMenu;
		}
	}
}