#pragma once


#include "../UI/UINodeBox.hpp"
#include "../UI/UINodeLink.hpp"

namespace woodman
{
	struct ModelConverterNodeLink : public UINodeLinkCallBackBase
	{
		void CallBackLinkToNodeSlot(UINodeLinkCallBackBase* partner);

		std::string name;

		ModelConverterNodeLink* partnerLink;
	};

	class ModelConverterNode : public UINodeBoxCallBackRecipient
	{

	};
}