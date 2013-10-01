#include "../stdafx.h"

#include "UIDataStrip.hpp"
#include "UICanvas.hpp"
#include "UINodeSlot.hpp"

namespace woodman
{
	UIDataStrip::UIDataStrip(std::shared_ptr<UICanvas> ParentCanvas,
		std::shared_ptr<UIWidget> parentWidget,
		const std::string& name,
		HashedString uniqueID,
		PropertyType pType,
		unsigned int typeSize)
		:UIWidget(ParentCanvas, parentWidget, name, uniqueID),
		m_startVector(30.0f, 0.0f),
		m_endVector(-30.0f, 0.0f),
		m_propertyType(pType),
		m_typeSize(typeSize)
	{
	}

	void UIDataStrip::Initialize()
	{
		m_lineStripShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\LineShader");
		Texture::TextureFormat tf;
		tf.MagFilter = GL_LINEAR;
		tf.MinFilter = GL_LINEAR;
		tf.MipMapTexture = false;
		m_filterTexture = Texture::CreateOrGetTexture(ASSETS + "FilterFunctions\\Filter_1D_Blue.png", tf);

		UIWidget::Initialize();
	}

	bool UIDataStrip::isPointInBounds(const Vector2f& point)
	{
		// TODO check line collision
		//
		return UIWidget::isPointInBounds(point);
	}

	void UIDataStrip::render(std::shared_ptr<UIMouse> currentMouse)
	{

		float zoomScale = m_parentCanvas->getZoomScale();
		m_lineStripShader->load();

		

		Vector2f NodeBoxMinScreen, NodeBoxMaxScreen;

		m_parentCanvas->mapPointToScreenSpace(m_coordinates, NodeBoxMinScreen);
		m_parentCanvas->mapPointToScreenSpace(m_coordinates + m_canvasCollisionBoxSize, NodeBoxMaxScreen);

		Vector2f NodeBoxSizeScreen = NodeBoxMaxScreen - NodeBoxMinScreen;
		AABB2D screenSpaceBounds = m_parentCanvas->getScreenSpace();

		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
 		glUniform2f( m_lineStripShader->getUniformID(HASHED_STRING_u_position),NodeBoxMinScreen.x, NodeBoxMinScreen.y );
		glUniform2f( m_lineStripShader->getUniformID(HASHED_STRING_u_size), NodeBoxSizeScreen.x, NodeBoxSizeScreen.y);
 		glUniform2f( m_lineStripShader->getUniformID(HASHED_STRING_u_screenMin), screenSpaceBounds.m_vMin.x, screenSpaceBounds.m_vMin.y  );
		glUniform2f( m_lineStripShader->getUniformID(HASHED_STRING_u_screenMax), screenSpaceBounds.m_vMax.x, screenSpaceBounds.m_vMax.y);
		glUniform2f(m_lineStripShader->getUniformID(HASHED_STRING_u_canvasMin), m_coordinates.x, m_coordinates.y  );
		glUniform2f(m_lineStripShader->getUniformID(HASHED_STRING_u_canvasMax), (m_coordinates + m_canvasCollisionBoxSize).x, (m_coordinates + m_canvasCollisionBoxSize).y);
		glUniform1i(m_lineStripShader->getUniformID(HASHED_STRING_u_filter),0);

 
 
 		glUniform2fv( m_lineStripShader->getUniformID(HASHED_STRING_u_lineSegmentsStrip), m_controlPoints.size(), reinterpret_cast<GLfloat*>( m_controlPoints.data() ) );
 		glUniform1i( m_lineStripShader->getUniformID(HASHED_STRING_u_numLines), m_controlPoints.size() - 1);
		glUniform1f( m_lineStripShader->getUniformID(HASHED_STRING_u_lineSize), m_style->LineWidth);
		glUniform2f(m_lineStripShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

		Texture::ApplyTexture(m_filterTexture);

		m_lineStripShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

		glDrawArrays( GL_QUADS, 0, 4);

		m_lineStripShader->disableAttribute(HASHED_STRING_in_position);
	}

	void UIDataStrip::calcControlPoints()
	{
		m_controlPoints.clear();

		m_controlPoints.push_back(m_startPoint);
		m_controlPoints.push_back(m_startPoint + m_startVector);
		m_controlPoints.push_back(m_endPoint + m_endVector);
		m_controlPoints.push_back(m_endPoint);

		Vector2f minValues(m_startPoint), maxValues(m_endPoint);

		for(auto it = m_controlPoints.begin(); it != m_controlPoints.end(); ++it)
		{
			minValues.x = min(minValues.x, (*it).x);
			minValues.y = min(minValues.y, (*it).y);
			maxValues.x = max(maxValues.x, (*it).x);
			maxValues.y = max(maxValues.y, (*it).y);
		}

		m_coordinates = minValues - Vector2f(0.0f, m_style->LineWidth);
		m_canvasCollisionBoxSize = maxValues - minValues + Vector2f(0.0f, m_style->LineWidth * 2);
	}


	void UIDataStrip::updateStartPoint(const Vector2f& StartPoint)
	{
		m_startPoint = StartPoint;
		calcControlPoints();
	}

	void UIDataStrip::updateEndPoint(const Vector2f& EndPoint)
	{
		m_endPoint = EndPoint;
		calcControlPoints();
	}

	void UIDataStrip::MouseRelease(std::shared_ptr<UIMouse> currentMouse)
	{
		UIWidget::MouseRelease(currentMouse);

		std::shared_ptr<UINodeSlot> slot( std::dynamic_pointer_cast<UINodeSlot>(currentMouse->hoveringWidget) );

		if( slot != nullptr)
		{
			if(slot->getDataType()->type == m_propertyType )
			{
				//slot->setDataSize( m_typeSize );

				//if dragging the endpoint, then should only connect to entrance slots
				if(m_dragEndPoint)
				{
					if(!slot->getExitSlot())
					{
						m_endTarget = slot;
						slot->setDataStrip(std::dynamic_pointer_cast<UIDataStrip>(WidgetDatabase[m_id]), std::dynamic_pointer_cast<UINodeSlot>(m_startTarget) );
						UINodeSlot::PairNodeSlots(slot, std::dynamic_pointer_cast<UINodeSlot>(m_startTarget) );
					}
				}
				else
				{
					if(slot->getExitSlot())
					{
						m_startTarget = slot;
						slot->setDataStrip( std::dynamic_pointer_cast<UIDataStrip>(WidgetDatabase[m_id]), std::dynamic_pointer_cast<UINodeSlot>(m_endTarget) );
						UINodeSlot::PairNodeSlots( std::dynamic_pointer_cast<UINodeSlot>(m_endTarget), std::dynamic_pointer_cast<UINodeSlot>(m_startTarget) );
					}
				}
			}
		}
		else
		{
			//which point are we dragging
			if(m_dragEndPoint)
			{
				updateEndPoint(m_parentCanvas->getCurrentMouseCanvasPosition());
			}
			else
			{
				updateStartPoint(m_parentCanvas->getCurrentMouseCanvasPosition());
			}
		}

	}

	void UIDataStrip::update(std::shared_ptr<UIMouse> currentMouse)
	{
		if(currentMouse->isPressed)
		{
			if(currentMouse->selectedWidget.get() == this)
			{
				//Im being dragged, update end point

				std::shared_ptr<UINodeSlot> slot( std::dynamic_pointer_cast<UINodeSlot>(currentMouse->hoveringWidget) );
	
				if( slot != nullptr)
				{

				}
				else
				{
					//which point are we dragging
					if(m_dragEndPoint)
					{
						updateEndPoint(m_parentCanvas->getCurrentMouseCanvasPosition());
					}
					else
					{
						updateStartPoint(m_parentCanvas->getCurrentMouseCanvasPosition());
					}
				}
			}
		}
	}

	bool UIDataStrip::isGoodStrip()
	{
		return (m_endTarget != nullptr && m_startTarget != nullptr);
	}

	void UIDataStrip::setDragEndPoint(bool dragEndPoint)
	{
		m_dragEndPoint = dragEndPoint;
	}

	void UIDataStrip::updateStartTarget(std::shared_ptr<UIWidget> StartTarget)
	{
		m_startTarget = StartTarget;
	}

	void UIDataStrip::updateEndTarget(std::shared_ptr<UIWidget> EndTarget)
	{
		m_endTarget = EndTarget;
	}
}

