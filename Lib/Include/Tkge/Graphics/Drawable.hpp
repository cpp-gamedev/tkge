#pragma once
#include <Tkge/Graphics/Renderer.hpp>
#include <Tkge/Graphics/Shape.hpp>
#include <concepts>

namespace Tkge::Graphics
{
	class IDrawable : public klib::Polymorphic
	{
	  public:
		[[nodiscard]] virtual Primitive GetPrimitive() const = 0;
		[[nodiscard]] virtual std::span<const RenderInstance> GetInstances() const = 0;

		virtual void Draw(Renderer& renderer) const { renderer.Draw(GetPrimitive(), GetInstances()); }
	};

	template <std::derived_from<IGeometry> TGeometry>
	class BasicDrawable : public TGeometry, public IDrawable
	{
	  public:
		[[nodiscard]] Primitive GetPrimitive() const final
		{
			return Primitive{
				.vertices = this->GetVertices(),
				.indices = this->GetIndices(),
				.topology = this->GetTopology(),
				.texture = texture,
			};
		}

		const Texture* texture{nullptr};
	};

	template <std::derived_from<IGeometry> TGeometry>
	class Drawable : public BasicDrawable<TGeometry>, public RenderInstance
	{
	  public:
		[[nodiscard]] std::span<const RenderInstance> GetInstances() const final { return {static_cast<const RenderInstance*>(this), 1}; }
	};

	template <std::derived_from<IGeometry> TGeometry>
	class InstancedDrawable : public BasicDrawable<TGeometry>
	{
	  public:
		[[nodiscard]] std::span<const RenderInstance> GetInstances() const final { return instances; }

		std::vector<RenderInstance> instances{};
	};

	using Quad = Drawable<QuadShape>;
	using InstancedQuad = InstancedDrawable<QuadShape>;

} // namespace Tkge::Graphics
