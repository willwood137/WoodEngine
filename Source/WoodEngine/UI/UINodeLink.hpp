#pragma once

#include "UIWidget.hpp"
#include "UILinkStrip.hpp"

#include "..\Engine\Shader.hpp"

#include "..\Utility\Utility.hpp"

namespace woodman
{

	struct UINodeLinkCallBackBase
	{
		virtual void CallBackLinkToNodeSlot(UINodeLinkCallBackBase* partner) = 0;
	};


	class UINodeLink : public UIWidget
	{
	public:

		virtual void Initialize();
		virtual void render(UIMouse* currentMouse);
		virtual void move(const Vector2f& amountToMove);
		virtual void update(UIMouse* currentMouse);


		virtual void MouseClick(   UIMouse* currentMouse );
		virtual void MouseRClick(  UIMouse* currentMouse );
		virtual void MouseRelease( UIMouse* currentMouse);
		virtual void MouseDrag( UIMouse* currentMouse);

		// pure virtual functions
		virtual void Pair(std::shared_ptr<UILinkStrip> linkStrip, UINodeLink* partnerLink) = 0;
		virtual void unPair(std::shared_ptr<UILinkStrip> linkStrip) = 0;
		virtual bool DoIOwnStrip( std::shared_ptr<UILinkStrip> linkStrip) = 0;
		virtual UINodeLink* getPartnerSlot() = 0;

#pragma region UINodeLink_Getters
		bool IsOutLink() const;
		DataType* getDataType();
		unsigned int getTypeSize();
		UINodeLinkCallBackBase* getCallBackRecipient();
#pragma endregion
#pragma region UINodeLink_Setters
		void setDraggingStrip(std::shared_ptr<UILinkStrip> linkStrip);
		void setDataType(DataType* dType);
		void setDataSize(unsigned int size);
#pragma endregion

	protected:

		UINodeLink(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			HashedString uniqueID, 
			const Vector2f& canvasCoordinates,
			bool outLink,
			DataType* dType,
			UINodeLinkCallBackBase* callbackRecipient);


		UINodeLinkCallBackBase* m_callBackRecipient;

		//null if unpaired
		std::shared_ptr<UILinkStrip> m_draggingStrip;

		bool		 m_isOutLink;
		DataType*    m_parentDataType;
		unsigned int m_dataTypeSize;

		std::shared_ptr<Shader> m_slotShader;
	};

	class UIOutLink : public UINodeLink
	{
	public:
		UIOutLink(UICanvas* ParentCanvas,
			   UIWidget* parentWidget,
			   const std::string& name,
			   HashedString uniqueID, 
			   const Vector2f& canvasCoordinates,
			   DataType* dType,
			   UINodeLinkCallBackBase* callbackRecipient);

		virtual void render(UIMouse* currentMouse);
		virtual void MouseClick(   UIMouse* currentMouse );
		virtual void MouseRClick(  UIMouse* currentMouse );
		virtual void MouseRelease( UIMouse* currentMouse);
		virtual void MouseDrag( UIMouse* currentMouse);
		virtual void move(const Vector2f& amountToMove);

		virtual void Pair(std::shared_ptr<UILinkStrip> linkStrip, UINodeLink* partnerLink);
		virtual void unPair(std::shared_ptr<UILinkStrip> linkStrip);
		virtual bool DoIOwnStrip( std::shared_ptr<UILinkStrip> linkStrip);
		virtual UINodeLink* getPartnerSlot();


	private:
		//Multiple partner/datastrips
		std::set<std::shared_ptr<UILinkStrip> > m_linkStrips;
	};

	class UIInLink : public UINodeLink
	{
	public:
		UIInLink(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			HashedString uniqueID, 
			const Vector2f& canvasCoordinates,
			DataType* dType,
			UINodeLinkCallBackBase* callbackRecipient);

		virtual void render(UIMouse* currentMouse);
		virtual void MouseClick(   UIMouse* currentMouse );
		virtual void MouseRClick(  UIMouse* currentMouse );
		virtual void MouseRelease( UIMouse* currentMouse);
		virtual void MouseDrag( UIMouse* currentMouse);
		virtual void move(const Vector2f& amountToMove);

		virtual void Pair(std::shared_ptr<UILinkStrip> linkStrip, UINodeLink* partnerLink);
		virtual void Pair( UIOutLink* partnerLink);
		virtual void unPair(std::shared_ptr<UILinkStrip> linkStrip);
		virtual bool DoIOwnStrip( std::shared_ptr<UILinkStrip> linkStrip);
		virtual UINodeLink* getPartnerSlot();

	private:
		//singular partner/linkStrip
		std::shared_ptr<UILinkStrip> m_linkStrip;
	};
}