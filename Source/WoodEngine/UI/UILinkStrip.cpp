#include "../stdafx.h"

#include "UILinkStrip.hpp"
#include "UICanvas.hpp"
#include "UINodeLink.hpp"

namespace woodman
{
	UILinkStrip::UILinkStrip(UICanvas* ParentCanvas,
		UIWidget* parentWidget,
		const std::string& name,
		float RelativeLayer,
		PropertyType pType,
		unsigned int typeSize,
		CanvasCoordinates startPoint,
		CanvasCoordinates endPoint)
		: UIWidget(ParentCanvas, parentWidget, name, RelativeLayer, Vector2f(0.0f, 0.0f)),
		m_startVector(30.0f, 0.0f),
		m_endVector(-30.0f, 0.0f),
		m_propertyType(pType),
		m_typeSize(typeSize),
		m_startPoint(startPoint),
		m_endPoint(endPoint)
	{
		m_vectorMap = nullptr;
	}

	UILinkStrip::UILinkStrip(UICanvas* ParentCanvas,
		UIWidget* parentWidget,
		const std::string& name,
		float RelativeLayer,
		UINodeLink* OutLink,
		UINodeLink* InLink )
		:  UIWidget(ParentCanvas, parentWidget, name, RelativeLayer, Vector2f(0.0f, 0.0f)),
		m_startVector(30.0f, 0.0f),
		m_endVector(-30.0f, 0.0f),
		m_startTarget(OutLink),
		m_endTarget(InLink),
		m_startPoint(ParentCanvas, Vector2f(0.0f, 0.0f) ),
		m_endPoint(ParentCanvas, Vector2f(0.0f, 0.0f) )
	{
		m_propertyType = m_startTarget->getDataType()->type;
		m_vectorMap = new UIVectorMap(ParentCanvas, this, name + "_VectorMap", 1.0f, Vector2f(0.0f, 0.0f) );
		m_vectorMap->Initialize();
		m_vectorMap->updateInSize(OutLink->getTypeSize());
		m_vectorMap->updateExitSize(InLink->getTypeSize());
		m_vectorMap->setSize(Vector2f(32.0f, 32.0f));
		addChild(m_vectorMap);

		m_startPoint.coordinatesCanvasSpace = OutLink->getAbsoluteCoordinates() + Vector2f(0.0f, OutLink->getSize().y * .25f);
		m_endPoint.coordinatesCanvasSpace = InLink->getAbsoluteCoordinates() + Vector2f(0.0f, InLink->getSize().y * .25f);
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


	void UILinkStrip::render(UIMouse* currentMouse, const AABB2D& CanvasBounds) const
	{
		m_lineStripShader->load();


		std::vector< Vector2f > canvasPoints;

		//canvasPoints.resize(m_controlPoints.size());

		for(auto it = m_controlPoints.begin(); it != m_controlPoints.end(); ++it)
		{
			canvasPoints.push_back( (*it).coordinatesCanvasSpace );
		}

		Vector2f NodeBoxMinScreen, NodeBoxMaxScreen;

		getParentCanvas()->mapPointToScreenSpace(getAbsoluteCoordinates(), NodeBoxMinScreen);
		getParentCanvas()->mapPointToScreenSpace(getAbsoluteCoordinates() + getSize(), NodeBoxMaxScreen);

		Vector2f NodeBoxSizeScreen = NodeBoxMaxScreen - NodeBoxMinScreen;
		AABB2D screenSpaceBounds = getParentCanvas()->getScreenSpace();

		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		int filterSlot = 0;
		int numPoints = canvasPoints.size();
		int numLines = numPoints - 1;
		Vector2f inverseScreen( 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );
		Vector3f minScreenPos(NodeBoxMinScreen, getAbsoluteLayer());

		m_lineStripShader->SetUniformVector3(HASHED_STRING_u_position, minScreenPos, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_size, NodeBoxSizeScreen, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_screenMin, screenSpaceBounds.m_vMin, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_screenMax, screenSpaceBounds.m_vMax, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_canvasMin, getAbsoluteCoordinates(), 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_canvasMax, getAbsoluteCoordinates() + getSize(), 1);
		m_lineStripShader->SetUniformInt(HASHED_STRING_u_filter, filterSlot, 1);

		float screenLayer = getAbsoluteLayer() / g_MaxLayer;
		m_lineStripShader->SetUniformFloat(HASHED_STRING_u_layer, screenLayer, 1);


		//glUniform1i(m_lineStripShader->getUniformID(HASHED_STRING_u_filter), 0);
		//glUniform1i(m_lineStripShader->getUniformID(HASHED_STRING_u_numLines), numLines);
		//glUniform1f(m_lineStripShader->getUniformID(HASHED_STRING_u_lineSize), m_style->LineWidth);

		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_lineSegmentsStrip, canvasPoints[0], numPoints );
		//glUniform2fv(m_lineStripShader->getUniformID(HASHED_STRING_u_lineSegmentsStrip), numPoints, reinterpret_cast<GLfloat*>(canvasPoints.data()) );
		m_lineStripShader->SetUniformInt(HASHED_STRING_u_numLines, numLines, 1);
		m_lineStripShader->SetUniformFloat(HASHED_STRING_u_lineSize, getStyle()->LineWidth, 1);

		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_inverseScreenResolution, inverseScreen, 1 );

		std::shared_ptr<Texture> tempTexture = m_filterTexture;
		Texture::ApplyTexture(tempTexture);

		m_lineStripShader->renderSimpleQuad();

		UIWidget::render(currentMouse, CanvasBounds);
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
		
		setAbsoluteCoordinates(minValues - Vector2f(0.0f, getStyle()->LineWidth * .5f) );
		setSize(maxValues - minValues + Vector2f(0.0f, getStyle()->LineWidth) );
		calcFullCollisionBox();

		//Update vectormap
		Vector2f midPoint = (m_controlPoints[1].coordinatesCanvasSpace + m_controlPoints[2].coordinatesCanvasSpace) / 2.0f;
		
		if(m_vectorMap != nullptr)
			m_vectorMap->setRelativeCoordinates(midPoint - m_vectorMap->getSize() / 2.0f);
	}



	UINodeLink* UILinkStrip::getStartTarget()
	{
		return m_startTarget;
	}

	UINodeLink* UILinkStrip::getEndTarget()
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

	void UILinkStrip::updateData(UIMouse* currentMouse, const ParentInfo& parent_info )
	{
		UIWidget::update(currentMouse, parent_info);
	}

	bool UILinkStrip::isGoodStrip()
	{
		return (m_endTarget != nullptr && m_startTarget != nullptr);
	}

	void UILinkStrip::updateStartTarget(UINodeLink* StartTarget)
	{
		m_startTarget = StartTarget;
	}

	void UILinkStrip::updateEndTarget(UINodeLink* EndTarget)
	{
		m_endTarget = EndTarget;
	}

	void UILinkStrip::updateVectorMap()
	{
		m_vectorMap->updateInSize(m_startTarget->getTypeSize());
		m_vectorMap->updateExitSize(m_endTarget->getTypeSize());

		m_vectorMap->setSize( Vector2f(32.0f, 32.0f * static_cast<float>(m_vectorMap->getExitSize()) ) );
	}
}

