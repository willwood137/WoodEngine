#pragma once
#include "..\Utility\Utility.hpp"
#include "..\Math\Matrix4.hpp"
#include "EventSystem.hpp"
#include "Clock.hpp"


namespace woodman
{
	
	class Camera
	{
	public:

		enum CameraMode
		{
			Spectator = 0,
			FirstPerson,
			ThirdPerson,
			ThirdPersonChase,
			Last,
		};

		explicit Camera();
		Camera(Vector3f& Origin);
		~Camera();

		void update( float delta );


		const Vector3f& getPosition() const { return m_position; }
		void setPosition( const Vector3f& position ) {m_position = position;}
		const Vector3f& getRotation() const { return m_rotationAngles; }
		void setRotation( const Vector3f& rotation ) {m_rotationAngles = rotation; }
		void lockMouse();
		void unlockMouse();
		void setOrigin( Vector3f& Origin) { m_origin = Origin;}
		const Vector3f& getOrigin( ) const { return m_origin;}
		Matrix4f& getMatrix() { return m_matrix;}




	protected:

		void updateInput(Vector3f& impulse, Vector3f& rotation);


	private:

		Vector3f m_rotationAngles;
		Vector3f m_position;
		Vector3f m_velocity;
		Vector3f m_origin;

		Matrix4f m_matrix;


		float m_distance;

		CameraMode m_mode;
	};

}