#pragma once

namespace Editor
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_Name(name) {}
		virtual ~Layer() = default;
		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float deltaTime) {} //not sure if this is needed. this is more for game logic

		virtual void OnUIRender() {}

		std::string GetName() const { return m_Name; }
	private:
		std::string m_Name;
	};
} // namespace Editor