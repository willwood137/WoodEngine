#include "../stdafx.h"

#include "ShaderInstance.hpp"

#include <fstream>

namespace woodman
{
	void ShaderInstance::render()
	{
		//render background

	}

	std::shared_ptr<NodeInstance> ShaderInstance::CreateNewNodeInstance(std::shared_ptr<ShaderNode> NodeDefinition, SHADER_TYPE shaderType, const Vector2f& position)
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


		std::shared_ptr<NodeInstance> newNode( new NodeInstance(NodeDefinition, position, shaderType, tempID) );

		m_nodeInstances.insert(std::make_pair(tempID, newNode) );

		return newNode;
	}

	void ShaderInstance::Compile()
	{
		std::string vertexCode, fragmentCode;

		//go through and find each node that has a openGL variable or a varying
		std::set<std::shared_ptr<NodeLinkInstance> > GLLinks;
		for(auto it = m_nodeInstances.begin(); it != m_nodeInstances.end(); ++it)
		{
			std::map< HashedString, std::shared_ptr< NodeLinkInstance > >* links = it->second->getUINodeLinkInstances();
			for(auto LinkIt = links->begin(); LinkIt != links->end(); ++LinkIt)
			{
				//setup each links linkName
				if(!LinkIt->second->parentLink->attributeName.empty() )
				{
					m_attributes.insert(HashedString( LinkIt->second->parentLink->attributeName) );
					LinkIt->second->m_linkName = "a_" + LinkIt->second->parentLink->attributeName;
				}
				else if(!LinkIt->second->parentLink->OpenGLName.empty() )
				{
					LinkIt->second->m_linkName = LinkIt->second->parentLink->OpenGLName;

					//also use this as a starting point
					GLLinks.insert(LinkIt->second);
				}
				else if(!LinkIt->second->parentLink->outName.empty() )
				{
					LinkIt->second->m_linkName = LinkIt->second->parentLink->outName;
					m_outputs.insert(HashedString(LinkIt->second->parentLink->outName));

					//also use this as a starting point
					GLLinks.insert(LinkIt->second);
				}
				else if(!LinkIt->second->parentLink->varyingName.empty() )
				{
					m_varying.insert(HashedString( LinkIt->second->parentLink->varyingName ) );
					LinkIt->second->m_linkName = "v_" + LinkIt->second->parentLink->varyingName;

					//also use this as a starting point
					if(!LinkIt->second->exitNode)
						GLLinks.insert(LinkIt->second);
				}
				else if(!LinkIt->second->parentLink->uniformName.empty() )
				{
					m_uniforms.insert(HashedString( LinkIt->second->parentLink->uniformName) );

					LinkIt->second->m_linkName = "u_" + LinkIt->second->parentLink->uniformName;

				}
				else
				{
					LinkIt->second->m_linkName = LinkIt->second->m_uniqueID.m_string;
				}
				
					


			}
		}
		//now compile it

		for(auto it = GLLinks.begin(); it != GLLinks.end(); it++)
		{
			if( (*it)->parentNodeInstance->getShaderType() == SHADER_TYPE_VERTEX)
				CompileLink((*it), vertexCode);

			if( (*it)->parentNodeInstance->getShaderType() == SHADER_TYPE_FRAGMENT)
				CompileLink((*it), fragmentCode);
		}

		//now put it all together

		//write vertex file / header
		std::ofstream vertexFile;
		vertexFile.open ("ShaderTest.vert");
		vertexFile << "#version 400\n\n";

		for(auto it = m_uniforms.begin(); it != m_uniforms.end(); ++it)
		{
			vertexFile << "uniform " << "mat4 " << "u_" << it->m_string <<";\n";
		}

		vertexFile << "\n";

		for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
		{
			vertexFile << "in " << "vec3 " << "a_" << it->m_string <<";\n";
		}

		vertexFile << "\n";

		for(auto it = m_varying.begin(); it != m_varying.end(); ++it)
		{
			vertexFile << "out " << "vec2 " << "v_" << it->m_string <<";\n";
		}

		vertexFile << "void main(void)\n{\n";
		vertexFile << vertexCode << "\n}";


		//write fragment file / header
		std::ofstream fragFile;
		fragFile.open ("ShaderTest.frag");
		fragFile << "#version 400\n\n";

		for(auto it = m_uniforms.begin(); it != m_uniforms.end(); ++it)
		{
			fragFile << "uniform " << "mat4 " << "u_" << it->m_string <<";\n";
		}

		fragFile << "\n";

		for(auto it = m_varying.begin(); it != m_varying.end(); ++it)
		{
			fragFile << "in " << "vec2 " << "v_" << it->m_string <<";\n";
		}

		fragFile << "\n";

		for(auto it = m_outputs.begin(); it != m_outputs.end(); ++it)
		{
			fragFile << "out " << "vec4 " << it->m_string <<";\n";
		}

		fragFile << "\n";

		fragFile << "void main(void)\n{\n";
		fragFile << fragmentCode << "\n}";



	}


	void ShaderInstance::CompileLink(std::shared_ptr<NodeLinkInstance> link, std::string& compilation)
	{
		//we need to resolve this before we can compile this

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
						std::shared_ptr<NodeLinkInstance> tempLinkInstance = link->parentNodeInstance->getLinkInstanceByName(tempString.substr(1));

						if(tempLinkInstance != nullptr)
						{
							if(tempLinkInstance->partnerLinkInstance != nullptr)
							{
								CompileLink(tempLinkInstance, compilation);
							}

							insertString += tempLinkInstance->m_linkName + " ";
						}
					}
					else
					{
						insertString += tempString + " ";
					}
				}
				
				ss << "vec4 " << link->m_uniqueID.m_string << " = " << insertString << ";\n";
				compilation += ss.str();
			}
		}
		else
		{
			//we are an Input Node
			if(link->partnerLinkInstance != nullptr)
			{
				CompileLink(link->partnerLinkInstance, compilation);
				link->m_linkName = link->partnerLinkInstance->m_linkName;
			}

			if(link->parentLink->OpenGLName.compare("") != 0)
			{
				//so we have a openGL variable here USE IT
				compilation += link->parentLink->OpenGLName + " = " + link->partnerLinkInstance->m_linkName + ";\n";
			}
			else if(!link->parentLink->varyingName.empty())
			{
				//so we have a openGL variable here USE IT
				compilation += "v_" + link->parentLink->varyingName + " = " + link->partnerLinkInstance->m_linkName + ";\n";
			}
			else if(!link->parentLink->outName.empty())
			{
				//so we have a openGL variable here USE IT
				compilation += link->parentLink->outName + " = " + link->partnerLinkInstance->m_linkName + ";\n";
			}
		}
	}

	

	void ShaderInstance::linkSlots( HashedString linkAID, HashedString linkBID )
	{
		std::shared_ptr<NodeLinkInstance> linkA, linkB;

		//first find slot A and B
		for(auto it = m_nodeInstances.begin(); it != m_nodeInstances.end(); ++it)
		{
			std::shared_ptr<NodeLinkInstance> tempLink;
			tempLink = (it->second)->getLinkByID(linkAID);
			if(tempLink != nullptr)
			{
				linkA = tempLink;
			}

			tempLink = (it->second)->getLinkByID(linkBID);
			if(tempLink != nullptr)
			{
				linkB = tempLink;
			}

			if(linkA != nullptr && linkB != nullptr)
				break;
		}

		linkA->partnerLinkInstance = linkB;
		linkB->partnerLinkInstance = linkA;


	}
}