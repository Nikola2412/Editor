#pragma once

#include <string>
#include <iostream>

int main(int argc, char* argv[]);

namespace Editor
{
	struct ApplicationSpecification
	{
		std::string Name = "App";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
		~Application() = default;

		void Run() 
		{
			while (1)
				std::cout << "Running application" << std::endl;
		}

	};

	Application* CreateApplication(int argc, char** argv);
}