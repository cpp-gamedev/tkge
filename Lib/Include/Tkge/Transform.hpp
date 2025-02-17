#pragma once
#include <glm/mat4x4.hpp>

namespace Tkge
{
	struct Transform
	{
		static constexpr auto IdentityMat = glm::mat4{1.0f};

		[[nodiscard]] auto ToModel() const -> glm::mat4;
		[[nodiscard]] auto ToView() const -> glm::mat4;
		[[nodiscard]] auto ToInverseView() const -> glm::mat4;

		[[nodiscard]] static constexpr Transform Accumulate(const Transform& a, const Transform& b)
		{
			return Transform{
				.position = a.position + b.position,
				.orientation = a.orientation + b.orientation,
				.scale = a.scale * b.scale,
			};
		}

		glm::vec2 position{};
		float orientation{};
		glm::vec2 scale{1.0f};
	};
} // namespace Tkge
