#include "..\stdafx.h"
#include "Camera.hpp"
#include "DevConsole.hpp"
#include "..\Utility\Utility.hpp"

#include <sstream>


namespace woodman
{

	

	const float IMPULSE_DAMP = .50f;
	const float VELOCITY_DAMP = .8f;
	const float VELOCITY_MAX = .00015f;
	const float VELOCITY_MIN_STOP = IMPULSE_DAMP * .000005f;

	Camera::Camera( const Vector3f& position, Vector3f& target, EventSystem* eventsystem, Clock* parentClock )
		: m_position(position), 
		m_target(target), 
		m_mode(CAMERAMODE_OVERLORD), 
		EventRecipient(eventsystem),
		m_clock(HashedString("Camera"), 1.0, parentClock )
	{
		Vector3f diff = m_target - m_position;
		Vector2f xz(diff.x, diff.z);
		m_rotationAngles.y = (float)(atan2((double)(diff.z), (double)(diff.x))) * CONVERT_TO_DEGREES;
		m_rotationAngles.x = static_cast<float>(atan2((double)(diff.y), (double)(xz.calcLength()))) * CONVERT_TO_DEGREES;
		m_length = static_cast<float>(diff.calcLength());

	}



	void Camera::initialize()
	{
		p_eventSystem->RegisterObjectForEvent(this, &Camera::setMode, "CamMode");
		p_eventSystem->RegisterObjectForEvent(this, &Camera::catchCamInfo, "CamInfo");
		p_eventSystem->RegisterObjectForEvent(this, &Camera::catchMouseWheel, "MouseWheel");
		p_eventSystem->RegisterObjectForEvent(this, &Camera::catchRMouseDown, "RMouseDown");
	}

	void Camera::update()
	{
		
		float delta = clamp<float>( static_cast<float>(m_clock.getLastTimeDeltaSeconds()), 0.0f, 1.0f / 30.0f);


		Matrix4f rotationMatrix;

		if(m_mode == CAMERAMODE_OVERLORD)
		{
			Vector3f impulse, rotation;
			updateInput(impulse, rotation);
			impulse *= IMPULSE_DAMP;
			m_velocity += impulse;
			m_velocity.y *= VELOCITY_DAMP;
			m_velocity.x *= VELOCITY_DAMP;
			m_velocity.z *= VELOCITY_DAMP;
			m_rotationAngles.x += m_velocity.x;
			m_rotationAngles.y += m_velocity.y;
			
			m_rotationAngles.x = clamp<float>( m_rotationAngles.x, -70, -10);
			double dy = sin((double)-m_rotationAngles.x * CONVERT_TO_RADIANS) * (double)m_length;
			double xzLength = cos((double)-m_rotationAngles.x * CONVERT_TO_RADIANS)  * (double)m_length;
			double dx = sin((double)m_rotationAngles.y * CONVERT_TO_RADIANS) * xzLength;
			double dz = cos((double)m_rotationAngles.y * CONVERT_TO_RADIANS) * xzLength;

			ConstructRotationMatrixDegrees( rotationMatrix, m_rotationAngles.y, m_rotationAngles.x, m_rotationAngles.z );
			m_position.x = m_target.x + static_cast<float>(dx);
			m_position.y = m_target.y + static_cast<float>(dy);
			m_position.z = m_target.z + static_cast<float>(dz);
		}
		else
		{
			Vector3f impulse, rotation;
			updateInput(impulse, rotation);

			impulse *= IMPULSE_DAMP;

			m_rotationAngles -= rotation;
			m_rotationAngles.x = clamp<float>( m_rotationAngles.x, -90, 90);
			ConstructRotationMatrixDegrees( rotationMatrix, m_rotationAngles.y, m_rotationAngles.x, m_rotationAngles.z );

			impulse = rotationMatrix.getTransformedVector( impulse );

			m_velocity += impulse;

			m_position += m_velocity * (60.0f * delta );

			m_velocity.y *= VELOCITY_DAMP;
			m_velocity.x *= VELOCITY_DAMP;
			m_velocity.z *= VELOCITY_DAMP;
		}
		Matrix4f trans;
		ConstructTranslationMatrix(trans, m_position);
		trans.NegateTranslation();
		rotationMatrix.TransposeRotation();

		m_matrix = trans * rotationMatrix;
	}


	void Camera::lockMouse()
	{
		SetCursorPos( m_mousePos.x, m_mousePos.y);
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

		if( m_mode == CAMERAMODE_FREE)
		{
			POINT mousePos;
			GetCursorPos( &mousePos );

			float mouseDeltaX( mousePos.x - (float)m_mousePos.x );
			float mouseDeltaY( mousePos.y - (float)m_mousePos.y );

			// Update camera yaw.
			//
			rotation.y = mouseDeltaX * MOUSE_YAW_POWER;
			rotation.x = mouseDeltaY * MOUSE_PITCH_POWER;



			SetCursorPos( m_mousePos.x, m_mousePos.y);



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
		else if(m_mode == CAMERAMODE_OVERLORD)
		{
			if( isKeyDown( VK_RBUTTON) )
			{
				if(isKeyDown(VK_LBUTTON))
				{
					POINT mousePos;
					GetCursorPos( &mousePos );

					Vector3f diff( m_target - m_position);
					Vector2f foward(diff.x, diff.z);
					foward.normalize();
					Vector2f right(foward.y, -foward.x);
					float mouseDeltaX( mousePos.x - (float)m_mousePos.x );
					float mouseDeltaY( mousePos.y - (float)m_mousePos.y );


					m_target.x = clamp(m_target.x + foward.x * mouseDeltaY * MOUSE_PITCH_POWER + right.x * mouseDeltaX * MOUSE_YAW_POWER, -400.f, 400.f);
					m_target.z = clamp(m_target.z + foward.y * mouseDeltaY * MOUSE_PITCH_POWER + right.y * mouseDeltaX * MOUSE_YAW_POWER, -400.f, 400.f);

					SetCursorPos( m_mousePos.x, m_mousePos.y );
				}
				else
				{
					POINT mousePos;
					GetCursorPos( &mousePos );

					float mouseDeltaX( mousePos.x - (float)m_mousePos.x );
					float mouseDeltaY( mousePos.y - (float)m_mousePos.y );

					// Update camera yaw.
					//
					impulse.y = -mouseDeltaX * MOUSE_YAW_POWER;
					impulse.x = -mouseDeltaY * MOUSE_PITCH_POWER;

					SetCursorPos( m_mousePos.x, m_mousePos.y );
				}
			}
		}
	}

	void Camera::setMode(NamedPropertyContainer& parameters)
	{
		std::string mode_s;
		int mode;
		parameters.getNamedData(HashedString("Mode"), mode_s);
		std::stringstream ss;
		ss << mode_s;
		ss >> mode;


		switch(mode)
		{
		case 1:
			m_mode = CAMERAMODE_FREE;
			break;
		default:
			m_mode = CAMERAMODE_OVERLORD;
		}
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------

	void Camera::catchCamInfo(NamedPropertyContainer& parameters)
	{
		DevConsole* console;
		parameters.getNamedData(HASHED_STRING_Console, console);

		std::stringstream ss;
		ss << "Camera Rotation Angles ( " << m_rotationAngles.x << ", " << m_rotationAngles.y << ", " << m_rotationAngles.z << ") ";

		console->postMessage(MESSAGE_TYPE_INFO, ss.str());
		
		ss.clear();
		ss.str("");

		ss<< "Position ( " << m_position.x << ", " << m_position.y << ", " << m_position.z << ") ";
		console->postMessage(MESSAGE_TYPE_INFO, ss.str());

		ss.clear();
		ss.str("");

		ss<< "Target ( " << m_target.x << ", " << m_target.y << ", " << m_target.z << ") ";
		console->postMessage(MESSAGE_TYPE_INFO, ss.str());
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------

	void Camera::catchMouseWheel( NamedPropertyContainer& parameters )
	{
			unsigned int wParam;
			parameters.getNamedData(HashedString("Direction"), wParam);
			if( (  GET_WHEEL_DELTA_WPARAM(wParam) ) > 0)
			{
				m_length -= 10;
			}
			else
			{
				m_length += 10;
			}
			m_length = clamp(m_length, 10.0f, 400.0f);
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------

	void Camera::catchRMouseDown(NamedPropertyContainer& parameters)
	{
		POINT mousepos;
		GetCursorPos( &mousepos);
		m_mousePos.x = mousepos.x;
		m_mousePos.y = mousepos.y;
	}
}