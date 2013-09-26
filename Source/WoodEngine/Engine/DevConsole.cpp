#include "..\stdafx.h"
#include "DevConsole.hpp"
#include "..\Math\Vector2.hpp"
#include "..\libs\pugixml.hpp"
#include <sstream>

namespace woodman
{
	const int CHARACTERS_PER_ROW = 16;
	const float CHARACTERS_TEXCOORD_SIZE = 1.0f / CHARACTERS_PER_ROW;

	const std::string CONSOLE_SHADER_PATH ="Shaders\\Console";
	const std::string CURSOR_SHADER_PATH ="Shaders\\TextCursor";

#define DEVELOPER_CONSOLE_ENABLED

	DevConsole::DevConsole(EventSystem* eventsystem, const std::string& commandsFile ) : 
	EventRecipient(eventsystem), 
		m_isOpen(false), 
		m_size(.04f), 
		m_aspect(1.0f),
		m_scroll(0),  
		m_enabled(true),
		m_prevInputPos(0),
		m_cursorPos(0),
		m_time(0)
	{
	
#ifndef DEVELOPER_CONSOLE_ENABLED
		m_enabled = false;
#endif

		eventsystem->RegisterObjectForEvent( this, &DevConsole::catchKeyEvent, "ConsoleKeyDown" );
		eventsystem->RegisterObjectForEvent( this, &DevConsole::catchKeyEvent, "KeyDown" );
		eventsystem->RegisterObjectForEvent( this, &DevConsole::catchMouseWheel, "MouseWheel" );
		eventsystem->RegisterObjectForEvent( this, &DevConsole::postHelp, "ConsoleHelp" );

		postMessage(MESSAGE_TYPE_INFO, "Loading Wood Engine");
		postMessage(MESSAGE_TYPE_INFO, "Type \"Help\" for a list of commands");

		loadCommands(commandsFile);
	}

	void DevConsole::initialize()
	{
		m_font = Font::CreateOrGetFont( HASHED_STRING_MainFont_EN );
		std::vector<Vector2f> verts;
		verts.push_back(Vector2f(-1.0f, 0.0f));
		verts.push_back(Vector2f(-1.0f, 1.0f));
		verts.push_back(Vector2f(1.0f, 0.0f));
		verts.push_back(Vector2f(1.0f, 1.0f));

		m_idVBO_background = LoadBufferObject( GL_ARRAY_BUFFER, verts.data(), sizeof( Vector2f ) * 4 );

		verts.clear();

		verts.push_back(Vector2f(0.0f, 0.0f));
		verts.push_back(Vector2f(0.0f, 1.0f));

		m_idVBO_Cursor = LoadBufferObject( GL_ARRAY_BUFFER, verts.data(), sizeof( Vector2f ) * 2 );


		m_shaderConsole.loadShader(CONSOLE_SHADER_PATH);
		m_shaderCursor.loadShader(CURSOR_SHADER_PATH); 
		m_shaderConsole.addAttribute(HASHED_STRING_a_position);
		m_shaderConsole.addUniform(HASHED_STRING_u_TopLeft);
		m_shaderConsole.addUniform(HASHED_STRING_u_BottomRight);
		m_shaderConsole.addUniform(HASHED_STRING_u_middleLine);
		m_shaderConsole.addUniform(HASHED_STRING_u_color);

		m_shaderCursor.addAttribute(HASHED_STRING_a_position);
		m_shaderCursor.addUniform(HASHED_STRING_u_position);
		m_shaderCursor.addUniform(HASHED_STRING_u_color);
		m_shaderCursor.addUniform(HASHED_STRING_u_size);
		m_shaderCursor.addUniform(HASHED_STRING_u_debugInt);
	}

	void DevConsole::loadCommands( const std::string& filePath )
	{

		ConsoleCommand help;
		help.event = "ConsoleHelp";
		help.help = "Displays all commands with a description";
		m_commands[HASHED_STRING_help] = help;

		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( (filePath).c_str() );
		pugi::xml_node topNode = doc->first_child();

		if(std::string(topNode.name()).compare(std::string("ConsoleCommands") ) == 0 )
		{
			pugi::xml_node commandNode = topNode.first_child();
			while(commandNode)
			{
				ConsoleCommand cmd;
				HashedString command(commandNode.attribute("name").as_string());
				cmd.event = commandNode.attribute("event").as_string();
				cmd.help = commandNode.attribute("help").as_string();
				std::string s = commandNode.attribute("args").as_string();
				if(!s.empty() )
				{
					std::stringstream ss;
					ss << s;

					while(ss.good())
					{
						std::string temp;
						ss >> temp;
						cmd.args.push_back(temp);
					}
				}

				m_commands[command] = cmd;
				commandNode = commandNode.next_sibling();
			}
		}
	}

	void DevConsole::render()
	{
		if(m_enabled)
		{
			if(m_isOpen)
			{
				m_shaderConsole.load();

				glBindBuffer(GL_ARRAY_BUFFER, m_idVBO_background);
				glFrontFace(GL_CW);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glUniform2f(m_shaderConsole.getUniformID(HASHED_STRING_u_TopLeft), -1.0f, 1.0f );
				glUniform2f(m_shaderConsole.getUniformID(HASHED_STRING_u_BottomRight), 1.0f, 0.0f );
				glUniform1f(m_shaderConsole.getUniformID(HASHED_STRING_u_middleLine), .1f );
				glUniform3f(m_shaderConsole.getUniformID(HASHED_STRING_u_color), 0.0f, 0.8f, 0.0f );
				m_shaderConsole.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(Vector2f), 0);
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				m_shaderConsole.disableAttribute(HASHED_STRING_a_position);

				size_t numLines = m_lines.size();
				float y = .1f;
				for( size_t i = m_scroll; i >= 0 && i < numLines && y <= 1.0f - m_size; --i)
				{
					RGBA color;

					switch(m_lines[i].type)
					{
					case MESSAGE_TYPE_INFO:
						color = RGBA(1.0f, 1.0f, 1.0f, .9f);
						break;
					case MESSAGE_TYPE_ERROR:
						color = RGBA(1.0f, .2f, .2f, .8f);
						break;
					case MESSAGE_TYPE_WARNING:
						color = RGBA(1.f, 1.0f, .2f, .8f);
						break;
					case MESSAGE_TYPE_INPUT:
						color = RGBA(.2f, 1.0f, .2f, .8f);
						break;
					}

					Font::DrawTextToScreen(m_lines[i].s, m_font, color, Vector2f(-.99f, y), m_size, ALIGNMENT_LEFT );
					y += m_size;
				}

				Font::DrawTextToScreen(">" + m_currentInput, m_font, RGBA(1.0f, 1.0f, 1.0f, 1.0f), Vector2f(-.99f,.01f), m_size, ALIGNMENT_LEFT);

				//render cursor
				m_shaderCursor.load();
				float cursorstart = -.985f + Font::getLengthOfString(">", m_font, m_size );
				glBindBuffer(GL_ARRAY_BUFFER, m_idVBO_Cursor);
				if( m_cursorPos != 0)
					glUniform2f(m_shaderCursor.getUniformID(HASHED_STRING_u_position), cursorstart + Font::getLengthOfString(m_currentInput.substr(0, m_cursorPos ), m_font, m_size ), .01f );
				else
					glUniform2f(m_shaderCursor.getUniformID(HASHED_STRING_u_position), cursorstart , .01f );
				glUniform1f(m_shaderCursor.getUniformID(HASHED_STRING_u_size), m_size);
				glUniform4f(m_shaderCursor.getUniformID(HASHED_STRING_u_color), 0.0f, 0.8f, 0.0f, 1.0f );
				glUniform1i(m_shaderCursor.getUniformID(HASHED_STRING_u_debugInt), (int)(m_time * 2) % 2 );
				m_shaderCursor.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(Vector2f), 0);
				glLineWidth(2.0);
				glDrawArrays( GL_LINES, 0, 2 );
				m_shaderCursor.disableAttribute(HASHED_STRING_a_position);
			}
		}
	}


	void DevConsole::update( double dt )
	{
		m_time += dt;
	}

	void DevConsole::postHelp( NamedPropertyContainer& parameters )
	{

		std::map<HashedString, ConsoleCommand>::iterator it = m_commands.begin();
		while( it != m_commands.end())
		{
			std::string s(it->first.m_string);
			size_t numArgs = it->second.args.size();
			for(size_t i = 0; i < numArgs; ++i)
				s += " " + it->second.args[i];
			postMessage(MESSAGE_TYPE_INFO, s + " - " + it->second.help);
			++it;
		}
	}

	void DevConsole::scroll(int scrollAmount)
	{
		m_scroll += scrollAmount;
		m_scroll = clamp<int>(m_scroll, 0, m_lines.size() - 1);
	}

	void DevConsole::postMessage( MessageType type, const std::string& message)
	{
		m_lines.push_back(Line( message, type) );
		m_scroll = m_lines.size() - 1;
	}


	void DevConsole::catchKeyEvent( NamedPropertyContainer& parameters )
	{
		if( m_enabled)
		{

			unsigned int key; 
			parameters.getNamedData(HashedString("Key"), key);

			if( key == VK_OEM_3)
			{
				m_isOpen = !m_isOpen;
			}
			else if( m_isOpen)
			{
				if( (key >= '0' && key <= 'Z') || key == VK_SPACE )
				{
					m_cursorPos = clamp<unsigned int>(m_cursorPos, 0, m_currentInput.size());				
					m_currentInput.insert(m_cursorPos, 1, tolower(key));
					m_cursorPos++;
				}
				else if ( key == VK_PRIOR)
				{
					scroll(-10);
				}
				else if ( key == VK_NEXT)
				{
					scroll(10);
				}
				else if( (key == VK_RETURN) )
				{
					std::stringstream ss;
					ss << m_currentInput;
					std::string temp;
					ss >> temp;
					HashedString command(temp);

					std::map<HashedString, ConsoleCommand>::iterator it = m_commands.find(command);
					if(it != m_commands.end() )
					{
						std::vector<std::string> args;
						while(ss.good() )
						{
							ss>>temp;
							args.push_back(temp);
						}
						if(args.size() == it->second.args.size())
						{
							NamedPropertyContainer param;
							size_t numArgs = args.size();
							for(size_t i = 0; i < numArgs; ++i)
							{
								param.insertNamedData(it->second.args[i], args[i] );
							}
							param.insertNamedData(HASHED_STRING_Console, this);
							postMessage(MESSAGE_TYPE_INPUT, "> " + m_currentInput);
							EventRecipient::p_eventSystem->FireEvent(it->second.event, param );
						}
						else
						{
							std::stringstream ss2;
							ss2 << "Incorrect Number of Arguments: " 
								<< command.m_string
								<< " takes "
								<<it->second.args.size()
								<<" arguments, not "
								<< args.size();
							postMessage(MESSAGE_TYPE_WARNING, ss2.str());
						}
					}
					else
					{
						std::stringstream ss2;
						ss2 << ">Invalid Command - " << temp;
						postMessage(MESSAGE_TYPE_WARNING, ss2.str());
					}

					m_prevInputs.push_back(m_currentInput);
					m_currentInput.clear();
					m_prevInputPos = m_prevInputs.size();
					m_cursorPos = 0;
				}
				else if( key == VK_BACK )
				{
					if( !m_currentInput.empty() )
					{
						if( m_cursorPos > 0)
							m_currentInput.erase(m_cursorPos-1, 1);
						m_cursorPos--;
					}
				}
				else if( key == VK_DELETE )
				{
					if( !m_currentInput.empty() )
					{
						if( m_cursorPos < m_currentInput.size())
							m_currentInput.erase(m_cursorPos, 1);
					}
				}
				else if( key == VK_ESCAPE )
				{
					if( m_currentInput.empty() )
						m_isOpen = false;
					else
					{
						m_cursorPos = 0;
						m_currentInput.clear();
					}
				}
				else if (key == VK_DOWN)
				{
					if( !m_prevInputs.empty())
					{
						if(m_prevInputPos < m_prevInputs.size() - 1)
							m_prevInputPos++;
						else
							m_prevInputPos = 0;
					
						m_currentInput = m_prevInputs[m_prevInputPos];
					}
				}
				else if (key == VK_UP)
				{
					if( !m_prevInputs.empty())
					{
						if(m_prevInputPos > 0)
							m_prevInputPos--;
						else
							m_prevInputPos = m_prevInputs.size() - 1;

						m_currentInput = m_prevInputs[m_prevInputPos];
					}
				}
				else if (key == VK_LEFT)
				{
					if(m_cursorPos > 0)
						m_cursorPos--;
				}
				else if (key == VK_RIGHT)
				{
					if(m_cursorPos < m_currentInput.size())
						m_cursorPos++;
				}
				else if (key == VK_HOME)
				{
					m_cursorPos = 0;
				}
				else if (key == VK_END)
				{
					m_cursorPos = m_currentInput.size();
				}
			}
		}
	}

	void DevConsole::catchMouseWheel( NamedPropertyContainer& parameters )
	{
		if(m_enabled)
		{
			unsigned int wParam;
			parameters.getNamedData(HashedString("Direction"), wParam);
			if( (  GET_WHEEL_DELTA_WPARAM(wParam) ) > 0)
			{
				scroll(-1);
			}
			else
			{
				scroll(1);
			}
		}


	}
}