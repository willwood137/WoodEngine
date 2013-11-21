#include "../stdafx.h"

#include "UINodeLink.hpp"
#include "UICanvas.hpp"
#include "UIWidget.hpp"
#include "UIController.hpp"
#include "..\Math\Vector2.hpp"

namespace woodman
{

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------NODELINk----------------NODELINk---------------NODELINk----------NODELINk------------NODELINk-----------NODELINk----------NODELINk--------NODELINk-----------NODELINk--------
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



	std::weak_ptr<UINodeLink> UINodeLink::CreateUINodeLink( const std::string& _Title, 
		bool _outLink, 
		UINodeLinkCallBackBase* callbackRecipient,
		UIController* _ParentController, 
		std::weak_ptr<UICanvas> _ParentCanvas, 
		std::weak_ptr<UIWidget> _ParentWidget, 
		const HashedString& _ID, 
		float _RelativeLayer, 
		const Vector2f& _RelativeCoordinates, 
		const Vector2f& _CollisionSize )
	{
		assert(_ParentController != nullptr);
		std::shared_ptr<UINodeLink> newWidget(new UINodeLink(_Title, _outLink, callbackRecipient, _ParentController, _ParentCanvas, _ParentWidget, _ID, _RelativeLayer, _RelativeCoordinates, _CollisionSize) );
		_ParentController->RegisterUIWidget(std::dynamic_pointer_cast<UIWidget>(newWidget) );
		newWidget->Initialize();
		return std::weak_ptr<UINodeLink>(newWidget);
	}

	UINodeLink::UINodeLink( const std::string& _Title, 
		bool _outLink, 
		UINodeLinkCallBackBase* callbackRecipient,
		UIController* _ParentController, 
		std::weak_ptr<UICanvas> _ParentCanvas, 
		std::weak_ptr<UIWidget> _ParentWidget, 
		const HashedString& _ID, 
		float _RelativeLayer, 
		const Vector2f& _RelativeCoordinates, 
		const Vector2f& _CollisionSize )
		: UIWidget( _ParentController, _ParentCanvas, _ParentWidget, _ID, _RelativeLayer, _RelativeCoordinates, _CollisionSize ),
		m_title(_Title),
		m_callBackRecipient(callbackRecipient)
	{
		assert(m_callBackRecipient != nullptr);
	}

	void UINodeLink::Initialize()
	{
		m_slotShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\LinkNode");
	}

	void UINodeLink::render(std::shared_ptr<UIMouse> currentMouse )
	{
		float zoomScale = m_parentCanvas.lock()->getZoomScale();


		ALIGNMENT ali;
		float boxOffset;
		float textOffset;

		if(m_isOutLink)
		{
			ali = ALIGNMENT_RIGHT;
			boxOffset = -10.0f;
			textOffset = -m_style->NodeBoxBorderLength -4.0f;
		}
		else
		{
			ali = ALIGNMENT_LEFT;
			boxOffset = -8.0f;
			textOffset = m_style->NodeBoxBorderLength + 4.0f;
		}

		Vector2f TextCanvas( getAbsoluteCoordinates() + Vector2f(textOffset, 0.0f)), TextScreen;
		m_parentCanvas.lock()->mapPointToScreenSpace(TextCanvas, TextScreen);

		Font::DrawTextToScreen( m_title, 
			m_style->subTitleTextFont,
			m_style->subTitleColor,
			TextScreen,
			m_style->subTitleSize * zoomScale,
			ali);

		//due canvas to screen conversions
		Vector2f LinkBoxMinScreen, LinkBoxMaxScreen, LinkBoxMin( getAbsoluteCoordinates() + Vector2f(boxOffset, -5) ), LinkBoxMax(LinkBoxMin + getCollisionSize() );

		m_parentCanvas.lock()->mapPointToScreenSpace(LinkBoxMin, LinkBoxMinScreen);
		m_parentCanvas.lock()->mapPointToScreenSpace(LinkBoxMax, LinkBoxMaxScreen);

		Vector2f LinkBoxSizeScreen = LinkBoxMaxScreen - LinkBoxMinScreen;

		AABB2D screenSpaceBounds = m_parentCanvas.lock()->getScreenSpace();

		m_slotShader->load();
		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_position), LinkBoxMinScreen.x, LinkBoxMinScreen.y );
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_size), LinkBoxSizeScreen.x, LinkBoxSizeScreen.y);
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_screenMin), screenSpaceBounds.m_vMin.x, screenSpaceBounds.m_vMin.y  );
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_screenMax), screenSpaceBounds.m_vMax.x, screenSpaceBounds.m_vMax.y);
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

		float glLayer = getAbsoluteLayer() / g_MaxLayer;
		m_slotShader->SetUniformFloat(HASHED_STRING_u_layer, glLayer, 1);

		if(currentMouse->selectedWidget.lock().get() == this)
		{
			if(currentMouse->isPressed)
				glUniform4f(m_slotShader->getUniformID(HASHED_STRING_u_color), 0.0f, 1.0f, .5f, 1.0f);
			else
				glUniform4f(m_slotShader->getUniformID(HASHED_STRING_u_color), 1.0f, .0f, 1.0f, 1.0f);
		}
		else if( currentMouse->hoveringWidget.lock().get() == this )
		{
			glUniform4f(m_slotShader->getUniformID(HASHED_STRING_u_color), 0.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			glUniform4f(m_slotShader->getUniformID(HASHED_STRING_u_color), .7f, .2f, .2f, 1.0f);
		}

		m_slotShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

		glDrawArrays( GL_QUADS, 0, 4);

		m_slotShader->disableAttribute(HASHED_STRING_in_position);



		UIWidget::render(currentMouse);
	}


	void UINodeLink::MouseRelease( std::shared_ptr<UIMouse> currentMouse)
	{
		UIWidget::MouseRelease(currentMouse);
	}

	void UINodeLink::MouseRClick( std::shared_ptr<UIMouse> currentMouse)
	{
		UIWidget::MouseRClick(currentMouse);
	}

	void UINodeLink::MouseClick( std::shared_ptr<UIMouse> currentMouse)
	{
		UIWidget::MouseClick(currentMouse);
	}

	void UINodeLink::MouseDrag( std::shared_ptr<UIMouse> currentMouse)
	{
		if(m_draggingStrip != nullptr)
		{
			if(m_isOutLink)
				m_draggingStrip->updateEndPoint(m_parentCanvas.lock()->getCurrentMouseCanvasPosition());
			else
				m_draggingStrip->updateStartPoint(m_parentCanvas.lock()->getCurrentMouseCanvasPosition());
		}

		UIWidget::MouseDrag(currentMouse);
	}

	void UINodeLink::move(const Vector2f& amountToMove)
	{
		UIWidget::move(amountToMove);
	}


	void UINodeLink::update(std::shared_ptr<UIMouse> currentMouse)
	{
		UIWidget::update(currentMouse);
	}

#pragma region UINodeLink_Getters

	bool UINodeLink::IsOutLink() const
	{
		return m_isOutLink;
	}

	unsigned int UINodeLink::getTypeSize()
	{
		return m_dataTypeSize;
	}

	DataType* UINodeLink::getDataType()
	{
		return m_parentDataType;
	}

	UINodeLinkCallBackBase* UINodeLink::getCallBackRecipient()
	{
		return m_callBackRecipient;
	}

#pragma endregion
#pragma region UINodeLink_Setters

	void UINodeLink::setDataType(DataType* dType)
	{
		m_parentDataType = dType;
		m_dataTypeSize = dType->maxSize;
	}

	void UINodeLink::setDataSize(unsigned int size)
	{
		m_dataTypeSize = size;
	}


	void UINodeLink::setDraggingStrip(std::shared_ptr<UILinkStrip> linkStrip)
	{
		m_draggingStrip = linkStrip;
	}

	bool UINodeLink::pair( std::shared_ptr<UINodeLink> outLink, std::shared_ptr<UINodeLink> inLink, std::shared_ptr<UILinkStrip> strip /*= nullptr*/ )
	{
		assert(outLink->m_isOutLink && !inLink->m_isOutLink);

		if(isPairLegal(outLink, inLink))
		{

			if(strip == nullptr)			//strip is nullptr so we have to make one
			{
				strip = UILinkStrip::CreateUILinkStrip(outLink->getAbsoluteCoordinates(), 
					inLink->getAbsoluteCoordinates(),
					outLink->getParentController(),
					outLink->getParentCanvas(),
					std::weak_ptr<UIWidget>(),
					HashedString( "LINK_" + outLink->getUniqueID().m_string + inLink->getUniqueID().m_string),
					100,
					Vector2f(0.0f, 0.0f),
					Vector2f(0.0f, 0.0f) ).lock();
			}
			strip->updateEndTarget(inLink);
			strip->updateEndTarget(outLink);
			
			//pair outLink
			outLink->m_outLinkStrips.insert(strip);

			//pair inLink
			unPair(inLink);
			inLink->m_inLinkStrip = strip;
			inLink->m_callBackRecipient->CallBackLinkToNodeSlot(outLink->m_callBackRecipient);    // update its data member

			return true;
		}
		else 
		{
			return false;
		}

		assert(false);
	}

	bool UINodeLink::isPairLegal( std::shared_ptr<UINodeLink> outLink, std::shared_ptr<UINodeLink> inLink )
	{

		if( !outLink->m_isOutLink || inLink->m_isOutLink)
		{
			return false;
		}

		if( outLink->m_parentDataType->type != inLink->m_parentDataType->type )
		{
			return false;
		}

		if( outLink->m_dataTypeSize != inLink->m_dataTypeSize )
		{
			return false;
		}

		return true;
	}

	void UINodeLink::unPair( std::shared_ptr<UINodeLink> Link )
	{
		if(Link->m_isOutLink)
		{
			for(auto it = Link->m_outLinkStrips.begin(); it != Link->m_outLinkStrips.end(); )
			{
				std::shared_ptr<UILinkStrip> strip =  *(Link->m_outLinkStrips.erase(it++));
				(strip)->getParentController()->UnRegisterUIWidget( (strip)->getUniqueID() );
			}
		}
		else
		{
			if(Link->m_inLinkStrip != nullptr)
			{

				Link->getParentController()->UnRegisterUIWidget( Link->m_inLinkStrip->getUniqueID() );
				Link->m_inLinkStrip = nullptr;
			}
		}
	}

	void UINodeLink::removeLinkStrip( std::shared_ptr<UILinkStrip> strip )
	{
		if( m_isOutLink)
		{
			m_outLinkStrips.erase(strip);
		}
		else
		{
			if(m_inLinkStrip == strip)
			{
				m_inLinkStrip = nullptr;
			}
		}
	}



#pragma endregion

}