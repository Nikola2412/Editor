#include <Editor.h>


Editor::Application* Editor::CreateApplication(int argc, char** argv) 
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";
	spec.Width = 1600;
	spec.Height = 900;


	Editor::Application* app = new Editor::Application(spec);
	return app;
}

