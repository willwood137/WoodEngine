#include "..\stdafx.h"
#include "Camera.hpp"
#include "DevConsole.hpp"
#include "..\Utility\Utility.hpp"

#include <sstream>


namespace woodman
{
	const int MOUSE_CENTER_X = 512;
	const int MOUSE_CENTER_Y = 384;

	const float IMPULSE_DAMP = .50f;
	const float VELOCITY_DAMP = .8f;
	const float VELOCITY_MAX = .00015f;
	const float VELOCITY_MIN_STOP = IMPULSE_DAMP * .000005f;

	Camera::Camera(){ }

	Camera::Camera( Vector3f& Origin)
		: m_mousePos(0, 0)
	{
		m_origin = Origin;
	}

	Camera::~Camera()
	{
		//unlockMouse();
	}

	void Camera::update( float delta )
	{
		delta = clamp( delta, 0.0f, 1.0f / 30.0f);

		Vector3f impulse, rotation;
		updateInput(impulse, rotation);

		impulse *= IMPULSE_DAMP;

		m_rotationAngles -= rotation;
		m_rotationAngles.x = clamp<float>( m_rotationAngles.x, -90, 90);
		Matrix4f rotationMatrix;
		ConstructRotationMatrixDegrees( rotationMatrix, m_rotationAngles.y, m_rotationAngles.x, m_rotationAngles.z );
		impulse = rotationMatrix.getTransformedVector( impulse );

		m_velocity += impulse;

		m_position += m_velocity * (60.0f * delta );

		m_velocity.y *= VELOCITY_DAMP;
		m_velocity.x *= VELOCITY_DAMP;
		m_velocity.z *= VELOCITY_DAMP;

		Matrix4f trans;
		ConstructTranslationMatrix(trans, m_position);
		//Matrix4 rot = Matrix4::ConstructRotationMatrixDegrees( m_rotationAngles.y, m_rotationAngles.x, m_rotationAngles.z );
		trans.NegateTranslation();
		rotationMatrix.TransposeRotation();

		m_matrix = trans * rotationMatrix;
	}


	void Camera::lockMouse()
	{
		SetCursorPos( MOUSE_CENTER_X, MOUSE_CENTER_Y);
		ShowCursor(false);
	}

	void Camera::unlockMouse()
	{
		ShowCursor( true );
	}
	void Camera::updateInput(Vector3f& impulse, Vector3f& rotation)
	{

		const float MOUSE_YAW_POWER = 0.2f;
		const float MOUSE_PITCH_POWER = 0.2f;

		if(isKeyDown('C'))
		{

			POINT mousePos;
			GetCursorPos( &mousePos );

			float mouseDeltaX( mousePos.x - (float)MOUSE_CENTER_X );
			float mouseDeltaY( mousePos.y - (float)MOUSE_CENTER_Y );


			SetCursorPos( MOUSE_CENTER_X, MOUSE_CENTER_Y);

			m_mousePos = Vector2i(mousePos.x, mousePos.y);

			// Update camera yaw.
			//
			rotation.y = mouseDeltaX * MOUSE_YAW_POWER;
			rotation.x = mouseDeltaY * MOUSE_PITCH_POWER;

		}



		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if( isKeyDown( VK_UP ) || isKeyDown( 'W' ))
		{
			impulse.z -= 1.0f;
		}
		if( isKeyDown( VK_DOWN ) || isKeyDown( 'S' ))
		{
			impulse.z += 1.0f;
		}
		if( isKeyDown( VK_LEFT ) || isKeyDown( 'A' ))
		{
			impulse.x -= 1.0f;
		}
		if( isKeyDown( VK_RIGHT ) || isKeyDown( 'D' ))
		{
			impulse.x += 1.0f;
		}
		if( isKeyDown( VK_SPACE ) )
		{

			impulse.y = 1.0f;
		}

		if( isKeyDown( 'Y' ) || isKeyDown( VK_CONTROL ) )
		{
			impulse.y = -1.0f;
		}

		impulse.Normalize();
		

	}

}