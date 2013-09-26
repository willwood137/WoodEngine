#pragma once

#include "..\Math\Vector2.hpp"
#include "..\resource.h"
#include "..\Engine\Clock.hpp"
#include "Model.hpp"
#include "MetaShader.hpp"
#include "Font.hpp"
#include "EventSystem.hpp"
#include "DevConsole.hpp"
#include "..\ShaderEditor\ShaderEditor.hpp"
#include "..\UI\UICanvas.hpp"

#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include <list>


namespace woodman
{
	class OpenGLApp
	{
	public:
		

		OpenGLApp( std::shared_ptr<HINSTANCE> hInstance, int nCmdShow, Vector2i size );
		~OpenGLApp();


		int runMainLoop();
		void SpawnProjectile( NamedPropertyContainer& parameters );
		void keyDown( NamedPropertyContainer& parameters );

		static OpenGLApp* s_openGLAppInstance;

	private:

		HWND m_hWnd;
		HACCEL m_hAccelTable;
		HGLRC m_renderingContext;
		HDC m_hdcWindow;
		Vector2i m_windowSize;
		bool m_running;
		EventSystem m_eventSystem;
		Clock m_appClock;
		std::shared_ptr<Font> m_font;
		DevConsole m_devConsole;
		ShaderEditor m_shaderEditor;


		Model m_model;
		MetaShader m_shader;
		
		double m_remainingUpdateTime;
		
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		LRESULT CALLBACK HandleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		void initializeOpenGL();
		void shutdownOpenGL();
		void updateFrame();
	};


	

}