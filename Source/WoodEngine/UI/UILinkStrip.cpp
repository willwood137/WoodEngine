#include "../stdafx.h"

#include "UILinkStrip.hpp"
#include "UICanvas.hpp"
#include "UINodeLink.hpp"

namespace woodman
{
	UILinkStrip::UILinkStrip(UICanvas* ParentCanvas,
		UIWidget* parentWidget,
		const std::string& name,
		HashedString uniqueID,
		PropertyType pType,
		unsigned int typeSize,
		CanvasCoordinates startPoint,
		CanvasCoordinates endPoint)
		:UIWidget(ParentCanvas, parentWidget, name, uniqueID),
		m_startVector(30.0f, 0.0f),
		m_endVector(-30.0f, 0.0f),
		m_propertyType(pType),
		m_typeSize(typeSize),
		m_startPoint(startPoint),
		m_endPoint(endPoint)
	{
	}

	void UILinkStrip::Initialize()
	{
		m_lineStripShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\LineShader");
		Texture::TextureFormat tf;
		tf.MagFilter = GL_LINEAR;
		tf.MinFilter = GL_LINEAR;
		tf.MipMapTexture = false;
		m_filterTexture = Texture::CreateOrGetTexture(ASSETS + "FilterFunctions\\Filter_1D_Blue.png", tf);

		UIWidget::Initialize();
	}

	bool UILinkStrip::isPointInBounds(const Vector2f& point)
	{
		// TODO check line collision
		//
		return UIWidget::isPointInBounds(point);
	}

	void UILinkStrip::render(UIMouse* currentMouse)
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

	void UILinkStrip::calcControlPoints()
	{
		m_controlPoints.clear();

		if(m_startPoint.getOwningCanvas() == nullptr || m_endPoint.getOwningCanvas() == nullptr)
			return;

		CanvasCoordinates p2(m_startPoint);
		p2.coordinatesCanvasSpace += m_startVector;

		CanvasCoordinates p3(m_endPoint);
		p3.coordinatesCanvasSpace += m_endVector;

		m_controlPoints.push_back(m_startPoint);
		m_controlPoints.push_back(p2);
		m_controlPoints.push_back(p3);
		m_controlPoints.push_back(m_endPoint);

		Vector2f minValues(m_startPoint.getScreenCoordinates()), maxValues(m_endPoint.getScreenCoordinates());

		for(auto it = m_controlPoints.begin(); it != m_controlPoints.end(); ++it)
		{
			Vector2f screenCoords = (*it).getScreenCoordinates();
			minValues.x = min(minValues.x, screenCoords.x);
			minValues.y = min(minValues.y, screenCoords.y);
			maxValues.x = max(maxValues.x, screenCoords.x);
			maxValues.y = max(maxValues.y, screenCoords.y);
		}

		m_coordinates = minValues - Vector2f(0.0f, m_style->LineWidth);
		m_canvasCollisionBoxSize = maxValues - minValues + Vector2f(0.0f, m_style->LineWidth * 2);
	}



	UIWidget* UILinkStrip::getStartTarget()
	{
		return m_startTarget;
	}

	UIWidget* UILinkStrip::getEndTarget()
	{
		return m_endTarget;
	}

	void UILinkStrip::updateStartPoint(const CanvasCoordinates& StartPoint)
	{
		m_startPoint = StartPoint;
		calcControlPoints();
	}

	void UILinkStrip::updateEndPoint(const CanvasCoordinates& EndPoint)
	{
		m_endPoint = EndPoint;
		calcControlPoints();
	}

	void UILinkStrip::update( UIMouse* currentMouse )
	{
		UIWidget::update(currentMouse);
	}

	bool UILinkStrip::isGoodStrip()
	{
		return (m_endTarget != nullptr && m_startTarget != nullptr);
	}

	void UILinkStrip::updateStartTarget(UIWidget* StartTarget)
	{
		m_startTarget = StartTarget;
	}

	void UILinkStrip::updateEndTarget(UIWidget* EndTarget)
	{
		m_endTarget = EndTarget;
	}
}

