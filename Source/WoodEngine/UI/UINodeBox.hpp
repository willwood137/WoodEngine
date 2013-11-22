#ifndef UINODEBOX_HPP
#define UINODEBOX_HPP


#include "UIWidget.hpp"

#include "../Math/Vector2.hpp"

#include <string>
#include <list>
#include <vector>
#include <memory>

namespace woodman
{	
	class UINodeLink;
	class UITextEntry;

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
		virtual void setDataField(HashedString fieldName, float value) = 0;
	};

	struct LinkDefinition
	{
		std::string name;
		DataType typeInfo;
	};

	struct UINodeBoxDefinition
	{
		std::vector< LinkDefinition > inLinks;
		std::vector< LinkDefinition > outLinks;

	};

	class UINodeBox : public UIWidget
	{
	public:
		
		static std::weak_ptr<UINodeBox> CreateUINodeBox(const std::string& _Title, 
			UIController* _ParentController,
			std::weak_ptr<UICanvas> _ParentCanvas,
			std::weak_ptr<UIWidget> _ParentWidget,
			const HashedString& _ID,
			float _RelativeLayer,
			const Vector2f& _RelativeCoordinates,
			const Vector2f& _CollisionSize );

		virtual void Initialize();
		virtual void move(	const Vector2f& amountToMove );
		virtual void render(std::shared_ptr<UIMouse> currentMouse);
		virtual void update(std::shared_ptr<UIMouse> currentMouse);

// 		virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );
// 		virtual void MouseRelease( std::shared_ptr<UIMouse> currentMouse);
		virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse);
		virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );

		void setCallBackRecipient( UINodeBoxCallBackRecipient* recipient );
		void addDataField( std::weak_ptr<UITextEntry> field);
		void addLink( std::shared_ptr<UINodeLink> link);

	private:

		UINodeBox( const std::string& Title,
			UIController* parentController,
			std::weak_ptr<UICanvas> ParentCanvas,
			std::weak_ptr<UIWidget> parentWidget,
			HashedString uniqueID,
			float RelativeLayer,
			const Vector2f& relativeCoordinates,
			const Vector2f& collisionSize );


		void calcNodeBoxSize();

		//TL Corner
		Vector2f									m_titleOffset;
		Vector2f									m_titleSize;
		std::string									m_title;
		std::list<std::string>						n_data;
		std::vector< std::weak_ptr<UINodeLink> >	m_inSlots;
		std::vector< std::weak_ptr<UINodeLink> >	m_outSlots;
		std::vector< std::weak_ptr<UITextEntry> >	m_dataFields;
		unsigned int								m_numInSlots;
		unsigned int								m_numOutSlots;
		float										m_outLongest;
		float										m_inLongest;

		UINodeBoxCallBackRecipient* m_callBackRecipient;

		std::shared_ptr<Shader> m_nodeBoxShader;
		std::shared_ptr<Texture> m_borderFilterIdle;
		std::shared_ptr<Texture> m_borderFilterHover;
		std::shared_ptr<Texture> m_borderFilterDrag;
		std::shared_ptr<Texture> m_borderFilterSelected;
	};
}

#endif
