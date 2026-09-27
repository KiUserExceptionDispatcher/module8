#pragma once
#include "imgui.h"

class c_colors
{
public:
	ImColor accent{ 154, 127, 172 };
	ImColor accent_two{ 103, 76, 121 };
	ImColor glow{ 154, 127, 172 };

	struct
	{
		ImColor background_one{ 36, 36, 47 };
		ImColor background_two{ 42, 42, 56 };
		ImColor stroke{ 53, 53, 67 };
	} window;

	struct
	{
		ImColor stroke_two{ 36, 36, 48 };
		ImColor text{ 200, 200, 200 };
		ImColor text_inactive{ 136, 136, 136 };
		ImColor text_warning{ 255, 165, 50 };
		ImColor text_outline{ 0, 0, 0 };
		ImColor text_outline_two{ 0, 0, 0 };
	} widgets;
};

inline c_colors* clr = new c_colors();
