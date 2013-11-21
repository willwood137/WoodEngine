#include "../stdafx.h"

#include "UINodeBox.hpp"
#include "UINodeLink.hpp"
#include "UITextEntry.hpp"
#include "UICanvas.hpp"
#include "UIController.hpp"

namespace woodman
{
	

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------NODEBOX----------------NODEBOX---------------NODEBOX----------NODEBOX------------NODEBOX-----------NODEBOX----------NODEBOX--------NODEBOX-----------NODEBOX--------
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	std::weak_ptr<UINodeBox> UINodeBox::CreateUINodeBox( const std::string& _Title, 
		UIController* _ParentController, 
		std::weak_ptr<UICanvas> _ParentCanvas, 
		std::weak_ptr<UIWidget> _ParentWidget, 
		const HashedString& _ID, 
		float _RelativeLayer, 
		const Vector2f& _RelativeCoordinates, 
		const Vector2f& _CollisionSize )
	{
		assert(_ParentController != nullptr);
		std::shared_ptr<UINodeBox> newBox(new UINodeBox(_Title, _ParentController, _ParentCanvas, _ParentWidget, _ID, _RelativeLayer, _RelativeCoordinates, _CollisionSize) );
		_ParentController->RegisterUIWidget(std::dynamic_pointer_cast<UIWidget>(newBox) );
		newBox->Initialize();
		return std::weak_ptr<UINodeBox>(newBox);
	}

	UINodeBox::UINodeBox( const std::string& Title, 
		UIController* parentController, 
		std::weak_ptr<UICanvas> ParentCanvas, 
		std::weak_ptr<UIWidget> parentWidget, 
		HashedString uniqueID, 
		float RelativeLayer, 
		const Vector2f& relativeCoordinates, 
		const Vector2f& collisionSize )
		: UIWidget( parentController, ParentCanvas, parentWidget, uniqueID, RelativeLayer, relativeCoordinates, collisionSize ),
		m_title(Title)
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

		m_titleOffset = Vector2f(0.0f, 0.0f);
			//Vector2f( (m_canvasCollisionBoxSize.x - m_style->NodeBoxCornerSize) * .5f + m_style->NodeBoxCornerSize,
			//m_canvasCollisionBoxSize.y - m_style->NodeBoxBorderLength - m_style->TitleSize);
	}


	
	void UINodeBox::render( std::shared_ptr<UIMouse> currentMouse )
	{
		float zoomScale = m_parentCanvas.lock()->getZoomScale();

		//--------------------------
		// Render the box
		//--------------------------
		m_nodeBoxShader->load();

		Vector2f NodeBoxMinScreen, NodeBoxMaxScreen, NodeBoxMin(getAbsoluteCoordinates()), NodeBoxMax(NodeBoxMin + getCollisionSize() );

		m_parentCanvas.lock()->mapPointToScreenSpace(NodeBoxMin, NodeBoxMinScreen);
		m_parentCanvas.lock()->mapPointToScreenSpace(NodeBoxMax, NodeBoxMaxScreen);

		Vector2f NodeBoxSizeScreen = NodeBoxMaxScreen - NodeBoxMinScreen;
		AABB2D CanvasScreenSpace = m_parentCanvas.lock()->getScreenSpace();

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
		float mapLayer = getAbsoluteLayer() / g_MaxLayer;
		m_nodeBoxShader->SetUniformFloat(HASHED_STRING_u_layer, mapLayer, 1);


		if( currentMouse->selectedWidget.lock().get() == this)
		{
			if(currentMouse->isPressed)
				Texture::ApplyTexture(m_borderFilterDrag);
			else
				Texture::ApplyTexture(m_borderFilterSelected);
		}
		else if( currentMouse->hoveringWidget.lock().get() == this )
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
		
		Vector2f TextCanvas(getAbsoluteCoordinates() + m_titleOffset ), TextScreen;
		m_parentCanvas.lock()->mapPointToScreenSpace(TextCanvas, TextScreen);

		// 		Vector2f( (nodeBoxToRender->size.x + style->NodeBoxCornerSize) * .5f, 
		// 			nodeBoxToRender->size.y - style->NodeBoxBorderLength - style->TitleSize) ) * zoomScale,

		Font::DrawTextToScreen(m_title, 
			m_style->TitleTextFont,
			m_style->TitleColor,
			TextScreen,
			m_style->TitleSize * zoomScale,
			ALIGNMENT_CENTER);


		//BASE RENDER CALL
		UIWidget::render(currentMouse);

		if(currentMouse->selectedNodeBox.lock().get() == this)
		{
			for(auto it = m_dataFields.begin(); it != m_dataFields.end(); ++it)
			{
				(*it).lock()->setToRender(true);
			}
		}
	}


	void UINodeBox::move(const Vector2f& amountToMove)
	{
		UIWidget::move(amountToMove);
		
		if(m_callBackRecipient != nullptr)
		{
			m_callBackRecipient->setPosition(getAbsoluteCoordinates());
		}
	}

	void UINodeBox::MouseDrag(std::shared_ptr<UIMouse> currentMouse)
	{
		move(m_parentCanvas.lock()->getCurrentMouseCanvasPosition() - m_parentCanvas.lock()->getPrevMouseCanvasPosition() );
	}

	void UINodeBox::setCallBackRecipient( UINodeBoxCallBackRecipient* recipient )
	{
		m_callBackRecipient = recipient;

		if(m_callBackRecipient != nullptr)
		{
			m_callBackRecipient->setPosition(getAbsoluteCoordinates());
		}

	}

	void UINodeBox::MouseClick( std::shared_ptr<UIMouse> currentMouse )
	{
		currentMouse->selectedNodeBox = std::dynamic_pointer_cast<UINodeBox>(m_parentController->getUIWidgetByID(getUniqueID()).lock());
	}

	void UINodeBox::addDataField( std::weak_ptr<UITextEntry> field )
	{
		m_dataFields.push_back(field);
	}

	void UINodeBox::update( std::shared_ptr<UIMouse> currentMouse )
	{
		UIWidget::update(currentMouse);
		for(auto it = m_dataFields.begin(); it != m_dataFields.end(); ++it)
		{
			m_callBackRecipient->setDataField((*it).lock()->getUniqueID(), (*it).lock()->getValue() );
		}
	}

	

}