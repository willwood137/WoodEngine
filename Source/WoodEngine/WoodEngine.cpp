// ShadowMaster.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <time.h>
#include <sstream>
#include <windows.h>
#include <stdio.h>
#include <iostream>

#include "resource.h"
#include "Engine\OpenGLApp.hpp"
#include "Math\Vector2.hpp"
#include "Utility\ConsoleRedirector.hpp"
#include "Engine\Clock.hpp"
#include <string>


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	int resultCode;



	try
	{
		
		woodman::OpenGLApp myOpenGLApp( std::make_shared<HINSTANCE>(hInstance), nCmdShow, ScreenSize );

		resultCode = myOpenGLApp.runMainLoop();
	}
	catch( ... )
	{
		MessageBox( NULL,
			TEXT("The application threw an exception."),
			TEXT("Application Exception"),
			MB_OK|MB_ICONERROR );

	}

#ifdef _DEBUG

	//assert( _CrtCheckMemory() );

	//_CrtDumpMemoryLeaks();

#endif

	return resultCode;
}