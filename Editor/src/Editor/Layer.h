#pragma once


namespace Editor
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_Name(name) {}
		virtual ~Layer() = default;

		void SetUICallBack(const std::function<void()>& UICallback) { m_UICallback = UICallback; }
	protected:
		virtual void onAttach() {};

	private:
		void OnAttach();
		void OnDetach();

		void Begin();
		void End();

		virtual void OnUpdate(float deltaTime = 0); //not sure if this is needed. this is more for game logic

		void dockSpace(); // maybe make virtual
		void UICallBackRender();

	protected:
		virtual void OnUIRender() {}

		std::string GetName() const { return m_Name; }
	private:
		std::function<void()> m_UICallback;
		bool m_dockSpace = false;
		std::string m_Name;


		friend class Application;
	};
} // namespace Editor