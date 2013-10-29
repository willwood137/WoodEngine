#ifndef UINODEBOX_HPP
#define UINODEBOX_HPP

#include "UIWidget.hpp"
#include "UINodeLink.hpp"

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
			HashedString uniqueID, 
			float RelativeLayer,
			const Vector2f& canvasCoordinates);

		virtual void Initialize();
		virtual void move(	const Vector2f& amountToMove );
		virtual void render(UIMouse* currentMouse, float ParentLayer);

// 		virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );
// 		virtual void MouseRelease( std::shared_ptr<UIMouse> currentMouse);
 		virtual void MouseDrag( UIMouse* currentMouse);

		void setCallBackRecipient( UINodeBoxCallBackRecipient* recipient );
		void AddLinkSlotData(const LinkSlotData& data);
		void addLink(UINodeLink* newLink);

	private:
		//TL Corner
		Vector2f										m_titleOffset;
		std::string										m_title;
		std::list<std::string>							n_data;
		std::set< UINodeLink* >							m_linkSlots;
		std::vector<std::shared_ptr<UILinkSlotData> >	m_linkSlotDatas;
		unsigned int									m_numInSlots;
		unsigned int									m_numOutSlots;
		
		UINodeBoxCallBackRecipient* m_callBackRecipient;

		std::shared_ptr<Shader> m_nodeBoxShader;
		std::shared_ptr<Texture> m_borderFilterIdle;
		std::shared_ptr<Texture> m_borderFilterHover;
		std::shared_ptr<Texture> m_borderFilterDrag;
		std::shared_ptr<Texture> m_borderFilterSelected;
	};
}

#endif
