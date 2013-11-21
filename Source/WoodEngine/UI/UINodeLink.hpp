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

		static std::weak_ptr<UINodeLink> CreateUINodeLink(const std::string& _Title,
			bool _outLink,
			UINodeLinkCallBackBase* callbackRecipient,
			UIController* _ParentController,
			std::weak_ptr<UICanvas> _ParentCanvas,
			std::weak_ptr<UIWidget> _ParentWidget,
			const HashedString& _ID,
			float _RelativeLayer,
			const Vector2f& _RelativeCoordinates,
			const Vector2f& _CollisionSize );

		virtual void Initialize();
		virtual void render(std::shared_ptr<UIMouse> currentMouse );
		virtual void move(const Vector2f& amountToMove);
		virtual void update(std::shared_ptr<UIMouse> currentMouse);


		virtual void MouseClick(   std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseRClick(  std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseRelease( std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse );

		// pure virtual functions
		
		std::shared_ptr<UINodeLink> getPartnerSlot();
		void removeLinkStrip( std::shared_ptr<UILinkStrip> strip);

		static bool pair(std::shared_ptr<UINodeLink> outLink, std::shared_ptr<UINodeLink> inLink, std::shared_ptr<UILinkStrip> strip = nullptr);
		static void unPair(std::shared_ptr<UINodeLink> Link);
		static bool isPairLegal(std::shared_ptr<UINodeLink> outLink, std::shared_ptr<UINodeLink> inLink);


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

	private:

		UINodeLink(const std::string& _Title,
			bool _outLink,
			UINodeLinkCallBackBase* callbackRecipient,
			UIController* _ParentController,
			std::weak_ptr<UICanvas> _ParentCanvas,
			std::weak_ptr<UIWidget> _ParentWidget,
			const HashedString& _ID,
			float _RelativeLayer,
			const Vector2f& _RelativeCoordinates,
			const Vector2f& _CollisionSize );


		std::string m_title;
		UINodeLinkCallBackBase* m_callBackRecipient;

		//null if unpaired
		std::shared_ptr<UILinkStrip> m_draggingStrip;
		std::set<std::shared_ptr<UILinkStrip> > m_outLinkStrips;
		std::shared_ptr<UILinkStrip> m_inLinkStrip;

		bool		 m_isOutLink;
		DataType*    m_parentDataType;
		unsigned int m_dataTypeSize;

		std::shared_ptr<Shader> m_slotShader;
	};
}