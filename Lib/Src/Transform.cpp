#include <Tkge/Transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Tkge
{
	namespace
	{
		struct Trs
		{
			glm::mat4 t;
			glm::mat4 r;
			glm::mat4 s;

			explicit Trs(const glm::vec2 position, const float orientation, const glm::vec2 scale)
				: t(glm::translate(Transform::IdentityMat, glm::vec3{position, 0.0f})),
				  r(glm::rotate(Transform::IdentityMat, glm::radians(orientation), glm::vec3{0.0f, 0.0f, 1.0f})),
				  s(glm::scale(Transform::IdentityMat, glm::vec3{scale, 1.0f}))
			{
			}
		};
	} // namespace

	auto Transform::ToModel() const -> glm::mat4
	{
		const auto trs = Trs{position, orientation, scale};
		return trs.t * trs.r * trs.s;
	}

	auto Transform::ToView() const -> glm::mat4
	{
		const auto trs = Trs{-position, -orientation, scale};
		return trs.s * trs.r * trs.t;
	}

	auto Transform::ToInverseView() const -> glm::mat4
	{
		if (scale == glm::vec2{0.0f}) { return {}; }
		const auto trs = Trs{position, orientation, 1.0f / scale};
		return trs.t * trs.r * trs.s;
	}
} // namespace Tkge
