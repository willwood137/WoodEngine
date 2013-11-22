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
		m_callBackRecipient(callbackRecipient),
		m_isOutLink(_outLink)
	{
		assert(m_callBackRecipient != nullptr);
	}

	void UINodeLink::Initialize()
	{
		m_slotShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\LinkNode");
		UIWidget::Initialize();
	}

	void UINodeLink::render(std::shared_ptr<UIMouse> currentMouse )
	{
		float zoomScale = m_parentCanvas.lock()->getZoomScale();


		ALIGNMENT ali;
		
		if(m_isOutLink)
		{
			ali = ALIGNMENT_RIGHT;
		}
		else
		{
			ali = ALIGNMENT_LEFT;
		}


		Vector2f TextCanvas( getAbsoluteCoordinates() ), TextScreen;
		m_parentCanvas.lock()->mapPointToScreenSpace(TextCanvas, TextScreen);

		Font::DrawTextToScreen( m_title, 
			m_style->subTitleTextFont,
			m_style->subTitleColor,
			TextScreen,
			m_style->subTitleSize * zoomScale,
			ali);

		//due canvas to screen conversions
		Vector2f LinkBoxMinScreen, LinkBoxMaxScreen, LinkBoxMin( getAbsoluteCollisionBox().m_vMin ), LinkBoxMax(getAbsoluteCollisionBox().m_vMax);

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

		bool deleteDrag = true;
		if(!currentMouse->hoveringWidget.expired())
		{
			std::shared_ptr<UINodeLink> partner = std::dynamic_pointer_cast<UINodeLink>(currentMouse->hoveringWidget.lock());

			if(partner != nullptr)
			{
				if(m_isOutLink)
				{
					if(!partner->m_isOutLink)
					{
						pair(std::dynamic_pointer_cast<UINodeLink>(m_selfPtr.lock()), partner);
					}
				}
				else
				{
					if(partner->m_isOutLink)
					{
						pair(partner, std::dynamic_pointer_cast<UINodeLink>(m_selfPtr.lock()));
					}
				}
			}
		}

		if(deleteDrag)
		{
			HashedString ID(m_draggingStrip.lock()->getUniqueID());
			getParentController()->UnRegisterUIWidget(ID);
		}
		else
		{
			m_draggingStrip = std::weak_ptr<UILinkStrip>();
		}


		UIWidget::MouseRelease(currentMouse);
	}

	void UINodeLink::MouseRClick( std::shared_ptr<UIMouse> currentMouse)
	{
		unPair(std::dynamic_pointer_cast<UINodeLink>(m_selfPtr.lock()));
		UIWidget::MouseRClick(currentMouse);
	}

	void UINodeLink::MouseClick( std::shared_ptr<UIMouse> currentMouse)
	{
		UIWidget::MouseClick(currentMouse);
	}

	void UINodeLink::MouseDrag( std::shared_ptr<UIMouse> currentMouse)
	{
		if( m_draggingStrip.expired())
		{
			m_draggingStrip = UILinkStrip::CreateUILinkStrip(getAbsoluteCollisionBox().calcCenter(), 
				getAbsoluteCollisionBox().calcCenter(),
				getParentController(),
				getParentCanvas(),
				std::weak_ptr<UIWidget>(),
				HashedString( "LINK_" + getUniqueID().m_string + "_DRAG"),
				10,
				Vector2f(0.0f, 0.0f),
				Vector2f(0.0f, 0.0f) ).lock();

			getParentCanvas().lock()->RegisterUIWidget(m_draggingStrip.lock());
		}

		if(m_isOutLink)
			m_draggingStrip.lock()->updateEndPoint(m_parentCanvas.lock()->getCurrentMouseCanvasPosition());
		else
			m_draggingStrip.lock()->updateStartPoint(m_parentCanvas.lock()->getCurrentMouseCanvasPosition());

		UIWidget::MouseDrag(currentMouse);
	}

	void UINodeLink::move(const Vector2f& amountToMove)
	{
		UIWidget::move(amountToMove);
	}


	void UINodeLink::update(std::shared_ptr<UIMouse> currentMouse)
	{
		for(auto it = m_outLinkStrips.begin(); it != m_outLinkStrips.end(); ++it)
		{
			(*it).lock()->updateStartPoint( getAbsoluteCollisionBox().calcCenter());
		}

		if(!m_inLinkStrip.expired())
		{
			m_inLinkStrip.lock()->updateEndPoint(getAbsoluteCollisionBox().calcCenter());
		}

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

			unPair(inLink);
			if(strip == nullptr)			//strip is nullptr so we have to make one
			{
				strip = UILinkStrip::CreateUILinkStrip(outLink->getAbsoluteCollisionBox().calcCenter(), 
					inLink->getAbsoluteCollisionBox().calcCenter(),
					outLink->getParentController(),
					outLink->getParentCanvas(),
					std::weak_ptr<UIWidget>(),
					HashedString( "LINK_" + outLink->getUniqueID().m_string + inLink->getUniqueID().m_string),
					10,
					Vector2f(0.0f, 0.0f),
					Vector2f(0.0f, 0.0f) ).lock();

				outLink->getParentCanvas().lock()->RegisterUIWidget(strip);
			}
			strip->updateEndTarget(inLink);
			strip->updateEndTarget(outLink);

			strip->calcControlPoints();
			
			//pair outLink
			outLink->m_outLinkStrips.push_back(strip);

			//pair inLink
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

		int outSize = outLink->m_parentDataType->currentSize;
		if( !(outSize >= inLink->m_parentDataType->minSize && outSize <= inLink->m_parentDataType->maxSize) )
		{
			return false;
		}

		return true;
	}

	void UINodeLink::unPair( std::shared_ptr<UINodeLink> Link )
	{
		if(Link->m_isOutLink)
		{
			while( !Link->m_outLinkStrips.empty() )
			{
				auto it = Link->m_outLinkStrips.begin();
				(*it).lock()->getParentController()->UnRegisterUIWidget( ( (*it).lock() )->getUniqueID() );
			}
		}
		else
		{
			if(!Link->m_inLinkStrip.expired())
			{

				Link->getParentController()->UnRegisterUIWidget( Link->m_inLinkStrip.lock()->getUniqueID() );
			}
		}
	}

	void UINodeLink::removeLinkStrip( std::shared_ptr<UILinkStrip> strip )
	{
		if( m_isOutLink)
		{
			for(auto it = m_outLinkStrips.begin(); it != m_outLinkStrips.end(); ++it )
			{
				if( strip == (*it).lock() )
				{
					m_outLinkStrips.erase(it);
					break;
				}
			}
		}
		else
		{
			if(m_inLinkStrip.lock() == strip)
			{
				m_inLinkStrip = std::weak_ptr<UILinkStrip>();
			}
		}
	}

	std::string UINodeLink::getTitle()
	{
		return m_title;
	}



#pragma endregion

}