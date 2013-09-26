#pragma once
#include "..\Utility\Utility.hpp"
#include "..\Math\Matrix4.hpp"
#include "EventSystem.hpp"
#include "Clock.hpp"


namespace woodman
{
	
	class Camera : public EventRecipient<Camera>
	{
	public:

		enum CameraMode
		{
			CAMERAMODE_OVERLORD,
			CAMERAMODE_FREE,
			CAMERAMODE_SIZE,
		};

		Camera(const Vector3f& position, Vector3f& target, EventSystem* eventsystem, Clock* parentClock);
		
		void initialize();
		void update( );


		const Vector3f& getPosition() const { return m_position; }
		void setPosition( const Vector3f& position ) {m_position = position;}
		const Vector3f& getRotation() const { return m_rotationAngles; }
		void setRotation( const Vector3f& rotation ) {m_rotationAngles = rotation; }
		void lockMouse();
		void unlockMouse();
		void setTarget( const Vector3f& Target) { m_target = Target;}
		Vector3f& getTarget() { return m_target;}
		Matrix4f& getMatrix() { return m_matrix;}
		void setMode(NamedPropertyContainer& parameters);
		void catchCamInfo(NamedPropertyContainer& parameters);
		void catchRMouseDown(NamedPropertyContainer& parameters);
		void catchMouseWheel(NamedPropertyContainer& parameters);

		
		
		
		
	protected:

		void updateInput(Vector3f& impulse, Vector3f& rotation);
		

	private:

		Camera();
		Vector3f m_rotationAngles;
		Vector3f m_position;
		Vector3f m_target;
		Vector3f m_velocity;
		float m_length;

		Matrix4f m_matrix;
		float m_distance;

		Vector2i m_mousePos;
		Clock m_clock;

		CameraMode m_mode;
	};

}