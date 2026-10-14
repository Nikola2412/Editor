#import <Cocoa/Cocoa.h>

extern "C" bool EditorStartCocoaWindowMove(void* nativeWindow)
{
	NSWindow* window = reinterpret_cast<NSWindow*>(nativeWindow);
	if (!window)
		return false;

	[window setMovable:YES];
	const NSPoint mouseLocation = [NSEvent mouseLocation];
	const NSPoint windowLocation = [window convertPointFromScreen:mouseLocation];
	NSEvent* mouseDown = [NSEvent mouseEventWithType:NSEventTypeLeftMouseDown
		location:windowLocation
		modifierFlags:NSEventModifierFlagNone
		timestamp:NSProcessInfo.processInfo.systemUptime
		windowNumber:window.windowNumber
		context:nil
		eventNumber:0
		clickCount:1
		pressure:1.0];
	if (!mouseDown)
		return false;

	[window performWindowDragWithEvent:mouseDown];
	return true;
}
