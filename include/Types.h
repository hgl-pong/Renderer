#pragma once

union Color
{
	struct
	{
		float r, g, b, a;
	};
	float data[4];
};
