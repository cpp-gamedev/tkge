#pragma once
#include <Tkge/Transform.hpp>
#include <kvf/color.hpp>

namespace Tkge::Graphics
{
	struct RenderInstance
	{
		Transform transform{};
		kvf::Color tint{kvf::white_v};
	};
} // namespace Tkge::Graphics
