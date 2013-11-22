#include "../stdafx.h"

#include "NodeInstance.hpp"
#include "../Engine/Font.hpp"
#include "../UI/UIStyle.hpp"


namespace woodman
{
	void NodeLinkInstance::CallBackLinkToNodeSlot( UINodeLinkCallBackBase* partner)
	{
		NodeLinkInstance* instance = dynamic_cast<NodeLinkInstance*>(partner);

		if(instance != nullptr)
		{
			partnerLinkInstance = instance;
		}
		else
		{
			partnerLinkInstance = nullptr;
		}
	}

	void NodeInstance::render()
	{

	}

	NodeInstance::NodeInstance( ShaderNode* NodeReference, const Vector2f& startPosition, SHADER_TYPE shaderType, HashedString uniqueID )
		: m_referenceNode(NodeReference),
		m_position(startPosition),
		m_uniqueID(uniqueID),
		m_shaderType(shaderType)
	{

		//create in Links
		int i = 0;
		for(auto it = NodeReference->inLinks.begin(); it != NodeReference->inLinks.end(); ++it)
		{
			HashedString LinkuniqueID( uniqueID.m_string + "_" + (*it)->name );
			NodeLinkInstance* newNodeLink = new NodeLinkInstance(LinkuniqueID);
			newNodeLink->parentLink = (*it).get();
			newNodeLink->parentNodeInstance = this;
			newNodeLink->exitNode = false;
			newNodeLink->m_dType = (*it)->typeData;
			//newNodeLink-> = (*it)->typeData.maxSize;
			m_linkInstances.insert( std::make_pair(LinkuniqueID, std::unique_ptr<NodeLinkInstance>(newNodeLink) ) );
		}

		//create out links
		for( auto it = NodeReference->outLinks.begin(); it != NodeReference->outLinks.end(); ++it)
		{
			HashedString LinkuniqueID( uniqueID.m_string + "_" + (*it)->name );
			NodeLinkInstance* newNodeLink = new NodeLinkInstance(LinkuniqueID);
			newNodeLink->parentLink = (*it).get();
			newNodeLink->parentNodeInstance = this;
			newNodeLink->exitNode = true;
			newNodeLink->m_dType = (*it)->typeData;
			//newNodeLink->typeSize = (*it)->typeData.maxSize;
			m_linkInstances.insert( std::make_pair(LinkuniqueID, std::unique_ptr<NodeLinkInstance>(newNodeLink) ) );
		}

		//create datafields
		for( auto it = NodeReference->dataFields.begin(); it != NodeReference->dataFields.end(); ++it)
		{
			HashedString uniqueID(uniqueID.m_string + "_" + (*it).name );
			DataFieldInstance* field = new DataFieldInstance(uniqueID);
			field->m_name = (*it).name;
			m_dataFields.insert( std::make_pair(uniqueID, std::unique_ptr<DataFieldInstance>(field)));
		}
	}



	void NodeInstance::update()
	{
	}



	NodeLinkInstance* NodeInstance::getLinkByID(HashedString uniqueID)
	{
		auto it = m_linkInstances.find(uniqueID);

		if(it != m_linkInstances.end())
		{
			return it->second.get();
		}
		else 
			return nullptr;
	}


	void NodeInstance::CompileLink(NodeLinkInstance* link, std::string& compilation, unsigned int compileCounter)
	{

		//check if this link has already been compiled
		if(compileCounter != link->lastCompile)
		{


			link->lastCompile = compileCounter;

			if( link->exitNode )
			{
				if( !link->parentLink->ShaderCode.empty() )
				{
					//this has shader code put it in!
					std::stringstream ss, shaderCodeParse(link->parentLink->ShaderCode);

					std::string tempString, insertString;

					while(shaderCodeParse.good())
					{

						shaderCodeParse >> tempString;

						if(tempString[0] == '$')
						{
							//this is a variable, get its value
							NodeLinkInstance* tempLinkInstance = getLinkInstanceByName(tempString.substr(1));

							if(tempLinkInstance != nullptr)
							{
								if(tempLinkInstance->partnerLinkInstance != nullptr)
								{
									tempLinkInstance->partnerLinkInstance->parentNodeInstance->CompileLink(tempLinkInstance, compilation, compileCounter);
								}
								if( tempLinkInstance->m_dType.type == PROPERTYTYPE_FLOAT)
								{
									std::string variableConversion;
									ConvertToType(	tempLinkInstance->partnerLinkInstance->m_dType,	tempLinkInstance->m_dType,	tempLinkInstance->varInfo.VariableName.m_string, variableConversion );
									insertString += variableConversion; 
								}
								else
								{
									insertString += tempLinkInstance->varInfo.VariableName.m_string;
								}
							}
						}
						else if(tempString[0] == '#')
						{
							//this is a variable, get its value
							DataFieldInstance* inst = getDataFieldInstanceByName(tempString.substr(1) );

							std::stringstream ssData;
							ssData << inst->m_value;
							insertString += ssData.str();
						}
						else
						{
							insertString += tempString + " ";
						}
					}

					ss << link->varInfo.variablePrefix << " " << link->m_uniqueID.m_string << " = " << insertString << ";\n";
					compilation += ss.str();
				}
			}
			else
			{
				//we are an Input Node
				if(link->partnerLinkInstance != nullptr)
				{
					link->partnerLinkInstance->parentNodeInstance->CompileLink(link->partnerLinkInstance, compilation, compileCounter);
				}



				if( (link->parentLink->OpenGLName.compare("") != 0) || (!link->parentLink->varyingName.empty()) || (!link->parentLink->outName.empty() ) )
				{
					//so we have a openGL variable here USE IT
					std::string variableConversion;
					ConvertToType(	link->partnerLinkInstance->m_dType,	link->m_dType,	link->partnerLinkInstance->varInfo.VariableName.m_string, variableConversion );
					compilation += link->varInfo.VariableName.m_string + " = " + variableConversion + ";\n";
				}
				else if(link->partnerLinkInstance != nullptr)
				{
					link->varInfo = link->partnerLinkInstance->varInfo;
				}
			}
		}
	}


	NodeLinkInstance* NodeInstance::getLinkInstanceByName(const std::string& linkName)
	{
		for(auto it = m_linkInstances.begin(); it != m_linkInstances.end(); ++it)
		{
			if( (it->second)->parentLink->name.compare(linkName) == 0 )
			{
				return it->second.get();
			}
		}

		return nullptr;
	}

	std::map< HashedString, std::unique_ptr<DataFieldInstance> >* NodeInstance::getDataFields()
	{
		return &m_dataFields;
	}

	void NodeInstance::setDataField( HashedString fieldName, float value )
	{
		auto fieldIt = m_dataFields.find(fieldName);
		if(fieldIt != m_dataFields.end() )
		{
			fieldIt->second->m_value = value;
		}
	}

	DataFieldInstance* NodeInstance::getDataFieldInstanceByName( const std::string& fieldName )
	{
		for(auto it = m_dataFields.begin(); it != m_dataFields.end(); ++it)
		{
			if( (it->second)->m_name.compare(fieldName) == 0 )
			{
				return it->second.get();
			}
		}

		return nullptr;
	}

}