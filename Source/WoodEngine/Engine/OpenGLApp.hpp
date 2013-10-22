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
		virtual ~OpenGLApp();


		int runMainLoop();

		static OpenGLApp* s_openGLAppInstance;

	protected:

		HWND m_hWnd;
		HACCEL m_hAccelTable;
		HGLRC m_renderingContext;
		HDC m_hdcWindow;
		Vector2i m_windowSize;

		bool m_running;
		EventSystem m_eventSystem;
		Clock m_appClock;
		DevConsole m_devConsole;

		double m_remainingUpdateTime;
		double m_framDeltaTime;
		Vector2f m_mouseScreenPos;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual LRESULT CALLBACK HandleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		virtual void initializeOpenGL();
		void shutdownOpenGL();

		virtual void updateFrame();
		virtual void preRender();
		virtual void render();
		virtual void postRender();

	};


	

}