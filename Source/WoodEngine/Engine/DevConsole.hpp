#pragma once

#include <string>
#include <vector>
#include "EventSystem.hpp"
#include "..\Math\Vector3.hpp"
#include "Shader.hpp"
#include "Font.hpp"
#include "..\Utility\Utility.hpp"
#include <map>

namespace woodman
{
	enum MessageType
	{
		MESSAGE_TYPE_ERROR,
		MESSAGE_TYPE_WARNING,
		MESSAGE_TYPE_INFO,
		MESSAGE_TYPE_INPUT,
		MESSAGE_TYPE_SIZE
	};

	struct ConsoleCommand
	{
		std::string event;
		std::string help;
		std::vector<std::string> args;
	};

	struct Line
	{
		std::string s;
		MessageType type;
		Line( const std::string& s_, MessageType type_) : s(s_), type(type_) {}
	};
	
	class DevConsole : public EventRecipient<DevConsole>
	{
	public:

		DevConsole(EventSystem* eventsystem, const std::string& commandsFile );

		void initialize();

		bool isOpen(){ return m_isOpen && m_enabled; }
		void toggle(){ m_isOpen = !m_isOpen;}
		void enable(bool enable) { m_enabled = enable;}

		void render();
		void update( double dt );
		void setSize( float size ){ m_size = size; }

		void postMessage( MessageType type, const std::string& message);
		void postHelp( NamedPropertyContainer& parameters );

		void loadCommands(const std::string& filePath );
		void loadFont( const std::string& path );
		void catchKeyEvent( NamedPropertyContainer& parameters );
		void catchMouseWheel( NamedPropertyContainer& parameters );

	private:
		void scroll( int numLines );
		
		Shader m_shaderConsole;
		Shader m_shaderCursor;
		std::shared_ptr<Font> m_font;
		unsigned int m_idVBO_background;
		unsigned int m_idVBO_Cursor;

		std::vector<Line> m_lines;
		std::vector<std::string> m_inputs;
		std::string m_currentInput;
		std::map<HashedString, ConsoleCommand> m_commands;
		std::vector<std::string> m_prevInputs;
		unsigned int m_prevInputPos;
		unsigned int m_cursorPos;

		double m_time;

		float m_size;
		float m_aspect;
		unsigned int m_scroll;
		
		bool m_isOpen;
		bool m_enabled;
		
	};

}