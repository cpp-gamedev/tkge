#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace tkge::graphics
{
	struct Vertex
	{
		glm::vec2 position{};
		glm::vec4 colour{1.0f};
		glm::vec2 uv{};
	};
} // namespace tkge::graphics
