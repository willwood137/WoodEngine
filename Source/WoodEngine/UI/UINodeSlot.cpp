#include "../stdafx.h"

#include "UINodeSlot.hpp"
#include "UICanvas.hpp"

namespace woodman
{


	UINodeSlot::UINodeSlot(std::shared_ptr<UICanvas> ParentCanvas,
		std::shared_ptr<UIWidget> parentWidget,
		const std::string& name,
		HashedString uniqueID, 
		const Vector2f& canvasCoordinates,
		bool exitSlot)
		: UIWidget(ParentCanvas, parentWidget, name, uniqueID, canvasCoordinates),
		m_exitSlot(exitSlot)
	{

	}

	void UINodeSlot::Initialize()
	{
		m_slotShader = Shader::CreateOrGetShader("Shaders\\UI\\LinkNode");
	}

	void UINodeSlot::render(std::shared_ptr<UIMouse> currentMouse)
	{
		float zoomScale = m_parentCanvas->getZoomScale();

		ALIGNMENT ali;
		float boxOffset;
		float textOffset;

		if(m_exitSlot)
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

		Vector2f TextCanvas( m_coordinates + Vector2f(textOffset, 0.0f)), TextScreen;
		m_parentCanvas->mapPointToScreenSpace(TextCanvas, TextScreen);

		Font::DrawTextToScreen( m_name, 
			m_style->subTitleTextFont,
			m_style->subTitleColor,
			TextScreen,
			m_style->subTitleSize * zoomScale,
			ali);

		//due canvas to screen conversions
		Vector2f LinkBoxMinScreen, LinkBoxMaxScreen, LinkBoxMin(m_coordinates.x + boxOffset, m_coordinates.y - 5), LinkBoxMax(LinkBoxMin + m_canvasCollisionBoxSize );

		m_parentCanvas->mapPointToScreenSpace(LinkBoxMin, LinkBoxMinScreen);
		m_parentCanvas->mapPointToScreenSpace(LinkBoxMax, LinkBoxMaxScreen);

		Vector2f LinkBoxSizeScreen = LinkBoxMaxScreen - LinkBoxMinScreen;

		AABB2D screenSpaceBounds = m_parentCanvas->getScreenSpace();

		m_slotShader->load();
		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_position), LinkBoxMinScreen.x, LinkBoxMinScreen.y );
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_size), LinkBoxSizeScreen.x, LinkBoxSizeScreen.y);
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_screenMin), screenSpaceBounds.m_vMin.x, screenSpaceBounds.m_vMin.y  );
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_screenMax), screenSpaceBounds.m_vMax.x, screenSpaceBounds.m_vMax.y);
		glUniform2f(m_slotShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

		if(currentMouse->selectedWidget.get() == this)
		{
			if(currentMouse->isPressed)
				glUniform4f(m_slotShader->getUniformID(HASHED_STRING_u_color), 0.0f, 1.0f, .5f, 1.0f);
			else
				glUniform4f(m_slotShader->getUniformID(HASHED_STRING_u_color), 1.0f, .0f, 1.0f, 1.0f);
		}
		else if( currentMouse->hoveringWidget.get() == this )
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

	bool UINodeSlot::getExitSlot() const
	{
		return m_exitSlot;
	}

	

	void UINodeSlot::MouseRelease( std::shared_ptr<UIMouse> currentMouse)
	{

		UIWidget::MouseRelease(currentMouse);
	}

	void UINodeSlot::MouseRClick( std::shared_ptr<UIMouse> currentMouse)
	{
		UIWidget::MouseRClick(currentMouse);

		if(m_dataStrip != nullptr)
		{
			if(m_partnerSlot != nullptr)
				m_partnerSlot->m_dataStrip = nullptr;
	
			m_parentCanvas->UnRegisterUIWidget(m_dataStrip);
			m_dataStrip = nullptr;
		}
	}

	void UINodeSlot::MouseClick( std::shared_ptr<UIMouse> currentMouse)
	{

		UIWidget::MouseClick(currentMouse);
 

		if(m_dataStrip == nullptr)
		{


			//create a new one
			m_dataStrip = std::shared_ptr<UIDataStrip>(new UIDataStrip(
				m_parentCanvas, 
				nullptr, 
				"lineSTRIP",
				HashedString("DataStrip") ) );

			if(m_exitSlot)
			{
				m_dataStrip->updateStartPoint(m_coordinates);
				m_dataStrip->updateEndPoint(m_parentCanvas->getCurrentMouseCanvasPosition());
				m_dataStrip->updateStartTarget( WidgetDatabase[m_id]);
				m_dataStrip->setDragEndPoint(true);
			}
			else
			{

				m_dataStrip->updateEndPoint(m_coordinates);
				m_dataStrip->updateStartPoint(m_parentCanvas->getCurrentMouseCanvasPosition());
				m_dataStrip->updateEndTarget(WidgetDatabase[m_id]);
				m_dataStrip->setDragEndPoint(false);
			}

			m_dataStrip->Initialize();
			UIWidget::RegisterUIWidget(m_dataStrip);
			m_parentCanvas->RegisterUIWidget(m_dataStrip);

			currentMouse->selectedWidget = m_dataStrip;

		}
 		

	}

	void UINodeSlot::move(const Vector2f& amountToMove)
	{
		UIWidget::move(amountToMove);

		if(m_dataStrip != nullptr)
		{
			if(m_exitSlot)
				m_dataStrip->updateStartPoint(m_coordinates);
			else
				m_dataStrip->updateEndPoint(m_coordinates);
		}
	}


	void UINodeSlot::update(std::shared_ptr<UIMouse> currentMouse)
	{
		if(!currentMouse->isPressed)
		{


			if(m_dataStrip != nullptr)
			{
				if( !m_dataStrip->isGoodStrip())
				{
					m_parentCanvas->UnRegisterUIWidget(m_dataStrip);
					m_dataStrip = nullptr;
				}
			}
		}
	}
}