# Editor
Piranha Engine

## CLONE
    git clone --recurse https://github.com/Nikola2412/Editor.git

## BUGS / WARNINGS
### BUG 1
In imgui.cpp `window` pointer should be added like this

 	for (int i = 0; window && i < window->DC.Layouts.Data.Size; i++)
	{
    	ImGuiLayout* layout = (ImGuiLayout*)window->DC.Layouts.Data[i].val_p;
    	IM_DELETE(layout);
	}

### WARRNING (still compiles)
In imgui.h comment/remove defiences for IMGUI_VERSION and IMGUI_VERSION_NUM there are overriden
