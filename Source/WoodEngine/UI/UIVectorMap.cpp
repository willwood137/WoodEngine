#include "../stdafx.h"

#include "UIVectorMap.hpp"
#include "UICanvas.hpp"

namespace woodman
{

	RGBA getVectorFieldColor(VectorField vField)
	{
		switch(vField)
		{
		case VECTORFIELD_X:
			return RGBA(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case VECTORFIELD_Y:
			return RGBA(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		case VECTORFIELD_Z:
			return RGBA(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		case VECTORFIELD_W:
			return RGBA(1.0f, 0.8f, 0.0f, 1.0f);
			break;
		case VECTORFIELD_0:
			return RGBA(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case VECTORFIELD_1:
			return RGBA(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
	}


	UIVectorMap::UIVectorMap(UICanvas* ParentCanvas, UIWidget* parentWidget, const std::string& name, float RelativeLayer, Vector2f coordinates)
		:UIWidget(ParentCanvas, parentWidget, name, RelativeLayer, coordinates)
	{

	}

	void UIVectorMap::updateExitSize(unsigned int size)
	{
		m_mapping.outSize = clamp<unsigned int>(size, 1, 4);;

		m_mapping.mapping.clear();
		//setup default values
		switch(m_mapping.outSize)
		{
		case 4:
			{
				switch(m_mapping.inSize)
				{
				case 4:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					m_mapping.mapping.push_back(VECTORFIELD_Z);
					m_mapping.mapping.push_back(VECTORFIELD_W);
					break;
				case 3:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					m_mapping.mapping.push_back(VECTORFIELD_Z);
					m_mapping.mapping.push_back(VECTORFIELD_1);
					break;
				case 2:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					m_mapping.mapping.push_back(VECTORFIELD_0);
					m_mapping.mapping.push_back(VECTORFIELD_1);
					break;
				case 1:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_X);
					break;

				}
			}
			break;
		case 3:
			{
				switch(m_mapping.inSize)
				{
				case 4:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					m_mapping.mapping.push_back(VECTORFIELD_Z);
					break;
				case 3:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					m_mapping.mapping.push_back(VECTORFIELD_Z);
					break;
				case 2:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					m_mapping.mapping.push_back(VECTORFIELD_0);
					break;
				case 1:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_X);
					break;

				}
			}
			break;
		case 2:
			{
				switch(m_mapping.inSize)
				{
				case 4:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					break;
				case 3:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					break;
				case 2:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_Y);
					break;
				case 1:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					m_mapping.mapping.push_back(VECTORFIELD_X);
					break;

				}
			}
			break;
		case 1:
			{
				switch(m_mapping.inSize)
				{
				case 4:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					break;
				case 3:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					break;
				case 2:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					break;
				case 1:
					m_mapping.mapping.push_back(VECTORFIELD_X);
					break;
				}
			}
			break;
		}

	}

	void UIVectorMap::updateInSize(unsigned int size)
	{
		m_mapping.inSize = size;
	}

	void UIVectorMap::Initialize()
	{
		m_boxShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\BoxShader");
	}


	void UIVectorMap::render(UIMouse* currentMouse, const AABB2D& bounds ) const
	{
		
		size_t numBoxes = m_mapping.mapping.size();
		
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




		//draw inner boxes
		std::vector<RGBA> colors(numBoxes);
		std::vector<AABB2D> Boxes(numBoxes);

		for(size_t i = 0; i < numBoxes; ++i)
		{
			RGBA color = getVectorFieldColor(m_mapping.mapping[i]);

			float y = getAbsoluteCoordinates().y + getSize().y - 32.0f * static_cast<float>(i) - 24.0f;
			float x = getAbsoluteCoordinates().x + 6.0f;

			Vector2f BoxMinScreen, BoxMaxScreen, BoxMin(x, y), BoxMax(BoxMin + Vector2f(20.0f, 20.0f));
			getParentCanvas()->mapPointToScreenSpace(BoxMin, BoxMinScreen);
			getParentCanvas()->mapPointToScreenSpace(BoxMax, BoxMaxScreen);
			Vector2f BoxSizeScreen = BoxMaxScreen - BoxMinScreen;


			m_boxShader->load();
			glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
			m_boxShader->SetUniformVector2(HASHED_STRING_u_position, BoxMinScreen, 1);
			m_boxShader->SetUniformVector2(HASHED_STRING_u_size, BoxSizeScreen, 1);
			m_boxShader->SetUniformVector2(HASHED_STRING_u_screenMin, screenSpaceBounds.m_vMin, 1);
			m_boxShader->SetUniformVector2(HASHED_STRING_u_screenMax, screenSpaceBounds.m_vMax, 1);
			m_boxShader->SetUniformVector2(HASHED_STRING_u_inverseScreenResolution, inverseScreenRes, 1 );
			m_boxShader->SetUniformVector4(HASHED_STRING_u_borderColor, RGBA(0.0f, 0.0f, 0.0f, 1.0f), 1 );
			m_boxShader->SetUniformVector4(HASHED_STRING_u_color, color, 1 );
			float borderSize = 2.0f;
			float layer = .0f;
			m_boxShader->SetUniformFloat(HASHED_STRING_u_borderSize, borderSize, 1);
			m_boxShader->SetUniformFloat(HASHED_STRING_u_layer, layer, 1);

			m_boxShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);
			glDrawArrays( GL_QUADS, 0, 4);
			m_boxShader->disableAttribute(HASHED_STRING_in_position);
		}

		UIWidget::render(currentMouse, bounds);
	}
}