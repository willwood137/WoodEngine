#pragma once

#include "..\math\Vector4.hpp"
#include "..\Engine\Font.hpp"

#include <string>
#include <memory>

namespace woodman
{

	struct UIStyle
	{
		float TitleSize;
		RGBA TitleColor;
		float TitleBuffer;

		float subTitleSize;
		RGBA subTitleColor;
		float subTitleBuffer;

		float NodeBoxCornerSize;
		float NodeBoxBorderLength;

		float LineWidth;
		float NodeLineBuffer;

		float MouseMenuTextSize;
		float MouseMenuHeight;
		float MouseMenuWidth;
		float MouseMenuLineWidth;
		RGBA MouseMenuBackgroundColor;
		RGBA MouseMenuLineColor;
		RGBA MouseMenuTextColor;
		std::shared_ptr<Font> MouseMenuFont;


		std::shared_ptr<Font> TitleTextFont;
		std::shared_ptr<Font> subTitleTextFont;

		static std::shared_ptr<UIStyle> DefaultUIStyle;
		static void LoadDefaultUIStyle(const std::string& filePath);
	};

}