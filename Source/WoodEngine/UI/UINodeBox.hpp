#ifndef UINODEBOX_HPP
#define UINODEBOX_HPP

#include "UIWidget.hpp"
#include "UINodeLink.hpp"
#include "UIGrid.hpp"

#include "../Math/Vector2.hpp"

#include <string>
#include <list>
#include <vector>
#include <memory>

namespace woodman
{	
	class UICanvas;

	enum MOUSE_STATUS
	{
		MOUSE_STATUS_IDLE,
		MOUSE_STATUS_HOVER,
		MOUSE_STATUS_PRESSED,
		MOUSE_STATUS_SELECTED,
		MOUSE_STATUS_SIZE
	};

	struct UINodeBoxCallBackRecipient
	{
		virtual void setPosition(const Vector2f& position) = 0;
	};
	
	struct UILinkSlotData
	{
		LinkSlotDataType Type;
		std::set<UINodeLink*> Entries;
		std::set<UINodeLink*> Users;
	};

	class UINodeBox : public UIWidget
	{
	public:
		UINodeBox(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			float RelativeLayer,
			const Vector2f& canvasCoordinates);

		virtual void Initialize();

		virtual void render(  UIMouse* currentMouse, const AABB2D& CanvasBounds ) const;
		virtual void updateData(UIMouse* currentMouse, const ParentInfo& parent_info );
		virtual void move(const Vector2f& amountToMove);

		virtual void MouseEvent( UIMouse* currentMouse, MOUSE_EVENT_TYPE mouse_event, const ParentInfo& parent_info, std::vector<int>& mouse_impulse);
		
		void setCallBackRecipient( UINodeBoxCallBackRecipient* recipient );
		void AddLinkSlotData(const LinkSlotData& data);
		void addLink(UINodeLink* newLink);
		void pairLinks( UINodeLink* MyLink, UINodeLink* OtherLink );
		void CheckLinkSlotData();
		void setTitle(const std::string& title);
		std::string getTitle();

	private:
		//TL Corner
		Vector2f										m_titleOffset;
		std::string										m_title;
		std::list<std::string>							n_data;
		std::set< UINodeLink* >							m_linkSlots;
		std::vector<std::shared_ptr<UILinkSlotData> >	m_linkSlotDatas;
		unsigned int									m_numInSlots;
		unsigned int									m_numOutSlots;

		UIGrid*											m_inGrid;
		UIGrid*											m_outGrid;

		
		UINodeBoxCallBackRecipient* m_callBackRecipient;

		std::shared_ptr<Shader> m_nodeBoxShader;
		std::shared_ptr<Texture> m_borderFilterIdle;
		std::shared_ptr<Texture> m_borderFilterHover;
		std::shared_ptr<Texture> m_borderFilterDrag;
		std::shared_ptr<Texture> m_borderFilterSelected;
	};
}

#endif
