#include "../stdafx.h"

#include "ModelPreviewWidget.hpp"

namespace woodman
{
	ModelPreviewWidget::ModelPreviewWidget( std::shared_ptr<UICanvas> ParentCanvas, std::shared_ptr<UIWidget> parentWidget, const std::string& name, HashedString uniqueID )
		: UIWidget(ParentCanvas, parentWidget, name, uniqueID),
		m_camera(new Camera(Vector3f(0.0f, 0.0f, -20.0f) ) )
	{
		
	}

	void ModelPreviewWidget::loadBackgroundTexture( const std::string& texturePath )
	{
		m_backgroundTexture = Texture::CreateOrGetTexture(texturePath);
	}

	void ModelPreviewWidget::loadModelFromFile(const std::string& modelFilePath)
	{
		m_model = std::shared_ptr<Model>(new Model( modelFilePath ) );
		m_model->loadData();
	}

	void ModelPreviewWidget::updateShader( const std::string& shaderFilePath )
	{

		//remove the old shader!
		if(m_previewShader != nullptr)
		{
			Shader::DestroyShader(m_previewShader);	
		}

		m_previewShader = Shader::CreateOrGetShader(shaderFilePath);
	}

	void drawOrigin(Camera* camera);

	void ModelPreviewWidget::render(std::shared_ptr<UIMouse> currentMouse)
	{

		drawOrigin(m_camera.get());

		if(m_previewShader != nullptr && m_previewShader->IsLoaded() )
		{

			//first we get the MVP Matrix
			Matrix4f mvpMatrix, camMatrix;

			GLint viewport[4];
			GLdouble modelview[16];
			GLdouble projection[16];
			GLfloat winX, winY;
			GLdouble posX, posY, posZ;

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluPerspective( 45., 1.837, 1, 1200. );

			glGetDoublev( GL_PROJECTION_MATRIX, projection );
			glGetIntegerv( GL_VIEWPORT, viewport );

			for(size_t i = 0; i < 16; ++i)
			{
				mvpMatrix[i] = projection[i];
			}

			m_camera->update(.016f);
			camMatrix = m_camera->getMatrix();
			mvpMatrix = camMatrix * mvpMatrix;

 			glEnable(GL_DEPTH_TEST);
 			glCullFace(GL_CW);
			glFrontFace(GL_CCW);

			std::vector<BatchData>* batches(m_model->getBatchData());

			m_previewShader->load();

			if(m_previewShader->SetUniformMatrix(HASHED_STRING_u_MVP, mvpMatrix ) )
			{
				Texture::ApplyTexture(m_model->getMaterialTexture("Diffuse"));

				for(auto batchIt = batches->begin(); batchIt != batches->end(); ++batchIt)
				{

					glBindBuffer(GL_ARRAY_BUFFER, batchIt->idVBO);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchIt->idIBO);

					m_previewShader->setUpAttrbituesFromModel(m_model);

					glDrawElements(GL_TRIANGLES, batchIt->numIndices, GL_UNSIGNED_INT, 0);

				}

			}
		}
	}

	void ModelPreviewWidget::MouseDrag(std::shared_ptr<UIMouse> currentMouse)
	{

	}
}