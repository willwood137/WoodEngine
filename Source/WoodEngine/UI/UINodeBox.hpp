#ifndef UINODEBOX_HPP
#define UINODEBOX_HPP

#include "UIWidget.hpp"
#include "UINodeSlot.hpp"

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

	

	class UINodeBox : public UIWidget
	{
	public:
		UINodeBox(std::shared_ptr<UICanvas> ParentCanvas,
			std::shared_ptr<UIWidget> parentWidget,
			const std::string& name,
			HashedString uniqueID, 
			const Vector2f& canvasCoordinates);

		virtual void Initialize();
		virtual void render(std::shared_ptr<UIMouse> currentMouse);

// 		virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );
// 		virtual void MouseRelease( std::shared_ptr<UIMouse> currentMouse);
 		virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse);

	private:
		//TL Corner
		Vector2f								m_titleOffset;
		std::string								m_title;
		std::list<std::string>					n_data;
		std::set< std::shared_ptr<UINodeSlot> >	m_linkSlots;
		unsigned int							m_numInSlots;
		unsigned int							m_numOutSlots;

		std::shared_ptr<Shader> m_nodeBoxShader;
		std::shared_ptr<Texture> m_borderFilterIdle;
		std::shared_ptr<Texture> m_borderFilterHover;
		std::shared_ptr<Texture> m_borderFilterDrag;
		std::shared_ptr<Texture> m_borderFilterSelected;
	};
}

#endif
