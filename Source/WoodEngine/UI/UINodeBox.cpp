#include "../stdafx.h"

#include "UINodeBox.hpp"
#include "UICanvas.hpp"

namespace woodman
{
	

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------NODEBOX----------------NODEBOX---------------NODEBOX----------NODEBOX------------NODEBOX-----------NODEBOX----------NODEBOX--------NODEBOX-----------NODEBOX--------
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	UINodeBox::UINodeBox(UICanvas* ParentCanvas,
		UIWidget* parentWidget,
		const std::string& name,
		HashedString uniqueID,
		float RelativeLayer,
		const Vector2f& canvasCoordinates)
			: UIWidget(ParentCanvas, parentWidget, name, uniqueID, RelativeLayer, canvasCoordinates)
	{

	}

	void UINodeBox::Initialize()
	{
		m_nodeBoxShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\NodeBox");

		Texture::TextureFormat tf;
		tf.MagFilter = GL_LINEAR;
		tf.MinFilter = GL_LINEAR;
		tf.MipMapTexture = false;
		m_borderFilterIdle = Texture::CreateOrGetTexture(ASSETS + "FilterFunctions\\Filter_1D_Blue.png", tf);
		m_borderFilterHover = Texture::CreateOrGetTexture(ASSETS + "FilterFunctions\\Filter_1D_Blue_Yellow.png", tf);
		m_borderFilterDrag = Texture::CreateOrGetTexture(ASSETS + "FilterFunctions\\Filter_1D_Teal_Yellow.png", tf);
		m_borderFilterSelected = Texture::CreateOrGetTexture(ASSETS + "FilterFunctions\\Filter_1D_Green.png", tf);

		UIWidget::Initialize();
	}

	void UINodeBox::render( UIMouse* currentMouse, float ParentLayer )
	{
		float zoomScale = m_parentCanvas->getZoomScale();
		float layer = ParentLayer - m_relativeLayer;

		//--------------------------
		// Render the box
		//--------------------------
		m_nodeBoxShader->load();

		Vector2f NodeBoxMinScreen, NodeBoxMaxScreen, NodeBoxMin(m_coordinates.x, m_coordinates.y), NodeBoxMax(NodeBoxMin + m_canvasCollisionBoxSize );

		m_parentCanvas->mapPointToScreenSpace(NodeBoxMin, NodeBoxMinScreen);
		m_parentCanvas->mapPointToScreenSpace(NodeBoxMax, NodeBoxMaxScreen);

		Vector2f NodeBoxSizeScreen = NodeBoxMaxScreen - NodeBoxMinScreen;
		AABB2D CanvasScreenSpace = m_parentCanvas->getScreenSpace();

		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_position),NodeBoxMinScreen.x, NodeBoxMinScreen.y );
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_size), NodeBoxSizeScreen.x, NodeBoxSizeScreen.y);
		glUniform1f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_cornerSize), m_style->NodeBoxCornerSize * zoomScale );
		glUniform1f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_borderSize), m_style->NodeBoxBorderLength * zoomScale);
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_screenMin), CanvasScreenSpace.m_vMin.x, CanvasScreenSpace.m_vMin.y  );
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_screenMax), CanvasScreenSpace.m_vMax.x, CanvasScreenSpace.m_vMax.y);
		glUniform1i(m_nodeBoxShader->getUniformID(HASHED_STRING_u_borderFilter), 0);
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );
		float mapLayer = layer / g_MaxLayer;
		m_nodeBoxShader->SetUniformFloat(HASHED_STRING_u_layer, mapLayer, 1);


		if( currentMouse->selectedWidget == this)
		{
			if(currentMouse->isPressed)
				Texture::ApplyTexture(m_borderFilterDrag);
			else
				Texture::ApplyTexture(m_borderFilterSelected);
		}
		else if( currentMouse->hoveringWidget == this )
		{
			Texture::ApplyTexture(m_borderFilterHover);
		}
		else
		{
			Texture::ApplyTexture(m_borderFilterIdle);
		}

		m_nodeBoxShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

		glDrawArrays( GL_QUADS, 0, 4);

		m_nodeBoxShader->disableAttribute(HASHED_STRING_in_position);

		//--------------------------
		//render the Title
		//--------------------------
		Vector2f TitleOffset = Vector2f( (m_canvasCollisionBoxSize.x - m_style->NodeBoxCornerSize) * .5f + m_style->NodeBoxCornerSize,
											m_canvasCollisionBoxSize.y - m_style->NodeBoxBorderLength - m_style->TitleSize);
		Vector2f TextCanvas(m_coordinates + TitleOffset ), TextScreen;
		m_parentCanvas->mapPointToScreenSpace(TextCanvas, TextScreen);

		// 		Vector2f( (nodeBoxToRender->size.x + style->NodeBoxCornerSize) * .5f, 
		// 			nodeBoxToRender->size.y - style->NodeBoxBorderLength - style->TitleSize) ) * zoomScale,

		Font::DrawTextToScreen(m_name, 
			m_style->TitleTextFont,
			m_style->TitleColor,
			TextScreen,
			m_style->TitleSize * zoomScale,
			ALIGNMENT_CENTER);


		//BASE RENDER CALL
		UIWidget::render(currentMouse, ParentLayer);
	}


	void UINodeBox::move(const Vector2f& amountToMove)
	{
		UIWidget::move(amountToMove);
		
		if(m_callBackRecipient != nullptr)
		{
			m_callBackRecipient->setPosition(m_coordinates);
		}
	}

	void UINodeBox::MouseDrag(UIMouse* currentMouse)
	{
		move(m_parentCanvas->getCurrentMouseCanvasPosition() - m_parentCanvas->getPrevMouseCanvasPosition() );
	}

	void UINodeBox::setCallBackRecipient( UINodeBoxCallBackRecipient* recipient )
	{
		m_callBackRecipient = recipient;

		if(m_callBackRecipient != nullptr)
		{
			m_callBackRecipient->setPosition(m_coordinates);
		}

	}
}