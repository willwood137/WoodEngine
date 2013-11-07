#pragma once

#include "UIWidget.hpp"
#include "UICanvas.hpp"
#include "UIVectorMap.hpp"

#include "..\Math\AABB2D.hpp"
#include "..\Math\Vector2.hpp"

#include "..\Engine\Texture.hpp"

#include <vector>

namespace woodman
{
	class UINodeLink;


	class UILinkStrip : public UIWidget
	{
	public:

		UILinkStrip(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			HashedString uniqueID,
			float RelativeLayer,
			PropertyType pType,
			unsigned int typeSize,
			CanvasCoordinates startPoint,
			CanvasCoordinates endPoint);

		UILinkStrip(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			HashedString uniqueID,
			float RelativeLayer,
			UINodeLink* OutLink,
			UINodeLink* InLink );

		bool isPointInBounds(const Vector2f& point);

		//Inherited Functions
		virtual void Initialize();
		virtual void render(UIMouse* currentMouse, float ParentLayer);
		virtual void update(UIMouse* currentMouse);

		//calculates the line strip based on the start/end points/vectors
		void calcControlPoints();

		void updateVectorMap();

#pragma region UILinkStrip_Getters

		UINodeLink* getStartTarget();
		UINodeLink* getEndTarget();
		bool isGoodStrip();

#pragma endregion
#pragma region UILinkStrip_Setters

		void updateStartPoint(const CanvasCoordinates& StartPoint);
		void updateEndPoint(const CanvasCoordinates& EndPoint);
		void updateStartTarget( UINodeLink* StartTarget);
		void updateEndTarget( UINodeLink* EndTarget);

#pragma endregion


	private:

		void renderCrossCanvas(UIMouse* currentMouse, float layer );
		void renderIntraCanvas(UIMouse* currentMouse, float layer );


		CanvasCoordinates m_startPoint;
		CanvasCoordinates m_endPoint;
		Vector2f m_startVector;
		Vector2f m_endVector;

		PropertyType m_propertyType;
		unsigned int m_typeSize;

		UINodeLink* m_startTarget;
		unsigned int m_startSize;
		UINodeLink* m_endTarget;
		unsigned int m_endSize;

		std::vector< CanvasCoordinates > m_controlPoints;
		UIVectorMap* m_vectorMap;

		std::shared_ptr<Shader> m_lineStripShader;
		std::shared_ptr<Shader> m_crossCanvasLineStripShader;
		std::shared_ptr<Texture> m_filterTexture;

	};
}