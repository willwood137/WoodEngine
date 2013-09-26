#include "..\stdafx.h"

#include "UIWindow.hpp"

namespace shadow
{

	const std::string SHADER_UIWINDOW_GRID_SOURCE_FILE_PATH = "Shaders\\UIWindow";

	UIWindow::UIWindow(const std::string& name, EventSystem* eventsystem)
		:m_name(name),
		EventRecipient(eventsystem)
	{

	}


	UIWindow* UIWindow::parseWindow(pugi::xml_node& node)
	{
		return this;
	}

	void UIWindow::setName(const std::string& name)
	{
		m_name = name;
	}

	void UIWindow::setParent(UIWindow* parent)
	{
		m_pParentWindow = parent;
	}

	void UIWindow::initialize( const AABB2D& parentBox )
	{
		//set Position
		
		
		AABB2D windowBox(Vector2f(0.f, 0.f), m_collisionBox.getSize() );
		std::vector<WindowVertexData> tempVertexData;
		float halfBorderLength = m_borderLength * .5f;
		float inverseBorderLength =  1.f / m_borderLength;

		//Create Background
		//
		{
			CreateRenderDataForElement(ASSETS + "Textures\\UI\\" + m_backgroundTextureFile,
				ContainedAABB2D( windowBox, Vector2f(0.0f, .0f), Vector2f(-halfBorderLength,-halfBorderLength), ATTACH_TYPE_BOTTOMLEFT, ATTACH_TYPE_TOPRIGHT ),
				AABB2D(Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f)),
				AABB2D(Vector2f(0.0f, inverseBorderLength * .5f), Vector2f(inverseBorderLength, .0f)),
				false,
				tempVertexData );
		}

		//Create Top Bar
		//
		if( !m_drawBorderOnEdges 
			|| m_collisionBox.getAttachToType() == ATTACH_TYPE_BOTTOMLEFT
			|| m_collisionBox.getAttachToType() == ATTACH_TYPE_BOTTOM
			|| m_collisionBox.getAttachToType() == ATTACH_TYPE_LEFT
			|| m_collisionBox.getAttachToType() == ATTACH_TYPE_FREE
			|| m_collisionBox.getAttachToType() == ATTACH_TYPE_BOTTOMRIGHT
			|| m_collisionBox.getAttachToType() == ATTACH_TYPE_RIGHT)
		{
			CreateRenderDataForElement(ASSETS + "Textures\\UI\\" + m_borderBarTextureFile,
				ContainedAABB2D( windowBox, Vector2f(0.0f, -m_borderLength), Vector2f(-m_borderLength, 0.0f), ATTACH_TYPE_TOPLEFT, ATTACH_TYPE_TOPRIGHT ),
				AABB2D(Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f)),
				AABB2D(Vector2f(0.0f, inverseBorderLength), Vector2f( inverseBorderLength, 0.f  )),
				false,
				tempVertexData );
		}

		//Create Right Bar
		//
		if( !m_drawBorderOnEdges 
			||m_collisionBox.getAttachToType() == ATTACH_TYPE_BOTTOMLEFT
			||m_collisionBox.getAttachToType() == ATTACH_TYPE_BOTTOM
			||m_collisionBox.getAttachToType() == ATTACH_TYPE_LEFT
			||m_collisionBox.getAttachToType() == ATTACH_TYPE_FREE
			||m_collisionBox.getAttachToType() == ATTACH_TYPE_TOPLEFT
			||m_collisionBox.getAttachToType() == ATTACH_TYPE_TOP )
		{
			CreateRenderDataForElement(ASSETS + "Textures\\UI\\" + m_borderBarTextureFile,
				ContainedAABB2D( windowBox, Vector2f(-m_borderLength, 0.f), Vector2f(0.0f, -m_borderLength), ATTACH_TYPE_BOTTOMRIGHT, ATTACH_TYPE_TOPRIGHT ),
				AABB2D(Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f)),
				AABB2D(Vector2f( .0f, inverseBorderLength ), Vector2f( inverseBorderLength * .6f, 0.0f) ),
				true,
				tempVertexData );
		}

		//Create Top-Right Corner
		//
		if( !m_drawBorderOnEdges 
			|| m_collisionBox.getAttachToType()  == ATTACH_TYPE_BOTTOMLEFT
			|| m_collisionBox.getAttachToType()  == ATTACH_TYPE_BOTTOM
			|| m_collisionBox.getAttachToType()  == ATTACH_TYPE_LEFT
			|| m_collisionBox.getAttachToType()  == ATTACH_TYPE_FREE )
		{
			CreateRenderDataForElement(ASSETS + "Textures\\UI\\" + m_borderCornerTextureFile,
				ContainedAABB2D( windowBox, Vector2f(-m_borderLength, -m_borderLength), Vector2f(0.0f, 0.0f), ATTACH_TYPE_TOPRIGHT, ATTACH_TYPE_TOPRIGHT ),
				AABB2D(Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f)),
				AABB2D(Vector2f(0.0f, inverseBorderLength), Vector2f( inverseBorderLength, 0.f  )),
				false,
				tempVertexData );
		}



		m_idVBO = LoadBufferObject(GL_ARRAY_BUFFER, tempVertexData.data(), sizeof(WindowVertexData) * tempVertexData.size() );

		//Load shader
		m_shaderBorder.loadShader(SHADER_UIWINDOW_GRID_SOURCE_FILE_PATH);
		m_shaderBorder.addUniform(HASHED_STRING_u_windowPosition);
		m_shaderBorder.addUniform(HASHED_STRING_u_position);
		m_shaderBorder.addUniform(HASHED_STRING_u_size);
		m_shaderBorder.addUniform(HASHED_STRING_u_diffuse);
		m_shaderBorder.addUniform(HASHED_STRING_u_cursorPosition);

		m_shaderBorder.addAttribute(HASHED_STRING_a_position);
		m_shaderBorder.addAttribute(HASHED_STRING_a_texCoords);


		m_collisionBox.attachToAABB2D(parentBox);
		size_t numChildren = m_childrenWindows.size();
		for( size_t i = 0; i < numChildren; ++i)
		{
			m_childrenWindows[i]->initialize( m_collisionBox.getAABB2D() );
		}

		p_eventSystem->RegisterObjectForEvent(this, &UIWindow::catchShow, "ShowWindow");
		p_eventSystem->RegisterObjectForEvent(this, &UIWindow::catchHide, "HideWindow");
	}

	void UIWindow::CreateRenderDataForElement(const std::string& textureFile, 
		const ContainedAABB2D& relativeWindowPosition,
		const AABB2D& positionBox,
		const AABB2D& textureBox,
		bool flip,
		std::vector<WindowVertexData>& verts )
	{
		unsigned int i = m_renderData.size();
		m_renderData.push_back(WindowRenderData());

		m_renderData[i].texture = Texture::CreateOrGetTexture(textureFile);
		m_renderData[i].startVBO = verts.size();
		m_renderData[i].numVerts = 4;
		m_renderData[i].flip = flip;
		m_renderData[i].relativeWindowBox = relativeWindowPosition;

		float texHeight = static_cast<float>(m_renderData[i].texture->GetHeight());
		float texWidth = static_cast<float>(m_renderData[i].texture->GetWidth());

		float texWidthRatio = texHeight / texWidth;

		verts.push_back( WindowVertexData( Vector2f( positionBox.m_vMin), Vector2f( textureBox.m_vMin.x, textureBox.m_vMin.y ) ) );
		verts.push_back( WindowVertexData( Vector2f( positionBox.m_vMin.x, positionBox.m_vMax.y), Vector2f(textureBox.m_vMin.x, textureBox.m_vMax.y) ) );
		verts.push_back( WindowVertexData( Vector2f( positionBox.m_vMax.x, positionBox.m_vMin.y), Vector2f(textureBox.m_vMax.x * texWidthRatio, textureBox.m_vMin.y  ) ) );
		verts.push_back( WindowVertexData( Vector2f( positionBox.m_vMax ), Vector2f( textureBox.m_vMax.x * texWidthRatio, textureBox.m_vMax.y) ) );
	}

	float UIWindow::render( float z )
	{
		if(m_hidden)
			return z;

		size_t numChildren = m_childrenWindows.size();
		for( size_t i = 0; i < numChildren; ++i)
		{
				z = m_childrenWindows[i]->render( z );
		}


		glFrontFace(GL_CW);

		

		m_shaderBorder.load();
		glBindBuffer(GL_ARRAY_BUFFER, m_idVBO);
		glUniform2f(m_shaderBorder.getUniformID(HASHED_STRING_u_cursorPosition), m_cursorPosition.x, m_cursorPosition.y);
		glUniform2f(m_shaderBorder.getUniformID(HASHED_STRING_u_windowPosition), m_collisionBox.getAABB2D().m_vMin.x, m_collisionBox.getAABB2D().m_vMin.y);
		m_shaderBorder.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(WindowVertexData), offsetof(WindowVertexData, pos) );
		m_shaderBorder.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(WindowVertexData), offsetof(WindowVertexData, uv) );
		glDisable(GL_DEPTH_TEST);
		size_t numElementsToDraw = m_renderData.size();
		for(size_t i = 0; i < numElementsToDraw; ++i)
		{
			Vector2f pos2 = m_renderData[i].relativeWindowBox.getBox().m_vMin;

			Vector2f size = m_renderData[i].relativeWindowBox.getBox().calcSize();

			Vector3f pos(pos2.x, pos2.y, z);
			z-=.00005f;
			glUniform3f(m_shaderBorder.getUniformID(HASHED_STRING_u_position), pos.x, pos.y, pos.z);
			if(m_renderData[i].flip)
			{
				glUniform3f(m_shaderBorder.getUniformID(HASHED_STRING_u_size), size.x, size.y, 1.f);
			}
			else
			{
				glUniform3f(m_shaderBorder.getUniformID(HASHED_STRING_u_size), size.x, size.y, .0f);
			}
			
			glActiveTexture( GL_TEXTURE0 );
			Texture::ApplyTexture(m_renderData[i].texture);
			glUniform1i(m_shaderBorder.getUniformID(HASHED_STRING_u_diffuse), 0);

			glDrawArrays(GL_TRIANGLE_STRIP, m_renderData[i].startVBO, m_renderData[i].numVerts);

		}

		m_shaderBorder.disableAttribute(HASHED_STRING_a_position);
		m_shaderBorder.disableAttribute(HASHED_STRING_a_texCoords);

		size_t numWidgets = m_widgets.size();
		for( size_t i = 0; i < numWidgets; ++i)
		{
			m_widgets[i]->render();
		}

		glEnable(GL_DEPTH_TEST);
		return z;
	}

// 	inline Vector2f UIWindow::getPositionOfWindowPositionRelativeToBottomLeft(const WindowPosition& windowPosition)
// 	{
// 		Vector2f dir(1.0f, 1.0f);
// 		Vector2f pos(0.0f, 0.0f);
// 		switch (windowPosition.offsetOf)
// 		{
// 		case ATTACH_TYPE_TOPLEFT:
// 		case ATTACH_TYPE_TOP:
// 			dir.y = -1.f;
// 			break;
// 		case ATTACH_TYPE_TOPRIGHT:
// 			dir.x = -1.f;
// 			dir.y = -1.f;
// 			break;
// 		case ATTACH_TYPE_RIGHT:
// 		case ATTACH_TYPE_BOTTOMRIGHT:
// 			dir.x = -1.f;
// 			break;
// 		case ATTACH_TYPE_LEFT:
// 		case ATTACH_TYPE_BOTTOMLEFT:
// 		case ATTACH_TYPE_BOTTOM:
// 			break;
// 		}
// 
// 		switch (windowPosition.offsetOf)
// 		{
// 		case ATTACH_TYPE_TOPLEFT:
// 			pos.y = m_size.y;
// 			break;
// 		case ATTACH_TYPE_TOP:
// 			pos.x = m_size.x * .5f;
// 			pos.y = m_size.y;
// 			break;
// 		case ATTACH_TYPE_TOPRIGHT:
// 			pos.x = m_size.x;
// 			pos.y = m_size.y;
// 			break;
// 		case ATTACH_TYPE_RIGHT:
// 			pos.x = m_size.x;
// 			pos.y = m_size.y * .5f;
// 			break;
// 		case ATTACH_TYPE_BOTTOMRIGHT:
// 			pos.x = m_size.x;
// 			break;
// 		case ATTACH_TYPE_BOTTOM:
// 			pos.x = m_size.x * .5f;
// 			break;
// 		//case ATTACH_TYPE_BOTTOMLEFT:
// 		case ATTACH_TYPE_LEFT:
// 			pos.y = m_size.y * .5f;
// 			break;
// 		}
// 		return pos + Vector2f(windowPosition.offset.x * dir.x, windowPosition.offset.y * dir.y);
// 	}


	void UIWindow::update(const Vector2f& cursorPosition)
	{
		m_cursorPosition = cursorPosition;
		
		size_t numChildren = m_childrenWindows.size();
		for( size_t i = 0; i < numChildren; ++i)
		{
			m_childrenWindows[i]->update(cursorPosition);
		}

		size_t numWidgets = m_widgets.size();
		for( size_t i = 0; i < numWidgets; ++i)
		{
			m_widgets[i]->update(cursorPosition);
		}
	}

	bool UIWindow::checkMouseClick(const Vector2f& cursorPosition)
	{
		if(m_hidden)
			return false;

		bool collision = m_collisionBox.getAABB2D().isPointInsideBounds(cursorPosition);

		size_t numWidgets = m_widgets.size();
		for(size_t i = 0; i < numWidgets; ++i)
		{
			if(m_widgets[i]->checkMouseClick( cursorPosition) )
				collision = true;
		}

		size_t numWindows = m_childrenWindows.size();
		for(size_t i = 0; i < numWindows; ++i)
		{
			if(m_childrenWindows[i]->checkMouseClick( cursorPosition ) )
				collision = true;
		}

		return collision;
	}


	void UIWindow::catchShow(NamedPropertyContainer& param)
	{
		std::string name;
		param.getNamedData("WindowName", name);
		if( _stricmp( name.c_str(), m_name.c_str() ) == 0)
		{
			m_hidden = false;
		}
	}

	void UIWindow::catchHide(NamedPropertyContainer& param)
	{
		std::string name;
		param.getNamedData("WindowName", name);
		if( _stricmp( name.c_str(), m_name.c_str() ) == 0)
		{
			m_hidden = true;
		}
	}
}