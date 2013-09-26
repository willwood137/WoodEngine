#pragma once

#include "UIWidget.hpp"

#include "..\Math\AABB2D.hpp"
#include "..\Math\Vector2.hpp"

#include "..\Engine\Texture.hpp"

#include <vector>

namespace woodman
{
	class UICanvas;

	class UIDataStrip : public UIWidget
	{
	public:

		UIDataStrip(std::shared_ptr<UICanvas> ParentCanvas,
			std::shared_ptr<UIWidget> parentWidget,
			const std::string& name,
			HashedString uniqueID);

		bool isPointInBounds(const Vector2f& point);

		virtual void Initialize();
		virtual void render(std::shared_ptr<UIMouse> currentMouse);
		virtual void update(std::shared_ptr<UIMouse> currentMouse);


		//virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseRelease( std::shared_ptr<UIMouse> currentMouse);
		//virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse);

		bool isGoodStrip();
		void setDragEndPoint(bool dragEndPoint);
		void calcControlPoints();
		void updateStartPoint(const Vector2f& StartPoint);
		void updateEndPoint(const Vector2f& EndPoint);
		void updateStartTarget(std::shared_ptr<UIWidget> StartTarget);
		void updateEndTarget(std::shared_ptr<UIWidget> EndTarget);

	private:
		Vector2f m_startPoint;
		Vector2f m_startVector;
		Vector2f m_endPoint;
		Vector2f m_endVector;

		std::shared_ptr<UIWidget> m_startTarget;
		std::shared_ptr<UIWidget> m_endTarget;


		//this is true if we are updateing the end point
		bool m_dragEndPoint;

		std::vector< Vector2f > m_controlPoints;
		std::shared_ptr<Shader> m_lineStripShader;

		std::shared_ptr<Texture> m_filterTexture;



	};
}