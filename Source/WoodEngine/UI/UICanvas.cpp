#include "../stdafx.h"

#include "UICanvas.hpp"
#include "UINodeBox.hpp"
#include "UILinkStrip.hpp"
#include "../ShaderEditor/NodeInstance.hpp"

#include <sstream>

namespace woodman
{
	UICanvas::UICanvas( EventSystem* eventsystem, const AABB2D& canvasScreenSpace, const std::string& backgroundTexturePath,
		const Vector2i& backgroundResolution, Vector2i zoomBounds)
		: m_zoom(10),
		m_center( Vector2f(0.0f, 0.0f) ),
		m_screenSpace( canvasScreenSpace ),
		m_backgroundTexturePath(backgroundTexturePath),
		m_backgroundResolution(backgroundResolution),
		m_zoomBounds(zoomBounds),
		m_layer(0),
		EventRecipient(eventsystem)
	{
	}

	bool UICanvas::Initialize()
	{
		p_eventSystem->RegisterObjectForEvent(this, &UICanvas::catchMouseWheel, "MouseWheel" );

		m_backgroundShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\CanvasBackground");

		Texture::TextureFormat tf;
		tf.MagFilter = GL_LINEAR;
		tf.MinFilter = GL_LINEAR;
		tf.MipMapTexture = false;
		
		
		m_backgroundTexture = Texture::CreateOrGetTexture(m_backgroundTexturePath, tf);

		m_textFont = Font::CreateOrGetFont(HASHED_STRING_tahoma);
		calcCanvasSpace();

 		return true;
	}

	void UICanvas::RegisterUIWidget( UIWidget* widget)
	{
		m_UIWidgets.insert(std::unique_ptr<UIWidget>(widget));
	}

	void UICanvas::UnRegisterUIWidget( UIWidget* widget )
	{
		//Actually erase stuff
		//
		// Todo

		//m_UIWidgets.erase( widget );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	void UICanvas::RenderCanvas( UIMouse* currentMouse )
	{

		m_backgroundShader->load();
		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		Texture::ApplyTexture(m_backgroundTexture);
		glUniform2f(m_backgroundShader->getUniformID(HASHED_STRING_u_position), 
			m_screenSpace.m_vMin.x, 
			m_screenSpace.m_vMin.y);
		glUniform2f(m_backgroundShader->getUniformID(HASHED_STRING_u_size), m_screenSpace.calcSize().x, m_screenSpace.calcSize().y);
		glUniform2f(m_backgroundShader->getUniformID(HASHED_STRING_u_mousePos), m_currentMouseCanvasPosition.x, m_currentMouseCanvasPosition.y);
		glUniform2f(m_backgroundShader->getUniformID(HASHED_STRING_u_canvasMin), m_canvasSpace.m_vMin.x, m_canvasSpace.m_vMin.y  );
		glUniform2f(m_backgroundShader->getUniformID(HASHED_STRING_u_canvasMax), m_canvasSpace.m_vMax.x, m_canvasSpace.m_vMax.y);
		glUniform1i(m_backgroundShader->getUniformID(HASHED_STRING_u_backgroundTexture), 0);
		glUniform2f(m_backgroundShader->getUniformID(HASHED_STRING_u_backgroundResolution), 1.0f / static_cast<float>(m_backgroundResolution.x), 1.0f / static_cast<float>(m_backgroundResolution.y) );
		glUniform2f(m_backgroundShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

		m_backgroundShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

		glDrawArrays( GL_QUADS, 0, 4);

		m_backgroundShader->disableAttribute(HASHED_STRING_in_position);


		//render my nodeBoxes
		for(auto it = m_UIWidgets.begin(); it != m_UIWidgets.end(); ++it)
		{
			(*it)->render( currentMouse );
		}
	}

	void UICanvas::update( UIMouse* currentMouse )
	{
		if(m_screenSpace.isPointInsideBounds(currentMouse->screenPosition) )
		{
			m_prevMouseCanvasPosition = m_currentMouseCanvasPosition;
			mapPointToCanvasSpace(currentMouse->screenPosition, m_currentMouseCanvasPosition);
		
 
 			Vector2f mouseDelta = m_currentMouseCanvasPosition - m_prevMouseCanvasPosition;

			for(auto it = m_UIWidgets.begin(); it != m_UIWidgets.end(); ++it)
			{
				(*it)->update(currentMouse);
			}


 			if(currentMouse->isPressed && currentMouse->selectedCanvas == this)
 			{
 				if(currentMouse->selectedWidget == nullptr)
 				{
					//move canvas
					m_center -= mouseDelta;
					calcCanvasSpace();
					m_prevMouseCanvasPosition = m_currentMouseCanvasPosition;
					mapPointToCanvasSpace(currentMouse->screenPosition, m_currentMouseCanvasPosition);
 				}
 			}
		}
	}

	void UICanvas::catchMouseWheel(NamedPropertyContainer& parameters)
	{
		unsigned int wParam;
		parameters.getNamedData(HashedString("Direction"), wParam);
		if( (  GET_WHEEL_DELTA_WPARAM(wParam) ) > 0)
		{
			m_zoom++;
		}
		else
		{
			m_zoom--;
		}

		m_zoom = clamp(m_zoom, m_zoomBounds.x, m_zoomBounds.y);
		calcCanvasSpace();
	}

	void UICanvas::calcCanvasSpace()
	{
		//calculate new canvas space, zoom level 10 means 1:1 pixel ratio
		float zoomLevel = 10.0f / static_cast<float>(m_zoom);
		m_canvasSpace.m_vMin = m_center - (m_screenSpace.calcSize() * .5f) * zoomLevel;
		m_canvasSpace.m_vMax = m_center + (m_screenSpace.calcSize() * .5f) * zoomLevel;

	}

	void UICanvas::mapPointToCanvasSpace(Vector2f pointToMap, Vector2f& pointInCanvasSpace)
	{
		mapValueToRange(pointToMap.x, m_screenSpace.m_vMin.x, m_screenSpace.m_vMax.x, m_canvasSpace.m_vMin.x, m_canvasSpace.m_vMax.x, pointInCanvasSpace.x);
		mapValueToRange(pointToMap.y, m_screenSpace.m_vMin.y, m_screenSpace.m_vMax.y, m_canvasSpace.m_vMin.y, m_canvasSpace.m_vMax.y, pointInCanvasSpace.y);
	}

	void UICanvas::mapPointToScreenSpace(Vector2f pointToMap, Vector2f& pointInScreenSpace)
	{
		mapValueToRange(pointToMap.x, m_canvasSpace.m_vMin.x, m_canvasSpace.m_vMax.x, m_screenSpace.m_vMin.x, m_screenSpace.m_vMax.x, pointInScreenSpace.x);
		mapValueToRange(pointToMap.y, m_canvasSpace.m_vMin.y, m_canvasSpace.m_vMax.y, m_screenSpace.m_vMin.y, m_screenSpace.m_vMax.y, pointInScreenSpace.y);
	}

	void UICanvas::setScreenSpace( const AABB2D& ScreenSpace )
	{
		m_screenSpace = ScreenSpace;
		calcCanvasSpace();
	}

	float UICanvas::getZoomScale()
	{
		return (static_cast<float>( m_zoom) ) / 10.0f;
	}

	AABB2D UICanvas::getScreenSpace()
	{
		return m_screenSpace;
	}

	UIWidget* UICanvas::getUIWidgetatPoint(const Vector2f& PointScreenSpace)
	{
		Vector2f PointCanvasSpace;
		mapPointToCanvasSpace(PointScreenSpace, PointCanvasSpace);

		UIWidget* topWidget = nullptr;

		for(auto it = m_UIWidgets.begin(); it != m_UIWidgets.end(); ++it)
		{
			(*it)->getTopWidgetColliding(PointCanvasSpace, topWidget );
		}
		
		return topWidget;
	}


	UIWidget* UICanvas::getUIWidgetByID(HashedString ID)
	{
		UIWidget* result = nullptr;

		for(auto it = m_UIWidgets.begin(); it != m_UIWidgets.end(); ++it)
		{
			result = (*it)->getUIWidgetByID(ID);
			if(result != nullptr)
				return result;
		}

		return nullptr;
	}
}