#pragma once

#include "UIWidget.hpp"
#include "UICanvas.hpp"

#include "..\Math\AABB2D.hpp"
#include "..\Math\Vector2.hpp"

#include "..\Engine\Texture.hpp"

#include <vector>

namespace woodman
{
	class UILinkStrip : public UIWidget
	{
	public:

		static std::weak_ptr<UILinkStrip> CreateUILinkStrip( const Vector2f& startPoint,
			const Vector2f& endPoint,
			UIController* _ParentController,
			std::weak_ptr<UICanvas> _ParentCanvas,
			std::weak_ptr<UIWidget> _ParentWidget,
			const HashedString& _ID,
			float _RelativeLayer,
			const Vector2f& _RelativeCoordinates,
			const Vector2f& _CollisionSize );

		virtual ~UILinkStrip();

		bool isPointInBounds(const Vector2f& point);

		//Inherited Functions
		virtual void Initialize();
		virtual void render(std::shared_ptr<UIMouse> currentMouse);
		virtual void update(std::shared_ptr<UIMouse> currentMouse);
		
		//calculates the line strip based on the start/end points/vectors
		void calcControlPoints();

#pragma region UILinkStrip_Getters

		std::shared_ptr<UIWidget> getStartTarget();
		std::shared_ptr<UIWidget> getEndTarget();
		bool isGoodStrip();

#pragma endregion
#pragma region UILinkStrip_Setters

		void updateStartPoint(const Vector2f& StartPoint);
		void updateEndPoint(const Vector2f& EndPoint);
		void updateStartTarget( std::shared_ptr<UIWidget> StartTarget);
		void updateEndTarget( std::shared_ptr<UIWidget> EndTarget);

#pragma endregion


	private:

		UILinkStrip(const Vector2f& startPoint,
			const Vector2f& endPoint,
			UIController* _ParentController,
			std::weak_ptr<UICanvas> _ParentCanvas,
			std::weak_ptr<UIWidget> _ParentWidget,
			const HashedString& _ID,
			float _RelativeLayer,
			const Vector2f& _RelativeCoordinates,
			const Vector2f& _CollisionSize );

		void renderCrossCanvas(std::shared_ptr<UIMouse> currentMouse, float layer );
		void renderIntraCanvas(std::shared_ptr<UIMouse> currentMouse, float layer );


		Vector2f m_startPoint;
		Vector2f m_endPoint;
		Vector2f m_startVector;
		Vector2f m_endVector;

		PropertyType m_propertyType;
		unsigned int m_typeSize;

		//this is true if we are updateing the end point
		std::weak_ptr<UIWidget> m_startTarget;
		std::weak_ptr<UIWidget> m_endTarget;

		std::vector< Vector2f > m_controlPoints;

		std::shared_ptr<Shader> m_lineStripShader;
		std::shared_ptr<Shader> m_crossCanvasLineStripShader;
		std::shared_ptr<Texture> m_filterTexture;

	};
}