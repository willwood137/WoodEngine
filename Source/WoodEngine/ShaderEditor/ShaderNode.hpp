#pragma once

#include <vector>
#include <list>
#include <memory>
#include <string>
#include <array>

#include "../Utility/Utility.hpp"

namespace woodman
{



	struct NodeLink
	{
		std::string name;
		std::shared_ptr<DataType> typeData;
		std::string ShaderCode;
		std::string attributeName;
		std::string uniformName;
		std::string varyingName;
		std::string OpenGLName;
		std::string outName;
	};

	 struct ShaderNode
	 {
		 std::shared_ptr<NodeLink> getLinkByName(const std::string& name)
		 {
			 for(auto it = inLinks.begin(); it != inLinks.end(); ++it)
			 {
				 if( (*it)->name.compare(name) == 0)
				 {
					 return (*it);
				 }
			 }
			 return nullptr;
		 }

		std::string name;
		std::string category;
		std::list< std::shared_ptr<PropertyList> > propertyLists;
		std::vector<std::shared_ptr<NodeLink> > inLinks;
		std::vector<std::shared_ptr<NodeLink> > outLinks;
		bool canBeVert;
		bool canBeFrag;
	 };
}