#include "../stdafx.h"

#include "UINodeLink.hpp"
#include "UICanvas.hpp"
#include "UINodeBox.hpp"

namespace woodman
{

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------NODELINk----------------NODELINk---------------NODELINk----------NODELINk------------NODELINk-----------NODELINk----------NODELINk--------NODELINk-----------NODELINk--------
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	UINodeLink::UINodeLink(UICanvas* ParentCanvas,
		UIWidget* parentWidget,
		const std::string& name,
		float RelativeLayer,
		const Vector2f& canvasCoordinates,
		bool outLink,
		DataType* dType,
		UINodeLinkCallBackBase* callbackRecipient)
		: UIWidget(ParentCanvas, parentWidget, name, RelativeLayer, canvasCoordinates),
		m_isOutLink(outLink),
		m_parentDataType(dType),
		m_callBackRecipient(callbackRecipient)
	{
		m_dataTypeSize = m_parentDataType->maxSize;
	}

	void UINodeLink::Initialize()
	{
		m_slotShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\LinkNode");
	}

	void UINodeLink::render( UIMouse* currentMouse, const AABB2D& CanvasBounds ) const
	{
		float zoomScale = getParentCanvas()->getZoomScale();

		ALIGNMENT ali;
		float boxOffset;
		float textOffset;

		if(m_isOutLink)
		{
			ali = ALIGNMENT_RIGHT;
			boxOffset = -10.0f;
			textOffset = -getStyle()->NodeBoxBorderLength -4.0f;
		}
		else
		{
			ali = ALIGNMENT_LEFT;
			boxOffset = -8.0f;
			textOffset = getStyle()->NodeBoxBorderLength + 4.0f;
		}

		Vector2f TextCanvas( getAbsoluteCoordinates() + Vector2f(textOffset, 0.0f)), TextScreen;
		getParentCanvas()->mapPointToScreenSpace(TextCanvas, TextScreen);

		Font::DrawTextToScreen( getName(), 
			getStyle()->subTitleTextFont,
			getStyle()->subTitleColor,
			TextScreen,
			getStyle()->subTitleSize * zoomScale,
			ali);

		//due canvas to screen conversions
		Vector2f LinkBoxMinScreen, LinkBoxMaxScreen, LinkBoxMin(getAbsoluteCoordinates().x + boxOffset, getAbsoluteCoordinates().y - 5), LinkBoxMax(LinkBoxMin + getSize() );

		getParentCanvas()->mapPointToScreenSpace(LinkBoxMin, LinkBoxMinScreen);
		getParentCanvas()->mapPointToScreenSpace(LinkBoxMax, LinkBoxMaxScreen);

		Vector2f LinkBoxSizeScreen = LinkBoxMaxScreen - LinkBoxMinScreen;

		AABB2D screenSpaceBounds = getParentCanvas()->getScreenSpace();

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

		if(getParentCanvas()->getSelectedWidget() == this)
		{
			if(currentMouse->isPressed)
				glUniform4f(m_slotShader->getUniformID(HASHED_STRING_u_color), 0.0f, 1.0f, .5f, 1.0f);
			else
				glUniform4f(m_slotShader->getUniformID(HASHED_STRING_u_color), 1.0f, .0f, 1.0f, 1.0f);
		}
		else if( currentMouse->hoveringWidget == this )
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



		UIWidget::render(currentMouse, CanvasBounds);
	}


	void UINodeLink::MouseRelease( UIMouse* currentMouse)
	{
		
	}

	void UINodeLink::MouseRClick( UIMouse* currentMouse)
	{
	}

	void UINodeLink::MouseClick( UIMouse* currentMouse)
	{
	}

	void UINodeLink::MouseDrag( UIMouse* currentMouse)
	{
	}

	void UINodeLink::move(const Vector2f& amountToMove)
	{
		UIWidget::move(amountToMove);
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

#pragma endregion


	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------OUTLINK----------------OUTLINK---------------OUTLINK----------OUTLINK------------OUTLINK-----------OUTLINK----------OUTLINK--------OUTLINK-----------OUTLINK--------
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region UIOutLink

	UIOutLink::UIOutLink(UICanvas* ParentCanvas,
		UIWidget* parentWidget,
		const std::string& name,
		float RelativeLayer,
		const Vector2f& canvasCoordinates,
		DataType* dType,
		UINodeLinkCallBackBase* callbackRecipient)
		:UINodeLink(ParentCanvas, parentWidget, name, RelativeLayer, canvasCoordinates, true, dType, callbackRecipient )
	{

	}

	void UIOutLink::render(UIMouse* currentMouse, const AABB2D& CanvasBounds) const
	{
		UINodeLink::render(currentMouse, CanvasBounds );

		
		if(m_draggingStrip != nullptr)
		{
			m_draggingStrip->render(currentMouse, CanvasBounds);
		}

// 		for(auto it = m_linkStrips.begin(); it != m_linkStrips.end(); ++it)
// 		{
// 			(*it)->render(currentMouse, layer);
// 		}
	}

	//-------------------------------------------------------------------------

	void UIOutLink::MouseClick(   UIMouse* currentMouse )
	{

		UINodeLink::MouseClick(currentMouse);

		//we can have multiple LinkStrips so...

		//create a new one

		CanvasCoordinates startCoords(getParentCanvas(), getAbsoluteCoordinates() + Vector2f(0.0, getSize().y * .25f));

		std::shared_ptr<UILinkStrip> newStrip( new UILinkStrip(
			getParentCanvas(), 
			this, 
			"lineSTRIP",
			-1.0f,
			m_parentDataType->type,
			m_dataTypeSize, 
			startCoords,
			startCoords) );


		newStrip->Initialize();
		newStrip->updateStartTarget( this );

		m_draggingStrip = newStrip;

	}

	//-------------------------------------------------------------------------

	void UIOutLink::MouseDrag( UIMouse* currentMouse)
	{
		UINodeLink::MouseDrag(currentMouse);

		//so are we the one who is selected?
// 		if(currentMouse->selectedWidget == this)
// 		{
// 			//make sure that were dragging something
// 			if(m_draggingStrip != nullptr)
// 			{
// 				Vector2f canvasCoords;
// 				currentMouse->hoveringCanvas->mapPointToCanvasSpace(currentMouse->screenPosition, canvasCoords);
// 				CanvasCoordinates newEndPoint(currentMouse->hoveringCanvas, canvasCoords);
// 				m_draggingStrip->updateEndPoint(newEndPoint);
// 			}
// 		}
	}

	//-------------------------------------------------------------------------

	void UIOutLink::MouseRClick(  UIMouse* currentMouse )
	{
		while( !m_linkStrips.empty() )
		{
			unPair( *(m_linkStrips.begin() ) );
		}
	}

	//-------------------------------------------------------------------------

	void UIOutLink::MouseRelease( UIMouse* currentMouse)
	{
		//so are we the one who is selected
// 		if(currentMouse->selectedWidget == this)
// 		{
// 			//just to be sure check that were dragging something
// 			if(m_draggingStrip != nullptr)
// 			{
// 				//is the hovering widget an InLink? (NodeLink isn't good enough)
// 				UINodeLink* partnerLink = dynamic_cast<UIInLink*>(currentMouse->hoveringWidget);
// 				if(partnerLink != nullptr)
// 				{
// 
// 					//why yes it is, LINK US UP, but first we have to unlink any other links
// 					UINodeBox* nodeBox = dynamic_cast<UINodeBox*>(m_parentWidget);
// 					if(nodeBox != nullptr)
// 						nodeBox->pairLinks(this, partnerLink);
// 					// 					m_draggingStrip->updateStartTarget(this);
// 					// 					m_draggingStrip->updateEndTarget(partnerLink);
// 
// 					//partnerLink->Pair(m_draggingStrip);
// 				}
// 				else
// 				{
// 					//oh no we released on not a InLink, check if it already is linked
// 					// 					UINodeLink* existingPartner = dynamic_cast<UIInLink*>(m_draggingStrip->getEndTarget() );
// 					// 					if(existingPartner != nullptr)
// 					// 					{
// 					// 						//there is! set it back to that
// 					// 						//this should be done automatically?
// 					// 						m_draggingStrip->updateEndTarget(nullptr);
// 					// 						Pair(m_draggingStrip, existingPartner);
// 					// 						existingPartner->move(Vector2f(0.0f, 0.0f));
// 					// 					}
// 
// 				}
// 
// 				//set that we are not dragging anything anymore
// 				m_draggingStrip = nullptr;
// 			}
// 		}
	}

	//-------------------------------------------------------------------------

	// 	void UIOutLink::Pair(std::shared_ptr<UILinkStrip> linkStrip, UINodeLink* partnerLink)
	// 	{
	// 		// out links can have many partners, so just add it
	// 		m_linkStrips.insert(linkStrip);
	// 		linkStrip->updateStartTarget(this);
	// 		CanvasCoordinates newCoords(m_parentCanvas, m_coordinates + Vector2f(0.0, m_canvasCollisionBoxSize.y * .25f));
	// 		linkStrip->updateStartPoint(newCoords);
	// 
	// 		UINodeLink* stripLink = dynamic_cast<UINodeLink*>(linkStrip->getEndTarget());
	// 		if(partnerLink != stripLink)
	// 		{
	// 			partnerLink->Pair(linkStrip, this);
	// 		}
	// 	}

	//-------------------------------------------------------------------------

	void UIOutLink::unPair(UILinkStrip* linkStrip)
	{
		// So we have to remove that Thing

		//remove reference to us
		linkStrip->updateStartTarget(nullptr);

		//then tell the other guy to remove his reference

		UIInLink* partnerLink = dynamic_cast<UIInLink*>(linkStrip->getEndTarget());
		if(partnerLink != nullptr)
		{
			partnerLink->unPair();
		}

		//then erase the linkStrip
		m_linkStrips.erase(linkStrip);

		removeChild(linkStrip);
	}

	//-------------------------------------------------------------------------

	void UIOutLink::move(const Vector2f& amountToMove)
	{
		UINodeLink::move(amountToMove);

		CanvasCoordinates newCoords(getParentCanvas(), getAbsoluteCoordinates() + Vector2f(0.0, getSize().y * .25f));
		for(auto it = m_linkStrips.begin(); it != m_linkStrips.end(); ++it)
		{
			(*it)->updateStartPoint(newCoords);
		}
	}

	//-------------------------------------------------------------------------

	UINodeLink* UIOutLink::getPartnerSlot()
	{
		return nullptr;
	}

	void UIOutLink::AddLinkStrip(UILinkStrip* strip)
	{
		addChild(strip);
		m_linkStrips.insert(strip);
	}

	void UIOutLink::UpdateVectorMap()
	{
		for(auto it = m_linkStrips.begin(); it != m_linkStrips.end(); ++it)
		{
			(*it)->updateVectorMap();
		}
	}

#pragma endregion

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------INLINK----------------INLINK---------------INLINK----------INLINK------------INLINK-----------INLINK----------INLINK--------INLINK-----------INLINK--------
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region UIInLink


	UIInLink::UIInLink(UICanvas* ParentCanvas,
		UIWidget* parentWidget,
		const std::string& name,
		float RelativeLayer,
		const Vector2f& canvasCoordinates,
		DataType* dType,
		UINodeLinkCallBackBase* callbackRecipient)
		:UINodeLink(ParentCanvas, parentWidget, name, RelativeLayer, canvasCoordinates, false, dType, callbackRecipient )
	{

	}

	void UIInLink::render( UIMouse* currentMouse, const AABB2D& CanvasBounds ) const
	{
		UINodeLink::render(currentMouse, CanvasBounds);

		if(m_draggingStrip != nullptr)
		{
			m_draggingStrip->render(currentMouse, CanvasBounds);
		}
	}

	//-------------------------------------------------------------------------

	void UIInLink::MouseClick(   UIMouse* currentMouse )
	{
		//if there exists a strip already, start dragging it
		if(m_linkStrip != nullptr)
		{
			//but we want to know about it too
			//m_draggingStrip = m_linkStrip;

			// 			UIOutLink* partner = dynamic_cast<UIOutLink*>(m_linkStrip->getStartTarget());
			// 			if(partner != nullptr)
			// 			{
			// 
			// 				//make the partner link start dragging the strip
			// 				partner->setDraggingStrip( m_linkStrip );
			// 				currentMouse->selectedWidget = partner;
			// 				m_linkStrip = nullptr;
			// 			}
		}
		else
		{
			//otherwise create a new one
			CanvasCoordinates startCoords(getParentCanvas(), getAbsoluteCoordinates() + Vector2f(0.0, getSize().y * .25f));

			m_draggingStrip = std::shared_ptr<UILinkStrip>( new UILinkStrip(
				getParentCanvas(), 
				this, 
				"lineSTRIP",
				-1.0f,
				m_parentDataType->type,
				m_dataTypeSize, 
				startCoords,
				startCoords) );

			m_draggingStrip->Initialize();
			m_draggingStrip->updateEndTarget(this);
		}
	}

	//-------------------------------------------------------------------------

	void UIInLink::MouseRClick(  UIMouse* currentMouse )
	{
		if(m_linkStrip != nullptr)
		{
			//unPair(m_linkStrip);
		}
	}

	//-------------------------------------------------------------------------

	void UIInLink::MouseRelease( UIMouse* currentMouse)
	{
		//so are we the one who is selected
// 		if(currentMouse->selectedWidget == this)
// 		{
// 			//just to be sure check that were dragging something
// 			if(m_draggingStrip != nullptr)
// 			{
// 				//is the hovering widget an OutLink? (NodeLink isn't good enough)
// 				UINodeLink* partnerLink = dynamic_cast<UIOutLink*>(currentMouse->hoveringWidget);
// 				if(partnerLink != nullptr)
// 				{
// 
// 					//why yes it is, LINK US UP, but first we have to unlink any other links
// 					// 						m_draggingStrip->updateEndTarget(this);
// 					// 						m_draggingStrip->updateStartTarget(partnerLink);
// 
// 					UINodeBox* nodeBox = dynamic_cast<UINodeBox*>(m_parentWidget);
// 					if(nodeBox != nullptr)
// 						nodeBox->pairLinks(this, partnerLink);
// 				}
// 				else
// 				{
// 					//oh no we released on not a InLink, check if it already is linked
// 					if(m_draggingStrip->getEndTarget() != nullptr)
// 					{
// 						//there is! set it back to that
// 						//this should be done automatically?
// 					}
// 
// 				}
// 
// 				//set that we are not dragging anything anymore
// 				m_draggingStrip = nullptr;
// 			}
// 		}
	}

	//-------------------------------------------------------------------------

	void UIInLink::MouseDrag( UIMouse* currentMouse)
	{
// 		UIWidget::MouseDrag(currentMouse);
// 
// 		//so are we the one who is selected?
// 		if(currentMouse->selectedWidget == this)
// 		{
// 			//make sure that were dragging something
// 			if(m_draggingStrip != nullptr)
// 			{
// 
// 				Vector2f canvasCoords;
// 				currentMouse->hoveringCanvas->mapPointToCanvasSpace(currentMouse->screenPosition, canvasCoords);
// 				CanvasCoordinates newStartPoint(currentMouse->hoveringCanvas, canvasCoords);
// 				m_draggingStrip->updateStartPoint(newStartPoint);
// 			}
// 
// 		}
	}

	//-------------------------------------------------------------------------

	// 	void UIInLink::Pair(std::shared_ptr<UILinkStrip> linkStrip, UINodeLink* partnerLink)
	// 	{
	// 		//override the current one
	// 		if(m_linkStrip != nullptr)
	// 			unPair(m_linkStrip);
	// 	
	// 		m_linkStrip = linkStrip;
	// 		linkStrip->updateEndTarget(this);
	// 		m_callBackRecipient->CallBackLinkToNodeSlot(partnerLink->getCallBackRecipient());
	// 
	// 		UINodeLink* stripLink = dynamic_cast<UINodeLink*>(linkStrip->getStartTarget());
	// 		if(partnerLink != stripLink)
	// 		{
	// 			partnerLink->Pair(linkStrip, this);
	// 		}
	// 	}

	//-------------------------------------------------------------------------

	// 	void UIInLink::Pair( UIOutLink* partnerLink)
	// 	{
	// 
	// 		CanvasCoordinates startCoords(partnerLink->getParentCanvas(), m_coordinates + Vector2f(0.0, m_canvasCollisionBoxSize.y * .25f));
	// 		CanvasCoordinates endCoords(m_parentCanvas, m_coordinates + Vector2f(0.0, m_canvasCollisionBoxSize.y * .25f));
	// 
	// 		m_linkStrip = std::shared_ptr<UILinkStrip>( new UILinkStrip(
	// 			m_parentCanvas, 
	// 			this, 
	// 			"lineSTRIP",
	// 			HashedString("LinkStrip"),
	// 			-1.0f, 
	// 			m_parentDataType->type,
	// 			m_dataTypeSize, 
	// 			startCoords,
	// 			endCoords) );
	// 
	// 		m_callBackRecipient->CallBackLinkToNodeSlot(partnerLink->getCallBackRecipient());
	// 
	// 		m_linkStrip->Initialize();
	// 		m_linkStrip->updateEndTarget(this);
	// 
	// 		partnerLink->Pair(m_linkStrip, this);
	// 	}

	//-------------------------------------------------------------------------

	void UIInLink::unPair()
	{
		// So we have to remove that Thing

		//remove reference to us
		if(m_linkStrip != nullptr)
		{

			m_linkStrip->updateEndTarget(nullptr);

			//then tell the other guy to remove his reference
			UIOutLink* partnerLink = dynamic_cast<UIOutLink*>(m_linkStrip->getStartTarget());
			if(partnerLink != nullptr)
			{
				partnerLink->unPair(m_linkStrip);
			}

			//then remove the refernece to the linkStrip
			m_linkStrip = nullptr;


		}
	}

	//-------------------------------------------------------------------------

	void UIInLink::move(const Vector2f& amountToMove)
	{
		UINodeLink::move(amountToMove);

		if(m_linkStrip != nullptr)
		{
			CanvasCoordinates newCoords(getParentCanvas(), getAbsoluteCoordinates() + Vector2f(0.0, getSize().y * .25f));
			m_linkStrip->updateEndPoint(newCoords);
		}
	}

	//-------------------------------------------------------------------------

	UINodeLink* UIInLink::getPartnerSlot()
	{
		if(m_linkStrip != nullptr)
		{
			return dynamic_cast<UINodeLink*>(m_linkStrip->getStartTarget());
		}

		return nullptr;
	}

	void UIInLink::SetLinkStrip(UILinkStrip* strip)
	{
		m_linkStrip = strip;
		setDataSize(m_linkStrip->getStartTarget()->getTypeSize());
	}

	void UIInLink::UpdateVectorMap()
	{
		if(m_linkStrip != nullptr)
		{
			m_linkStrip->updateVectorMap();
		}
	}

#pragma endregion

}