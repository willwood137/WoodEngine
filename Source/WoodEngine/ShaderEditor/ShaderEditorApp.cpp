#include "../stdafx.h"

#include "ShaderEditorApp.hpp"

namespace woodman
{
	ShaderEditorApp::ShaderEditorApp( std::shared_ptr<HINSTANCE> hInstance, int nCmdShow, Vector2i size )
		: OpenGLApp(hInstance, nCmdShow, size)
	{
		m_shaderEditor = std::unique_ptr<ShaderEditor>(new ShaderEditor(&m_eventSystem));
		m_shaderEditor->Initialize();
	}

	ShaderEditorApp::~ShaderEditorApp()
	{

	}

	void ShaderEditorApp::updateFrame()
	{
		m_shaderEditor->update(m_mouseScreenPos);
		OpenGLApp::updateFrame();
	}

	void ShaderEditorApp::render()
	{
		OpenGLApp::render();

		m_shaderEditor->render();
	}
}