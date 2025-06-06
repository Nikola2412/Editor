#include <Editor.h>


Editor::Application* Editor::CreateApplication() 
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";


	Editor::Application* app = new Editor::Application(spec);
	app->SetUICallBack([app] ()
		{
			
		});
	return app;
}

