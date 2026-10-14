#import <Cocoa/Cocoa.h>

extern "C" void EditorRenderNativeMoveFrame();

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

	NSTimer* renderTimer = [NSTimer timerWithTimeInterval:1.0 / 60.0
		repeats:YES
		block:^(NSTimer*)
		{
			EditorRenderNativeMoveFrame();
		}];
	NSRunLoop* runLoop = [NSRunLoop mainRunLoop];
	[runLoop addTimer:renderTimer forMode:NSRunLoopCommonModes];
	[runLoop addTimer:renderTimer forMode:NSEventTrackingRunLoopMode];
	[window performWindowDragWithEvent:mouseDown];
	[renderTimer invalidate];
	return true;
}
