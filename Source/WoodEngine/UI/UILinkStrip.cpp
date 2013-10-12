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
		float RelativeLayer,
		PropertyType pType,
		unsigned int typeSize,
		CanvasCoordinates startPoint,
		CanvasCoordinates endPoint)
		:UIWidget(ParentCanvas, parentWidget, name, uniqueID, RelativeLayer),
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
		m_crossCanvasLineStripShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\CrossCanvasLineShader");
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

	void UILinkStrip::render(UIMouse* currentMouse, float ParentLayer)
	{

		float layer = ParentLayer - m_relativeLayer;

		if(m_startPoint.getOwningCanvas() == nullptr || m_endPoint.getOwningCanvas() == nullptr)
			return;

		if(m_startPoint.getOwningCanvas() == m_endPoint.getOwningCanvas())
		{
			renderIntraCanvas(currentMouse, layer);
		}
		else
		{
			renderCrossCanvas(currentMouse, layer);
		}

		UIWidget::render(currentMouse, layer);
	}


	void UILinkStrip::renderCrossCanvas(UIMouse* currentMouse, float layer)
	{

		
	}

	void UILinkStrip::renderIntraCanvas(UIMouse* currentMouse, float layer)
	{
		m_lineStripShader->load();


		std::vector< Vector2f > canvasPoints;

		//canvasPoints.resize(m_controlPoints.size());

		for(auto it = m_controlPoints.begin(); it != m_controlPoints.end(); ++it)
		{
			canvasPoints.push_back( (*it).coordinatesCanvasSpace );
		}

		Vector2f NodeBoxMinScreen, NodeBoxMaxScreen;

		m_parentCanvas->mapPointToScreenSpace(m_coordinates, NodeBoxMinScreen);
		m_parentCanvas->mapPointToScreenSpace(m_coordinates + m_canvasCollisionBoxSize, NodeBoxMaxScreen);

		Vector2f NodeBoxSizeScreen = NodeBoxMaxScreen - NodeBoxMinScreen;
		AABB2D screenSpaceBounds = m_parentCanvas->getScreenSpace();

		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		int filterSlot = 0;
		int numPoints = canvasPoints.size();
		int numLines = numPoints - 1;
		Vector2f inverseScreen( 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );
		Vector3f minScreenPos(NodeBoxMinScreen, layer);

		m_lineStripShader->SetUniformVector3(HASHED_STRING_u_position, minScreenPos, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_size, NodeBoxSizeScreen, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_screenMin, screenSpaceBounds.m_vMin, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_screenMax, screenSpaceBounds.m_vMax, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_canvasMin, m_coordinates, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_canvasMax, m_coordinates + m_canvasCollisionBoxSize, 1);
		m_lineStripShader->SetUniformInt(HASHED_STRING_u_filter, filterSlot, 1);

		float screenLayer = layer / g_MaxLayer;
		m_lineStripShader->SetUniformFloat(HASHED_STRING_u_layer, screenLayer, 1);


		//glUniform1i(m_lineStripShader->getUniformID(HASHED_STRING_u_filter), 0);
		//glUniform1i(m_lineStripShader->getUniformID(HASHED_STRING_u_numLines), numLines);
		//glUniform1f(m_lineStripShader->getUniformID(HASHED_STRING_u_lineSize), m_style->LineWidth);

		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_lineSegmentsStrip, canvasPoints[0], numPoints );
		//glUniform2fv(m_lineStripShader->getUniformID(HASHED_STRING_u_lineSegmentsStrip), numPoints, reinterpret_cast<GLfloat*>(canvasPoints.data()) );
		m_lineStripShader->SetUniformInt(HASHED_STRING_u_numLines, numLines, 1);
		m_lineStripShader->SetUniformFloat(HASHED_STRING_u_lineSize, m_style->LineWidth, 1);

		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_inverseScreenResolution, inverseScreen, 1 );

		Texture::ApplyTexture(m_filterTexture);

		m_lineStripShader->renderSimpleQuad();
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

		Vector2f minValues, maxValues;
		if(m_endPoint.getOwningCanvas() != m_startPoint.getOwningCanvas())
		{
			minValues = Vector2f(m_startPoint.getScreenCoordinates());
			maxValues = Vector2f(m_endPoint.getScreenCoordinates());
		}
		else
		{
			minValues = Vector2f(m_startPoint.coordinatesCanvasSpace);
			maxValues = Vector2f(m_endPoint.coordinatesCanvasSpace);

		}

		for(auto it = m_controlPoints.begin(); it != m_controlPoints.end(); ++it)
		{
			Vector2f screenCoords = (*it).coordinatesCanvasSpace;
			minValues.x = min(minValues.x, screenCoords.x);
			minValues.y = min(minValues.y, screenCoords.y);
			maxValues.x = max(maxValues.x, screenCoords.x);
			maxValues.y = max(maxValues.y, screenCoords.y);
		}
		
		m_coordinates = minValues - Vector2f(0.0f, m_style->LineWidth * .5f);
		m_canvasCollisionBoxSize = maxValues - minValues + Vector2f(0.0f, m_style->LineWidth);
		calcFullCollisionBox();
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

