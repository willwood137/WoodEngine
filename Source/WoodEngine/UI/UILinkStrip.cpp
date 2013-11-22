#include "../stdafx.h"

#include "UILinkStrip.hpp"
#include "UICanvas.hpp"
#include "UINodeLink.hpp"
#include "UIController.hpp"
namespace woodman
{
	std::weak_ptr<UILinkStrip> UILinkStrip::CreateUILinkStrip( const Vector2f& startPoint, 
		const Vector2f& endPoint, 
		UIController* _ParentController, 
		std::weak_ptr<UICanvas> _ParentCanvas, 
		std::weak_ptr<UIWidget> _ParentWidget, 
		const HashedString& _ID, 
		float _RelativeLayer, 
		const Vector2f& _RelativeCoordinates, 
		const Vector2f& _CollisionSize )
	{
		assert(_ParentController != nullptr);
		std::shared_ptr<UILinkStrip> newWidget(new UILinkStrip(startPoint, endPoint, _ParentController, _ParentCanvas, _ParentWidget, _ID, _RelativeLayer, _RelativeCoordinates, _CollisionSize) );
		_ParentController->RegisterUIWidget(std::dynamic_pointer_cast<UIWidget>(newWidget) );
		newWidget->Initialize();
		return std::weak_ptr<UILinkStrip>(newWidget);
	}

	UILinkStrip::UILinkStrip( const Vector2f& startPoint, 
		const Vector2f& endPoint, 
		UIController* _ParentController, 
		std::weak_ptr<UICanvas> _ParentCanvas,
		std::weak_ptr<UIWidget> _ParentWidget,
		const HashedString& _ID, 
		float _RelativeLayer, 
		const Vector2f& _RelativeCoordinates,
		const Vector2f& _CollisionSize )
		: UIWidget( _ParentController, _ParentCanvas, _ParentWidget, _ID, _RelativeLayer, _RelativeCoordinates, _CollisionSize ),
		m_startPoint(startPoint),
		m_endPoint(endPoint),
		m_startVector(30.0f, 0.0f),
		m_endVector(-30.0f, 0.0f)
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

	void UILinkStrip::render(std::shared_ptr<UIMouse> currentMouse)
	{
		m_lineStripShader->load();

		std::vector< Vector2f > canvasPoints;

		//canvasPoints.resize(m_controlPoints.size());

		for(auto it = m_controlPoints.begin(); it != m_controlPoints.end(); ++it)
		{
			canvasPoints.push_back( (*it) );
		}

		Vector2f NodeBoxMinScreen, NodeBoxMaxScreen;

		m_parentCanvas.lock()->mapPointToScreenSpace(getAbsoluteCoordinates(), NodeBoxMinScreen);
		m_parentCanvas.lock()->mapPointToScreenSpace(getAbsoluteCoordinates() + getCollisionSize(), NodeBoxMaxScreen);

		Vector2f NodeBoxSizeScreen = NodeBoxMaxScreen - NodeBoxMinScreen;
		AABB2D screenSpaceBounds = m_parentCanvas.lock()->getScreenSpace();

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
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_canvasMin, getAbsoluteCollisionBox().m_vMin, 1);
		m_lineStripShader->SetUniformVector2(HASHED_STRING_u_canvasMax, getAbsoluteCollisionBox().m_vMax, 1);
		m_lineStripShader->SetUniformInt(HASHED_STRING_u_filter, filterSlot, 1);

		float screenLayer = getAbsoluteLayer() / g_MaxLayer;
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


		UIWidget::render(currentMouse);
	}

	void UILinkStrip::calcControlPoints()
	{
		m_controlPoints.clear();

		Vector2f p2(m_startPoint);
		p2 += m_startVector;

		Vector2f p3(m_endPoint);
		p3 += m_endVector;

		m_controlPoints.push_back(m_startPoint);
		m_controlPoints.push_back(p2);
		m_controlPoints.push_back(p3);
		m_controlPoints.push_back(m_endPoint);

		Vector2f minValues, maxValues;
		minValues = Vector2f(m_startPoint);
		maxValues = Vector2f(m_endPoint);

		

		for(auto it = m_controlPoints.begin(); it != m_controlPoints.end(); ++it)
		{
			Vector2f screenCoords = (*it);
			minValues.x = min(minValues.x, screenCoords.x);
			minValues.y = min(minValues.y, screenCoords.y);
			maxValues.x = max(maxValues.x, screenCoords.x);
			maxValues.y = max(maxValues.y, screenCoords.y);
		}
		
		setRelativeCoordinates( minValues - Vector2f(0.0f, m_style->LineWidth * .5f) );
		setCollisionSize( maxValues - minValues + Vector2f(0.0f, m_style->LineWidth) );
		calcFullCollisionBox();

	}



	std::shared_ptr<UIWidget> UILinkStrip::getStartTarget()
	{
		return m_startTarget.lock();
	}

	std::shared_ptr<UIWidget> UILinkStrip::getEndTarget()
	{
		return m_endTarget.lock();
	}

	void UILinkStrip::updateStartPoint(const Vector2f& StartPoint)
	{
		m_startPoint = StartPoint;
		calcControlPoints();
	}

	void UILinkStrip::updateEndPoint(const Vector2f& EndPoint)
	{
		m_endPoint = EndPoint;
		calcControlPoints();
	}

	void UILinkStrip::update( std::shared_ptr<UIMouse> currentMouse )
	{
		UIWidget::update(currentMouse);
	}

	bool UILinkStrip::isGoodStrip()
	{
		return (!m_endTarget.expired() && !m_startTarget.expired());
	}

	void UILinkStrip::updateStartTarget( std::shared_ptr<UIWidget> StartTarget )
	{
		m_startTarget = StartTarget;
	}

	void UILinkStrip::updateEndTarget( std::shared_ptr<UIWidget> EndTarget )
	{
		m_endTarget = EndTarget;
	}

	UILinkStrip::~UILinkStrip()
	{
		std::shared_ptr<UINodeLink> inLink = std::dynamic_pointer_cast<UINodeLink>(m_endTarget.lock());
		std::shared_ptr<UINodeLink> outLink = std::dynamic_pointer_cast<UINodeLink>(m_startTarget.lock());
		
		if(inLink != nullptr && outLink != nullptr)
		{
			inLink->getCallBackRecipient()->CallBackLinkToNodeSlot(nullptr);
		}

		if(inLink != nullptr)
		{
			inLink->removeLinkStrip(std::dynamic_pointer_cast<UILinkStrip>(m_selfPtr.lock()));
		}

		if(outLink != nullptr)
		{
			outLink->removeLinkStrip(std::dynamic_pointer_cast<UILinkStrip>(m_selfPtr.lock()));
		}
	}

}

