#include "..\stdafx.h"

#include "UIWidgetContainer.hpp"
#include "UICanvas.hpp"

namespace woodman
{


	UIWidgetContainer::UIWidgetContainer( UICanvas* ParentCanvas, 
		UIWidget* parentWidget, 
		const std::string& name, 
		float RelativeLayer, 
		const Vector2f& relativeCoordinates )
		:UIWidget(ParentCanvas, parentWidget, name, RelativeLayer, relativeCoordinates)
	{

	}



	void UIWidgetContainer::render( UIMouse* currentMouse, const AABB2D& CanvasBounds ) const
	{
		
		//render outer Box
		Vector2f OuterBoxMinScreen, OuterBoxMaxScreen, OuterBoxMin(getAbsoluteCoordinates()), OuterBoxMax(OuterBoxMin + getSize());
		AABB2D screenSpaceBounds = getParentCanvas()->getScreenSpace();
		getParentCanvas()->mapPointToScreenSpace(OuterBoxMin, OuterBoxMinScreen);
		getParentCanvas()->mapPointToScreenSpace(OuterBoxMax, OuterBoxMaxScreen);
		Vector2f OuterBoxSizeScreen = OuterBoxMaxScreen - OuterBoxMinScreen;

		Vector2f inverseScreenRes( 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

		m_boxShader->load();
		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		m_boxShader->SetUniformVector2(HASHED_STRING_u_position, OuterBoxMinScreen, 1);
		m_boxShader->SetUniformVector2(HASHED_STRING_u_size, OuterBoxSizeScreen, 1);
		m_boxShader->SetUniformVector2(HASHED_STRING_u_screenMin, screenSpaceBounds.m_vMin, 1);
		m_boxShader->SetUniformVector2(HASHED_STRING_u_screenMax, screenSpaceBounds.m_vMax, 1);
		m_boxShader->SetUniformVector2(HASHED_STRING_u_inverseScreenResolution, inverseScreenRes, 1 );
		m_boxShader->SetUniformVector4(HASHED_STRING_u_borderColor, RGBA(0.0f, 0.0f, 0.0f, 1.0f), 1 );
		m_boxShader->SetUniformVector4(HASHED_STRING_u_color, RGBA(0.518f, 0.812f, 0.941f, 1.0f), 1 );
		float borderSize = 4.0f;
		float glLayer = .1f;
		m_boxShader->SetUniformFloat(HASHED_STRING_u_borderSize, borderSize, 1);
		m_boxShader->SetUniformFloat(HASHED_STRING_u_layer, glLayer, 1);

		m_boxShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);
		glDrawArrays( GL_QUADS, 0, 4);
		m_boxShader->disableAttribute(HASHED_STRING_in_position);


		UIWidget::render(currentMouse, CanvasBounds);
	}

	void UIWidgetContainer::Initialize()
	{
		m_boxShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\BoxShader");
	}

}