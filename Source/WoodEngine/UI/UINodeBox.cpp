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
		float RelativeLayer,
		const Vector2f& canvasCoordinates)
		: UIWidget(ParentCanvas, parentWidget, name, RelativeLayer, canvasCoordinates),
		m_numInSlots(0),
		m_numOutSlots(0)
	{
		Vector2f Offset(0.0f, 0.0f);
		Offset.y += getSize().y;
		Offset.y -= (getStyle()->NodeBoxBorderLength + getStyle()->TitleBuffer + getStyle()->TitleSize);
		m_inGrid = new UIGrid(getParentCanvas(), this, m_title + "_in_grid", 5, Offset );
		m_outGrid = new UIGrid(getParentCanvas(), this, m_title + "_in_grid", 5, Offset );
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

		

		float maxInLength = 0, maxOutLength = 0;
		std::shared_ptr<UIStyle> style = getStyle();
		Vector2f size(0.0f, 0.0f);

		for(auto it = m_linkSlots.begin(); it != m_linkSlots.end(); ++it)
		{
			if( (*it)->IsOutLink())
			{
				m_numOutSlots++;
				maxOutLength = max(maxOutLength, Font::getLengthOfString( (*it)->getName(), style->subTitleTextFont, style->subTitleSize) );
			}
			else
			{
				m_numInSlots++;
				maxInLength = max(maxInLength, Font::getLengthOfString( (*it)->getName(), style->subTitleTextFont, style->subTitleSize) );
			}
		}

		//now we calculate size of box
		unsigned int maxSlots = max(m_numOutSlots, m_numInSlots);
		
		
		size.y = maxSlots * style->subTitleSize + style->TitleSize + style->TitleBuffer * 2 + style->subTitleBuffer * (maxSlots + 1) + style->NodeBoxCornerSize + style->NodeBoxBorderLength * 2;
		size.x = max(maxOutLength + maxInLength + style->subTitleBuffer, Font::getLengthOfString( m_title, style->TitleTextFont, style->TitleSize) + style->NodeBoxCornerSize + style->TitleBuffer  ) + style->NodeBoxBorderLength * 2;
		setSize(size);

		Vector2f coords = getRelativeCoordinates();

		coords.y -= size.y;

		setRelativeCoordinates(coords);

		m_titleOffset = Vector2f( (getSize().x - getStyle()->NodeBoxCornerSize) * .5f + getStyle()->NodeBoxCornerSize,
			getSize().y - getStyle()->NodeBoxBorderLength - getStyle()->TitleSize);

		m_inGrid->setRelativeCoordinates( Vector2f( style->subTitleBuffer + style->NodeBoxBorderLength, size.y - (style->NodeBoxCornerSize + style->subTitleBuffer + style->NodeBoxBorderLength) ) );
		m_outGrid->setRelativeCoordinates( Vector2f( size.x - (style->subTitleBuffer + style->NodeBoxBorderLength + maxOutLength), size.y - (style->subTitleBuffer + style->NodeBoxBorderLength) ) );



		m_inGrid->Initialize();
		m_outGrid->Initialize();

		UIWidget::Initialize();
	}

	void UINodeBox::updateData( UIMouse* currentMouse, const ParentInfo& parent_info )
	{
		UIWidget::updateData(currentMouse, parent_info);	
	}

	void UINodeBox::render(  UIMouse* currentMouse, const AABB2D& CanvasBounds ) const
	{
		float zoomScale = getParentCanvas()->getZoomScale();

		//--------------------------
		// Render the box
		//--------------------------
		m_nodeBoxShader->load();

		Vector2f NodeBoxMinScreen, NodeBoxMaxScreen, NodeBoxMin(getAbsoluteCoordinates()), NodeBoxMax(NodeBoxMin + getSize() );

		getParentCanvas()->mapPointToScreenSpace(NodeBoxMin, NodeBoxMinScreen);
		getParentCanvas()->mapPointToScreenSpace(NodeBoxMax, NodeBoxMaxScreen);

		Vector2f NodeBoxSizeScreen = NodeBoxMaxScreen - NodeBoxMinScreen;
		AABB2D CanvasScreenSpace = getParentCanvas()->getScreenSpace();

		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_position),NodeBoxMinScreen.x, NodeBoxMinScreen.y );
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_size), NodeBoxSizeScreen.x, NodeBoxSizeScreen.y);
		glUniform1f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_cornerSize), getStyle()->NodeBoxCornerSize * zoomScale );
		glUniform1f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_borderSize), getStyle()->NodeBoxBorderLength * zoomScale);
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_screenMin), CanvasScreenSpace.m_vMin.x, CanvasScreenSpace.m_vMin.y  );
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_screenMax), CanvasScreenSpace.m_vMax.x, CanvasScreenSpace.m_vMax.y);
		glUniform1i(m_nodeBoxShader->getUniformID(HASHED_STRING_u_borderFilter), 0);
		glUniform2f(m_nodeBoxShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );
		float mapLayer = clamp( getAbsoluteLayer() / g_MaxLayer, 0.0f, 1.0f);
		m_nodeBoxShader->SetUniformFloat(HASHED_STRING_u_layer, mapLayer, 1);


		std::shared_ptr<Texture> tex;

		if( getParentCanvas()->getSelectedWidget() == this)
		{
			if(currentMouse->isPressed)
				tex = m_borderFilterDrag;
			else
				tex = m_borderFilterSelected;
		}
		else if( currentMouse->hoveringWidget == this )
		{
			tex = m_borderFilterHover;
		}
		else
		{
			tex = m_borderFilterIdle;
		}
			Texture::ApplyTexture(tex);

		m_nodeBoxShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

		glDrawArrays( GL_QUADS, 0, 4);

		m_nodeBoxShader->disableAttribute(HASHED_STRING_in_position);

		//--------------------------
		//render the Title
		//--------------------------
		Vector2f TextCanvas(getAbsoluteCoordinates() + m_titleOffset), TextScreen;
		getParentCanvas()->mapPointToScreenSpace(TextCanvas, TextScreen);

		// 		Vector2f( (nodeBoxToRender->size.x + style->NodeBoxCornerSize) * .5f, 
		// 			nodeBoxToRender->size.y - style->NodeBoxBorderLength - style->TitleSize) ) * zoomScale,

		Font::DrawTextToScreen(getName(), 
			getStyle()->TitleTextFont,
			getStyle()->TitleColor,
			TextScreen,
			getStyle()->TitleSize * zoomScale,
			ALIGNMENT_CENTER);


		//BASE RENDER CALL
		UIWidget::render(currentMouse, CanvasBounds);
	}


	void UINodeBox::move(const Vector2f& amountToMove)
	{
		UIWidget::move(amountToMove);

		if(m_callBackRecipient != nullptr)
		{
			m_callBackRecipient->setPosition(getAbsoluteCoordinates());
		}
	}

	void UINodeBox::setCallBackRecipient( UINodeBoxCallBackRecipient* recipient )
	{
		m_callBackRecipient = recipient;

		if(m_callBackRecipient != nullptr)
		{
			m_callBackRecipient->setPosition(getAbsoluteCoordinates());
		}

	}

	void UINodeBox::AddLinkSlotData(const LinkSlotData& data)
	{
		std::shared_ptr<UILinkSlotData> UIdata(new UILinkSlotData());

		UIdata->Type = data.Type;
		for( auto linkIt = m_linkSlots.begin(); linkIt != m_linkSlots.end(); ++linkIt)
		{
			HashedString name((*linkIt)->getName());

			if(data.Entries.find(name) != data.Entries.end() )
			{
				UIdata->Entries.insert( (*linkIt) );
			}

			if(data.Users.find(name) != data.Users.end() )
			{
				UIdata->Users.insert( (*linkIt) );
			}
		}

		m_linkSlotDatas.push_back(UIdata);
	}

	void UINodeBox::addLink(UINodeLink* newLink)
	{
		newLink->Initialize();
		m_linkSlots.insert(newLink);

		if(newLink->IsOutLink() )
			m_outGrid->addChild(newLink);
		else
			m_inGrid->addChild(newLink);
	}

	void UINodeBox::pairLinks( UINodeLink* MyLink, UINodeLink* OtherLink )
	{
		//Check If I own MyLink
		if( m_linkSlots.find(MyLink) == m_linkSlots.end() )
			return;

		//check if the types are the same
		if( MyLink->getDataType()->type != OtherLink->getDataType()->type )
			return;

		//determine which link is the out and which is the in
		UIOutLink* outLink;
		UIInLink* inLink;
		if(MyLink->IsOutLink())
		{
			outLink = dynamic_cast<UIOutLink*>(MyLink);
			inLink = dynamic_cast<UIInLink*>(OtherLink);
		}
		else
		{
			outLink = dynamic_cast<UIOutLink*>(OtherLink);
			inLink = dynamic_cast<UIInLink*>(MyLink);
		}

		//Check For Recursion
		// 
		//TODO

		//Create the linkStrip
		//

		std::string stripName = "DataStrip_" + outLink->getName() + "_" + inLink->getName();
		UILinkStrip* linkStrip = new UILinkStrip( getParentCanvas(), outLink, stripName, -100.0f, outLink, inLink);
		linkStrip->Initialize();

		//Pair the Links
		//
		outLink->AddLinkStrip(linkStrip);

		inLink->unPair();
		inLink->SetLinkStrip(linkStrip);

		linkStrip->calcControlPoints();

		CheckLinkSlotData();

	}

	void UINodeBox::CheckLinkSlotData()
	{
		for(auto dataIt = m_linkSlotDatas.begin(); dataIt != m_linkSlotDatas.end(); ++dataIt)
		{
			unsigned int largestSize = 0;

			for( auto entryIt = (*dataIt)->Entries.begin(); entryIt != (*dataIt)->Entries.end(); ++entryIt)
			{
				largestSize = max((*entryIt)->getTypeSize(), largestSize);
			}

			//now that we have the largest sizes of the entries, set the sizes;

			for( auto entryIt = (*dataIt)->Entries.begin(); entryIt != (*dataIt)->Entries.end(); ++entryIt)
			{
				if( (*entryIt)->getTypeSize() < largestSize)
				{
					(*entryIt)->setDataSize(largestSize);
					(*entryIt)->UpdateVectorMap();
				}
			}

			for( auto userIt = (*dataIt)->Entries.begin(); userIt != (*dataIt)->Entries.end(); ++userIt)
			{
				(*userIt)->setDataSize(largestSize);
				(*userIt)->UpdateVectorMap();
			}

		}
	}

	void UINodeBox::MouseEvent( UIMouse* currentMouse, MOUSE_EVENT_TYPE mouse_event, const ParentInfo& parent_info, std::vector<int>& mouse_impulse )
	{
		MouseEventData* data = &(currentMouse->EventData[mouse_event]);



		if( mouse_event == MOUSE_EVENT_LEFT_DRAG)
		{
			Vector2f position, oldPosition;


			getParentCanvas()->mapPointToCanvasSpace( currentMouse->prevScreenPosition, oldPosition);
			getParentCanvas()->mapPointToCanvasSpace( currentMouse->screenPosition, position);

			move(position - oldPosition);
		}
	}

	void UINodeBox::setTitle( const std::string& title )
	{
		m_title = title;
	}

	std::string UINodeBox::getTitle()
	{
		return m_title;
	}



}