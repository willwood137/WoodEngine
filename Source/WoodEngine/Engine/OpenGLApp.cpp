#include "../stdafx.h"
#include <assert.h>
#include "OpenGLApp.hpp"
#include <sstream>
#include <string>
#include "..\Math\Vector2.hpp"
#include <iostream>
#include <fstream>
#include "..\Utility\ProfileSection.hpp"
#include "..\UI\UIStyle.hpp"




namespace woodman
{
	OpenGLApp* OpenGLApp::s_openGLAppInstance = nullptr;
	std::string SERVER_ADDRESS("129.119.222.211");

	const std::string SHADER_RENDERMODEL_SOURCE_FILE_PATH = "..\\Tanks\\Shaders\\RenderModel";

	//std::string SERVER_ADDRESS("129.119.222.166");

	//---------------------------------------------------
	void drawOrigin(Camera* camera)
	{
		glUseProgram(0);

		glPushMatrix();

		const Vector3f& camRotation = camera->getRotation();
		const Vector3f& camPosition = camera->getPosition();



		glRotatef( -camRotation.x, 1, 0, 0);
		glRotatef( -camRotation.y, 0, 1, 0);

		glTranslatef( -camPosition.x, -camPosition.y, -camPosition.z);

		glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);

		glDisable( GL_TEXTURE_2D );
		glDisable( GL_BLEND );
		glDisable( GL_LIGHTING );

		glBegin( GL_LINES );
		{
			glColor3f( 1, 0, 0 );
			glVertex3f( 0, 0, 0 );

			glColor3f( 1, 0, 0 );
			glVertex3f( 100, 0, 0 );

			glColor3f( 0, 1, 0 );
			glVertex3f( 0, 0, 0 );

			glColor3f( 0, 1, 0 );
			glVertex3f( 0, 100, 0 );

			glColor3f( 0, 0, 1 );
			glVertex3f( 0, 0, 0 );

			glColor3f( 0, 0, 1 );
			glVertex3f( 0, 0, 100 );


		}
		glEnd();

		glPopAttrib();
		glPopMatrix();
	}
	//---------------------------------------------------

// 	void drawLineTest()
// 	{
// 		glUseProgram(0);
// 
// 		glPushMatrix();
// 
// 		glLoadIdentity();
// 
// 		GLfloat ctrlpoints[6][3] = {
// 			{ -0.8, -0.5, 0.0}, { -.5, -.5, 0.0},
// 			{ -0.5, -0.2, 0.0}, { .5, .2, 0.0},
// 			{.5, .5, 0.0}, {.8, .5, 0.0} };
// 
// 		glClearColor(0.0, 0.0, 0.0, 0.0);
// 		//glShadeModel(GL_FLAT);
// 		glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 6, &ctrlpoints[0][0]);
// 		glEnable(GL_MAP1_VERTEX_3);
// 		
// 		int i;
// 
// 		glClear(GL_COLOR_BUFFER_BIT);
// 		glColor3f(1.0, 1.0, 1.0);
// 		glBegin(GL_TRIANGLE_STRIP);
// 		for (i = 0; i <= 30; i++) 
// 		{
// 			glEvalCoord1f((GLfloat) i/30.0);
// 		}
// 		glEnd();
// 		/* The following code displays the control points as dots. */
// 		glPointSize(5.0);
// 		glColor3f(1.0, 1.0, 0.0);
// 		glBegin(GL_POINTS);
// 		for (i = 0; i < 6; i++) 
// 			glVertex3fv(&ctrlpoints[i][0]);
// 		glEnd();
// 		glFlush();
// 
// 		glPopMatrix();
// 	}

	OpenGLApp::OpenGLApp( std::shared_ptr<HINSTANCE> hInstance, int nCmdShow, Vector2i size  )
		:m_renderingContext(NULL), 
		m_hdcWindow(NULL), 
		m_windowSize(size),
		m_running(true),
		m_appClock( HashedString("AppClock"), 1.0, &m_eventSystem ),
		m_remainingUpdateTime(0.0),
		m_devConsole(&m_eventSystem, "Engine\\ConsoleCommands.xml"),
		m_model("test_box.xml"),
		m_shader("ModelShader"),
		m_shaderEditor(&m_eventSystem)
	{
		s_openGLAppInstance = this;
		const size_t MAX_LOADSTRING = 100;

		// Global Variables:

		// Initialize global strings
		TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
		LoadString(*hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

		TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
		LoadString(*hInstance, IDC_WOODENGINE, szWindowClass, MAX_LOADSTRING);

		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= *hInstance;
		wcex.hIcon			= LoadIcon(*hInstance, MAKEINTRESOURCE(IDC_WOODENGINE));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= szWindowClass;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		RegisterClassEx(&wcex);

		// Perform application initialization
		//
		m_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, m_windowSize.x, m_windowSize.y, NULL, NULL, *hInstance, NULL);

		if (!m_hWnd)
		{
			throw "FAILED TO CREATED WINDOW";
		}

		ShowWindow( m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);

		m_hAccelTable = LoadAccelerators( *hInstance, MAKEINTRESOURCE( IDC_WOODENGINE));

		m_hdcWindow = GetDC( m_hWnd );

		initializeOpenGL();



		float color[4] = {.4f, 0.4f, 0.4f, 1.0f };
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );
		const float aspectRatio = static_cast< float >(clientRect.right ) / clientRect.bottom;

		UIStyle::LoadDefaultUIStyle("UI\\DefaultStyle.xml");
		//setup timing


		//initialize objects
		//

		// Event System
		//
		m_eventSystem.RegisterObjectForEvent( this, &OpenGLApp::keyDown,"KeyDown" );
	
		m_font = Font::CreateOrGetFont( HASHED_STRING_tahoma );
		Font::LoadShader();
 		m_devConsole.initialize();
 
 		m_model.loadData();
 
		Shader::LoadQuadBuffer();
 		
 		
 		m_shaderEditor.Initialize( );



	}


	OpenGLApp::~OpenGLApp()
	{

	}


	int OpenGLApp::runMainLoop()
	{
		MSG msg;
		while( m_running )
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE ))
			{


				if( msg.message == WM_QUIT )
				{
					break;
				}

				if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			else
			{

				//update timing
				Clock::MasterClock()->update();
				updateFrame();
			}

		}
		return 0;
	}

	void OpenGLApp::updateFrame()
	{

		// TODO improve timing system

		double DeltaTime = m_appClock.getLastTimeDeltaSeconds();
		double FrameStartTime = m_appClock.GetAbsoluteTimeSeconds();


		m_remainingUpdateTime += DeltaTime;
		
		POINT mousePos;
		GetCursorPos( &mousePos );
		ScreenToClient(m_hWnd, &mousePos);

		Vector2f mouseScreenPos(static_cast<float>(mousePos.x), ScreenSize.y - static_cast<float>(mousePos.y) - 42.0f);
		m_shaderEditor.update(mouseScreenPos);


		m_remainingUpdateTime -= .016;



		// Clear.
		//

		// Setup scene-wide transformations.
		//
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );
		const double aspectRatio = static_cast< float >(clientRect.right ) / clientRect.bottom;
// 		glMatrixMode( GL_PROJECTION );
// 		glLoadIdentity();
// 		gluPerspective( 45., aspectRatio, 1, 1200. );


		//render
		//
		{

			
			glClearColor(.04f, .08f, .04f, 1.0f);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			//drawLineTest();

			//drawOrigin(&m_camera);

			
			Vector2f pos(-.99f, .60f);
			float diffLength = -.05f;
			float diff = 0.f;
			std::stringstream ss;
			
			ss << "Testing";
			//Font::DrawTextToScreen("TESTING abcdefg", m_font, RGBA(1.0f, 0.0f, 0.0f, 1.0f), Vector2f(100.0f, 100.0f), 100.0f, ALIGNMENT_LEFT);
			 			

			m_shaderEditor.render();
		}

		// Present
		//
		SwapBuffers( m_hdcWindow );

		while(m_appClock.GetAbsoluteTimeSeconds() - FrameStartTime < 0.016);
	}

	LRESULT CALLBACK OpenGLApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return s_openGLAppInstance->HandleWndProc(hWnd, message, wParam, lParam);
	}

	LRESULT CALLBACK OpenGLApp::HandleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		NamedPropertyContainer params;

		switch (message)
		{

		case WM_KEYDOWN:
			params.insertNamedData(HashedString("Key"), static_cast<unsigned int>(wParam) );
			if( m_devConsole.isOpen() )
				m_eventSystem.FireEvent(std::string("ConsoleKeyDown"), params);
			else
				m_eventSystem.FireEvent(std::string("KeyDown"), params);
			break;
		case WM_LBUTTONDOWN:
			m_eventSystem.FireEvent(std::string("LMouseDown"), params);
			break;
		case WM_LBUTTONUP:
			//params.insertNamedData("Key", static_cast<unsigned int>(wParam) );
			m_eventSystem.FireEvent(std::string("LMouseUp"), params);
			break;
		case WM_RBUTTONDOWN:
			//params.insertNamedData("Key", static_cast<unsigned int>(wParam) );
			m_eventSystem.FireEvent(std::string("RMouseDown"), params);
			break;

		case WM_MOUSEWHEEL:
			params.insertNamedData(HashedString("Direction"), static_cast<unsigned int>(wParam) );
			m_eventSystem.FireEvent(std::string("MouseWheel"), params);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	void OpenGLApp::initializeOpenGL()
	{


// 		m_hdcWindow = GetDC( m_hWnd );
// 		assert( m_hdcWindow );
// 
// 		// Set the window pixel format
// 		//
// 		PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};
// 
// 		pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
// 		pixelFormatDescriptor.nVersion = 1;
// 
// 		pixelFormatDescriptor.dwFlags = 
// 			PFD_DRAW_TO_WINDOW | 
// 			PFD_SUPPORT_OPENGL | 
// 			PFD_DOUBLEBUFFER;
// 		pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;
// 		pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
// 		pixelFormatDescriptor.cColorBits = 32;
// 		pixelFormatDescriptor.cDepthBits = 16;	// No depth buffer for now.
// 
// 		int pixelFormat = ChoosePixelFormat( m_hdcWindow, &pixelFormatDescriptor );
// 		assert (pixelFormat != 0);
// 		SetPixelFormat( m_hdcWindow, pixelFormat, &pixelFormatDescriptor );
// 
// 		// Create the OpenGL render context
// 		//
// 		m_renderingContext = wglCreateContext( m_hdcWindow );
// 		wglMakeCurrent ( m_hdcWindow, m_renderingContext );
// 
// 		glewInit();
// 
// 		// setup initial render state.
// 		glEnable( GL_DEPTH_TEST );
// 		glEnable( GL_TEXTURE_2D );
// 
// 		glDisable( GL_BLEND );
// 		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
// 		glColor4f( 1, 1, 1, 1.0f );
// 
// 		glEnable( GL_CULL_FACE );

		assert( m_hdcWindow );

		// Set the window pixel format
		//
		PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};

		pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
		pixelFormatDescriptor.nVersion = 1;

		pixelFormatDescriptor.dwFlags = 
			PFD_DRAW_TO_WINDOW | 
			PFD_SUPPORT_OPENGL | 
			PFD_DOUBLEBUFFER;
		pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;
		pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
		pixelFormatDescriptor.cColorBits = 32;
		pixelFormatDescriptor.cDepthBits = 0;	// No depth buffer for now.

		int pixelFormat = ChoosePixelFormat( m_hdcWindow, &pixelFormatDescriptor );
		assert (pixelFormat != 0);
		SetPixelFormat( m_hdcWindow, pixelFormat, &pixelFormatDescriptor );

		// Create the OpenGL render context
		//
		m_renderingContext = wglCreateContext( m_hdcWindow );
		wglMakeCurrent ( m_hdcWindow, m_renderingContext );

		// Determine the window size.
		//
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );

		// Set the viewport.
		//
		glViewport( 0, 0, ( GLsizei )m_windowSize.x, ( GLsizei )m_windowSize.y);

		

		// Set the projection.
		//
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( 0, m_windowSize.x, m_windowSize.y, 0, 0, 1.0 ); 

		// Return to ModelView matrix.
		// All following matrix calls will target this matrix.
		glMatrixMode( GL_MODELVIEW );

		glewInit();
	}


	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	void OpenGLApp::shutdownOpenGL()
	{
		assert( m_hdcWindow );
		assert( m_renderingContext );

		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_renderingContext );
		m_renderingContext = 0;

		ReleaseDC( m_hWnd, m_hdcWindow );
		m_hdcWindow = NULL;

	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void OpenGLApp::keyDown( NamedPropertyContainer& parameters )
	{
		unsigned int key;
		parameters.getNamedData(HashedString("Key"), key);

		switch( key )
		{
		case 'F':
			clearAllProfileSections();
			break;
		case 'Q':
			m_running = false;
			break;

		}

	}

	
}