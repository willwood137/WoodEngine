#pragma once

#include "UIWidget.hpp"
#include "UIDataStrip.hpp"

#include "..\Engine\Shader.hpp"

#include "..\Utility\Utility.hpp"

namespace woodman
{

	struct NodeSlotCallBackInfoBase
	{
		virtual void CallBackLinkToNodeSlot(std::shared_ptr<NodeSlotCallBackInfoBase> partner) = 0;
	};

	
	class UINodeSlot : public UIWidget
	{
	public:
		UINodeSlot(std::shared_ptr<UICanvas> ParentCanvas,
			std::shared_ptr<UIWidget> parentWidget,
			const std::string& name,
			HashedString uniqueID, 
			const Vector2f& canvasCoordinates,
			bool exitSlot,
			std::shared_ptr<DataType> dType,
			std::shared_ptr<NodeSlotCallBackInfoBase> callbackRecipient);

		bool getExitSlot() const;

		void setDataStrip(std::shared_ptr<UIDataStrip> strip, std::shared_ptr<UINodeSlot> partner)
		{
			m_dataStrip = strip;
		}

		virtual void Initialize();
		virtual void render(std::shared_ptr<UIMouse> currentMouse);
		virtual void move(const Vector2f& amountToMove);
		virtual void update(std::shared_ptr<UIMouse> currentMouse);


		virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseRClick( std::shared_ptr<UIMouse> currentMouse );
 		virtual void MouseRelease( std::shared_ptr<UIMouse> currentMouse);
// 		virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse);

		void unPair();
		
		
		std::shared_ptr<UINodeSlot> getPartnerSlot()
		{
			return m_partnerSlot;
		}

		void setDataType(std::shared_ptr<DataType> dType)
		{
			m_parentDataType = dType;
			m_dataTypeSize = dType->maxSize;
		}

		void setDataSize(unsigned int size)
		{
			m_dataTypeSize = size;
		}

		std::shared_ptr<DataType> getDataType()
		{
			return m_parentDataType;
		}
		
		unsigned int getTypeSize()
		{
			return m_dataTypeSize;
		}

		static void PairNodeSlots(std::shared_ptr<UINodeSlot> outSlot, std::shared_ptr<UINodeSlot> inSlot);


	private:
		//null if unpaired
		std::shared_ptr<UINodeSlot>	m_partnerSlot;
		std::shared_ptr<UIDataStrip>  m_dataStrip;
		bool		m_exitSlot;

		std::shared_ptr<DataType> m_parentDataType;
		unsigned int m_dataTypeSize;

		std::shared_ptr<Shader> m_slotShader;

		std::shared_ptr<NodeSlotCallBackInfoBase> m_callBackRecipient;

		

		
	};
}