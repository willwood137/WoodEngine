#include "../stdafx.h"

#include "ShaderInstance.hpp"

#include <fstream>

namespace woodman
{
	void ShaderInstance::render()
	{
		//render background

	}

	NodeInstance* ShaderInstance::CreateNewNodeInstance(ShaderNode* NodeDefinition, SHADER_TYPE shaderType, const Vector2f& position)
	{
	
		int i = 0;
		HashedString tempID(NodeDefinition->name + "_00");

		
		bool hasUniqueID = false;
		do
		{
			std::stringstream ss;
			ss << i;
			tempID = HashedString(NodeDefinition->name + "_" + ss.str());

			if(m_nodeInstances.find(tempID) == m_nodeInstances.end())
			{
				hasUniqueID = true;
			}
			i++;
		}while(!hasUniqueID);


		NodeInstance* newNode =  new NodeInstance(NodeDefinition, position, shaderType, tempID);

		m_nodeInstances.insert(std::make_pair(tempID, std::unique_ptr<NodeInstance>(newNode) ) );

		return newNode;
	}


	NodeInstance* ShaderInstance::CreateNewNodeInstance( ShaderNode* NodeDefinition, SHADER_TYPE shaderType, const Vector2f& position, const std::string& nameOverride )
	{
		HashedString tempID(nameOverride);
		NodeInstance* newNode =  new NodeInstance(NodeDefinition, position, shaderType, tempID);

		auto nodeIt = m_nodeInstances.find(tempID);
		if( nodeIt != m_nodeInstances.end() )
		{
			return (nodeIt->second.get() );
		}

		m_nodeInstances.insert(std::make_pair(tempID, std::unique_ptr<NodeInstance>(newNode) ) );

		return newNode;
	}

	void ShaderInstance::Compile()
	{
		std::string vertexCode, fragmentCode;
		std::vector<std::string> functions;
		std::set<HashedString> compiledDefinitions;

		//go through and find each node that has a openGL variable or a varying
		std::set<NodeLinkInstance*> GLLinks;
		for(auto it = m_nodeInstances.begin(); it != m_nodeInstances.end(); ++it)
		{
			//check if we need to add the Definition
			ShaderNode* nodeDef = it->second->getDefinitionNode();
			HashedString nodeDefName(nodeDef->name);

			if(compiledDefinitions.find(nodeDefName) == compiledDefinitions.end() )
			{
				compiledDefinitions.insert(nodeDefName);

				for(auto funcIt = nodeDef->functions.begin(); funcIt != nodeDef->functions.end(); ++funcIt)
				{
					functions.push_back( (*funcIt).functionCode );

				}
			}



			std::map< HashedString, std::unique_ptr< NodeLinkInstance > >* links = it->second->getUINodeLinkInstances();

			for(auto LinkIt = links->begin(); LinkIt != links->end(); ++LinkIt)
			{
				std::stringstream ss;
				
				ss << clamp<unsigned int>(LinkIt->second->m_dType.currentSize, LinkIt->second->m_dType.minSize, LinkIt->second->m_dType.maxSize);
				std::string sizeString(ss.str());

				switch( LinkIt->second->m_dType.type)
				{
				case PROPERTYTYPE_SAMPLER2D:
					LinkIt->second->varInfo.variablePrefix = "sampler2D";
					break;
				case PROPERTYTYPE_MATRIX:
					LinkIt->second->varInfo.variablePrefix = "mat" + sizeString;
					break;
				default:
				case PROPERTYTYPE_VECTOR:
				case PROPERTYTYPE_FLOAT:
					if( LinkIt->second->m_dType.currentSize == 1)
						LinkIt->second->varInfo.variablePrefix = "float";
					else
						LinkIt->second->varInfo.variablePrefix = "vec" + sizeString;
					break;
				}

				//setup each links linkName
				if(!LinkIt->second->parentLink->attributeName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString("a_" + LinkIt->second->parentLink->attributeName);
					m_attributes.insert( std::make_pair(LinkIt->second->varInfo.VariableName, LinkIt->second->varInfo) );
				}
				else if(!LinkIt->second->parentLink->OpenGLName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString(LinkIt->second->parentLink->OpenGLName);

					//also use this as a starting point
					GLLinks.insert(LinkIt->second.get());
				}
				else if(!LinkIt->second->parentLink->outName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString(LinkIt->second->parentLink->outName);
					m_outputs.insert(std::make_pair(LinkIt->second->varInfo.VariableName, LinkIt->second->varInfo) );

					//also use this as a starting point
					GLLinks.insert(LinkIt->second.get());
				}
				else if(!LinkIt->second->parentLink->varyingName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString("v_" + LinkIt->second->parentLink->varyingName);
					m_varying.insert( std::make_pair(LinkIt->second->varInfo.VariableName, LinkIt->second->varInfo) );

					//also use this as a starting point
					if(!LinkIt->second->exitNode && it->second->getShaderType() == SHADER_TYPE_VERTEX )
						GLLinks.insert(LinkIt->second.get());
				}
				else if(!LinkIt->second->parentLink->uniformName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString("u_" + LinkIt->second->parentLink->uniformName);
					m_uniforms.insert(std::make_pair(LinkIt->second->varInfo.VariableName, LinkIt->second->varInfo) );
				}
				else
				{
					LinkIt->second->varInfo.VariableName = LinkIt->second->m_uniqueID.m_string;
				}
			}
		}

		//now compile it
		m_compileCounter++;
		for(auto it = GLLinks.begin(); it != GLLinks.end(); it++)
		{
			if( (*it)->parentNodeInstance->getShaderType() == SHADER_TYPE_VERTEX)
			{
				(*it)->parentNodeInstance->CompileLink((*it), vertexCode, m_compileCounter);
			}
			else if( (*it)->parentNodeInstance->getShaderType() == SHADER_TYPE_FRAGMENT)
			{
				(*it)->parentNodeInstance->CompileLink((*it), fragmentCode, m_compileCounter);
			}
		}

		//now put it all together

		//write vertex file / header
		std::ofstream vertexFile;
		vertexFile.open ("ShaderTest.vert");
		vertexFile << "#version 400\n\n";

		std::string typePrefix;

		for(auto it = m_uniforms.begin(); it != m_uniforms.end(); ++it)
		{
			
			vertexFile << "uniform " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		vertexFile << "\n";

		for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
		{
			vertexFile << "in " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		vertexFile << "\n";

		for(auto it = m_varying.begin(); it != m_varying.end(); ++it)
		{
			vertexFile << "out " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		for(auto funcIt = functions.begin(); funcIt != functions.end(); ++funcIt)
		{
			vertexFile << "\n" << *funcIt;
		}

		vertexFile << "\nvoid main(void)\n{\n";
		vertexFile << vertexCode << "\n}";


		//write fragment file / header
		std::ofstream fragFile;
		fragFile.open ("ShaderTest.frag");
		fragFile << "#version 400\n\n";

		for(auto it = m_uniforms.begin(); it != m_uniforms.end(); ++it)
		{
			fragFile << "uniform " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		fragFile << "\n";

		for(auto it = m_varying.begin(); it != m_varying.end(); ++it)
		{
			fragFile << "in " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		fragFile << "\n";

		for(auto it = m_outputs.begin(); it != m_outputs.end(); ++it)
		{
			fragFile << "layout(location = 0) out " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		fragFile << "\n";

		for(auto funcIt = functions.begin(); funcIt != functions.end(); ++funcIt)
		{
			fragFile << "\n" << *funcIt;
		}

		fragFile << "\nvoid main(void)\n{\n";
		fragFile << fragmentCode << "\n}";
	}

	
	void ShaderInstance::PairlinkSlots(const variableInfo& linkAInfo, const variableInfo& linkBInfo  )
	{

	}


	void ShaderInstance::SaveShaderInstance( const std::string& fileName )
	{
		pugi::xml_document doc;

		//doc.append_child().set_name("Shader_WorkFile");

		pugi::xml_node shaderHeader = doc.append_child();//.set_name("Shader_WorkFile");
		shaderHeader.set_name("Shader_WorkFile");
		shaderHeader.append_attribute("name").set_value(m_shaderName.c_str());

		for(auto nodeIt = m_nodeInstances.begin(); nodeIt != m_nodeInstances.end(); ++nodeIt)
		{

			pugi::xml_node xmlNode = shaderHeader.append_child();
			xmlNode.set_name("Node");
			xmlNode.append_attribute("Name").set_value(nodeIt->second->getUniqueID().m_string.c_str());
			xmlNode.append_attribute("ShaderType").set_value(nodeIt->second->getShaderType());
			xmlNode.append_attribute("DefinitionCategory").set_value(nodeIt->second->getDefinitionNode()->category.c_str());
			xmlNode.append_attribute("DefinitionNode").set_value(nodeIt->second->getDefinitionNode()->name.c_str());
			xmlNode.append_attribute("x_Position").set_value(nodeIt->second->getPosition().x);
			xmlNode.append_attribute("y_Position").set_value(nodeIt->second->getPosition().y);
			
			//save data fields

			//save links

			std::map< HashedString, std::unique_ptr< NodeLinkInstance > >* nodeLinks = nodeIt->second->getUINodeLinkInstances();

			for(auto linkIt = nodeLinks->begin(); linkIt != nodeLinks->end(); ++linkIt)
			{
				if(linkIt->second->partnerLinkInstance != nullptr)
				{
					pugi::xml_node linkXmlNode = xmlNode.append_child();
					linkXmlNode.set_name("Link");
					linkXmlNode.append_attribute("Name").set_value( linkIt->second->m_uniqueID.m_string.c_str() );
					pugi::xml_node partnerXmlNode = linkXmlNode.append_child("Partner");
					partnerXmlNode.append_attribute("PartnerNodeName").set_value(linkIt->second->partnerLinkInstance->parentNodeInstance->getUniqueID().m_string.c_str() );
					partnerXmlNode.append_attribute("PartnerLinkName").set_value(linkIt->second->partnerLinkInstance->m_uniqueID.m_string.c_str() );
				}
			}

		}

		//finally save file
		
		doc.save_file(fileName.c_str() );
	}

	

	void ShaderInstance::LoadShaderInstance( const std::string& fileName, std::vector<std::shared_ptr<NodeInstanceData> >& NodeInstanceInfos )
	{
		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( (fileName).c_str() );
		pugi::xml_node topNode = doc->first_child();

		if(std::string(topNode.name()).compare(std::string("Shader_WorkFile") ) == 0 )
		{
			m_shaderName = topNode.attribute("Name").as_string();

			pugi::xml_node dataNode = topNode.first_child();
			while(dataNode)
			{
				std::shared_ptr<NodeInstanceData> data(new NodeInstanceData());
				data->defCategory = dataNode.attribute("DefinitionCategory").as_string();
				data->defName = dataNode.attribute("DefinitionNode").as_string();
				data->Name = dataNode.attribute("Name").as_string();
				data->Position.x = dataNode.attribute("x_Position").as_float();
				data->Position.y = dataNode.attribute("y_Position").as_float();
				data->shaderType = static_cast<SHADER_TYPE>(dataNode.attribute("ShaderType").as_int() );

				pugi::xml_node linkNode = dataNode.first_child();

				while(linkNode)
				{
					std::shared_ptr<LinkInfo> link(new LinkInfo());

					link->Name = linkNode.attribute("Name").as_string();

					pugi::xml_node partnerNode = linkNode.first_child();

					while(partnerNode)
					{
						link->PartnerNodeInstanceName = partnerNode.attribute("PartnerNodeName").as_string();
						link->PartnerLinkName = partnerNode.attribute("PartnerLinkName").as_string();

						partnerNode = partnerNode.next_sibling();
					}

					data->LinkInfos.push_back(link);

					linkNode = linkNode.next_sibling();
				}

				NodeInstanceInfos.push_back(data);

				dataNode = dataNode.next_sibling();
			}
		}
	}

	NodeInstance* ShaderInstance::getNodeInstanceByID( HashedString ID )
	{
		auto it = m_nodeInstances.find(ID);

		if(it != m_nodeInstances.end())
		{
			return it->second.get();
		}

		else 
			return nullptr;
	}
}