#include "api_macos.h"
#include <iostream>
#include <algorithm>
#include <dispatch/dispatch.h>
#include <CoreGraphics/CoreGraphics.h>

//! START IMPLEMENTATION

// Application consts selectors
static constexpr const char* kRespondsToSelector                = "respondsToSelector:";
static constexpr const char* kNSApplicationClass                = "NSApplication";
static constexpr const char* kNSWindowClass                     = "NSWindow";
static constexpr const char* kNSStringClass                     = "NSString";
static constexpr const char* kNSOpenGLPixelFormatClass          = "NSOpenGLPixelFormat";
static constexpr const char* kNSOpenGLViewClass                 = "NSOpenGLView";
static constexpr const char* kNSObjectClass                     = "NSObject";
static constexpr const char* kAppDelegateClass                  = "AppDelegate";
static constexpr const char* kWindowDelegateClass               = "WindowDelegate";
static constexpr const char* kCustomOpenGLViewClass             = "CustomOpenGLView";
static constexpr const char* kGeneralWindowDelegateClass        = "GeneralWindowDelegate";

// Application Screen management selectors
//static constexpr const char* kNSScreenClass                     = "NSScreen"; // Temp remove unused variable warning
static constexpr const char* kScreenSel                         = "screen";
static constexpr const char* kNSCursorClass                     = "NSCursor";
static constexpr const char* kUnhideSel                         = "unhide";
static constexpr const char* kHideSel                           = "hide";
static constexpr const char* kIsHiddenSel                       = "isHidden";


// Application memory management selectors
static constexpr const char* kAllocSel                          = "alloc";
static constexpr const char* kInitSel                           = "init";
static constexpr const char* kSetDelegateSel                    = "setDelegate:";
static constexpr const char* kReleaseSel                        = "release";
static constexpr const char* kIsKindOfClassSel                  = "isKindOfClass:";

// NSApplication lifecycle and management selectors
static constexpr const char* kSharedApplicationSel              = "sharedApplication";
static constexpr const char* kActivateIgnoringOtherAppsSel      = "activateIgnoringOtherApps:";
static constexpr const char* kSetActivationPolicySel            = "setActivationPolicy:";
static constexpr const char* kRunSel                            = "run";
static constexpr const char* kStopSel                           = "stop:";
static constexpr const char* kTerminateSel                      = "terminate:";

// NSApplicationDelegate lifecycle methods
static constexpr const char* kApplicationWillFinishLaunchingSel = "applicationWillFinishLaunching:";
static constexpr const char* kApplicationDidFinishLaunchingSel  = "applicationDidFinishLaunching:";
static constexpr const char* kApplicationWillTerminateSel       = "applicationWillTerminate:";
static constexpr const char* kApplicationDidBecomeActiveSel     = "applicationDidBecomeActive:";
static constexpr const char* kApplicationWillResignActiveSel    = "applicationWillResignActive:";

// NSWindow creation, display, and management selectors
static constexpr const char* kInitWithContentRectSel            = "initWithContentRect:styleMask:backing:defer:";
static constexpr const char* kStringWithUTF8StringSel           = "stringWithUTF8String:";
static constexpr const char* kSetTitleSel                       = "setTitle:";
static constexpr const char* kOrderFrontRegardlessSel           = "orderFrontRegardless";
static constexpr const char* kSetAcceptsMouseMovedEventsSel     = "setAcceptsMouseMovedEvents:";
static constexpr const char* kMakeFirstResponderSel             = "makeFirstResponder:";
static constexpr const char* kMakeKeyAndOrderFrontSel           = "makeKeyAndOrderFront:";
static constexpr const char* kMakeKeyWindowSel                  = "makeKeyWindow";
static constexpr const char* kFrameSel                          = "frame";
static constexpr const char* kSetFrameDisplaySel                = "setFrame:display:";
static constexpr const char* kSetFrameSel                       = "setFrame:";
static constexpr const char* kStyleMaskSel                      = "styleMask";
static constexpr const char* kToggleFullScreenSel               = "toggleFullScreen:";

// NSWindowDelegate lifecycle and event methods selectors
static constexpr const char* kWindowDidResizeSel                = "windowDidResize:";
static constexpr const char* kWindowWillCloseSel                = "windowWillClose:";
static constexpr const char* kWindowDidBecomeKeySel             = "windowDidBecomeKey:";
static constexpr const char* kWindowDidResignKeySel             = "windowDidResignKey:";
static constexpr const char* kWindowDidMiniaturizeSel           = "windowDidMiniaturize:";
static constexpr const char* kWindowDidDeminiaturizeSel         = "windowDidDeminiaturize:";

// NSResponder keyboard and mouse event methods selectors
static constexpr const char* kKeyDownSel                        = "keyDown:";
static constexpr const char* kKeyUpSel                          = "keyUp:";
static constexpr const char* kFlagsChangedSel                   = "flagsChanged:";
static constexpr const char* kMouseDownSel                      = "mouseDown:";
static constexpr const char* kMouseUpSel                        = "mouseUp:";
static constexpr const char* kMouseDraggedSel                   = "mouseDragged:";
static constexpr const char* kMouseMovedSel                     = "mouseMoved:";
static constexpr const char* kRightMouseDownSel                 = "rightMouseDown:";
static constexpr const char* kRightMouseUpSel                   = "rightMouseUp:";
static constexpr const char* kRightMouseDraggedSel              = "rightMouseDragged:";
static constexpr const char* kOtherMouseDownSel                 = "otherMouseDown:";
static constexpr const char* kOtherMouseUpSel                   = "otherMouseUp:";
static constexpr const char* kOtherMouseDraggedSel              = "otherMouseDragged:";
static constexpr const char* kScrollWheelSel                    = "scrollWheel:";
static constexpr const char* kDeltaXSel                         = "deltaX";
static constexpr const char* kDeltaYSel                         = "deltaY";
static constexpr const char* kUpdateTrackingAreasSel            = "updateTrackingAreas";
static constexpr const char* kMouseEnteredSel                   = "mouseEntered:";
static constexpr const char* kMouseExitedSel                    = "mouseExited:";
static constexpr const char* kTrackingAreaClass                 = "NSTrackingArea";
static constexpr const char* kAddTrackingAreaSel                = "addTrackingArea:";
static constexpr const char* kInitWithRectSel                   = "initWithRect:options:owner:userInfo:";

// Copied a lot of this code from iOS, will need to be tested
// NSResponder touch event method selectors
static constexpr const char* kSetAcceptsTouchEventsSel          = "setAcceptsTouchEvents:";
static constexpr const char* kSetWantsRestingTouchesSel         = "setWantsRestingTouches:";
static constexpr const char* kCGEventSel                        = "CGEvent";
static constexpr const char* kTouchesBeganSel                   = "touchesBeganWithEvent:";
static constexpr const char* kTouchesMovedSel                   = "touchesMovedWithEvent:";
static constexpr const char* kTouchesEndedSel                   = "touchesEndedWithEvent:";
static constexpr const char* kTouchesCancelledSel               = "touchesCancelledWithEvent:";

// NSTouch / NSSet data extraction selectors
static constexpr const char* kTouchesMatchingPhaseSel           = "touchesMatchingPhase:inView:";
static constexpr const char* kAllObjectsSel                     = "allObjects";
static constexpr const char* kNormalizedPositionSel             = "normalizedPosition";
static constexpr const char* kDeviceSizeSel                     = "deviceSize";
static constexpr const char* kIdentitySel                       = "identity";
static constexpr const char* kObjectAtIndexSel                  = "objectAtIndex:";
static constexpr const char* kTouchCountSel                     = "count";

// NSResponder tablet / stylus event method selectors
static constexpr const char* kTabletPointSel                    = "tabletPoint:";
static constexpr const char* kTabletProximitySel                = "tabletProximity:";
static constexpr const char* kPressureChangeSel                 = "pressureChangeWithEvent:";
static constexpr const char* kStageSel                          = "stage";

// NSEvent tablet data extraction selectors
static constexpr const char* kPenPressureSel                    = "pressure";
static constexpr const char* kPenRotationSel                    = "rotation";
static constexpr const char* kPenTiltSel                        = "tilt";
static constexpr const char* kPenIsEnteringProximitySel         = "isEnteringProximity";
static constexpr const char* kPointingDeviceTypeSel             = "pointingDeviceType";
static constexpr const char* kPenTangentialPressureSel          = "tangentialPressure";


// Managing first responder status and keyboard focus selectors
static constexpr const char* kAcceptsFirstResponderSel          = "acceptsFirstResponder";
static constexpr const char* kBecomeFirstResponderSel           = "becomeFirstResponder";
static constexpr const char* kCanBecomeKeyViewSel               = "canBecomeKeyView";
static constexpr const char* kNeedsPanelToBecomeKeySel          = "needsPanelToBecomeKey";
static constexpr const char* kDrawRectSel                       = "drawRect:";
static constexpr const char* kReshapeSel                        = "reshape";
static constexpr const char* kUpdateSel                         = "update";

// NSOpenGL pixel format, view, and context management selectors
static constexpr const char* kInitWithAttributesSel             = "initWithAttributes:";
static constexpr const char* kInitWithFramePixelFormatSel       = "initWithFrame:pixelFormat:";
static constexpr const char* kSetContentViewSel                 = "setContentView:";
static constexpr const char* kContentViewSel                    = "contentView";
static constexpr const char* kBoundsSel                         = "bounds";
static constexpr const char* kConvertRectToBackingSel           = "convertRectToBacking:";      // Thank you - Ben the Ultimate Guru
static constexpr const char* kConvertRectFromBackingSel         = "convertRectFromBacking:";    // Thank you - Mr B the Ultimate Teacher!!!
static constexpr const char* kConvertPointToBackingSel          = "convertPointToBacking:";
static constexpr const char* kConvertPointFromBackingSel        = "convertPointFromBacking:";
static constexpr const char* kConvertPointFromViewSel           = "convertPoint:fromView:";
static constexpr const char* kBackingScaleFactorSel             = "backingScaleFactor";
static constexpr const char* kOpenGLContextSel                  = "openGLContext";
static constexpr const char* kMakeCurrentContextSel             = "makeCurrentContext";
static constexpr const char* kSetAutoresizingMaskSel            = "setAutoresizingMask:";
static constexpr const char* kFlushBufferSel                    = "flushBuffer";
static constexpr const char* kDisplaySel                        = "display";
static constexpr const char* kCGLContextObjSel                  = "CGLContextObj";
static constexpr const char* kSetValuesSel                      = "setValues:forParameter:";

// Extracting data from NSEvent objects selectors
static constexpr const char* kKeyCodeSel                        = "keyCode";
static constexpr const char* kCharactersSel                     = "characters";
static constexpr const char* kLocationInWindowSel               = "locationInWindow";
static constexpr const char* kButtonNumberSel                   = "buttonNumber";
static constexpr const char* kClickCountSel                     = "clickCount";
static constexpr const char* kModifierFlagsSel                  = "modifierFlags";
static constexpr const char* kUTF8StringSel                     = "UTF8String";

// NSLocale class and method names
static constexpr const char* kNSLocaleClass                     = "NSLocale";
static constexpr const char* kCurrentLocaleSel                  = "currentLocale";
static constexpr const char* kLocaleIdentifierSel               = "localeIdentifier";


// Default values and configuration settings
static constexpr const char* kWindowTitle                       = "C macOS OpenGL Framework";
static constexpr double kDefaultWindowWidth                     = 800.0;
static constexpr double kDefaultWindowHeight                    = 600.0;
// static constexpr int kBitsPerByte                               = 8; // Temp remove unused variable warning
static constexpr int kMinValidDimension                         = 0;
static constexpr int kRGBABytesPerPixel                         = 4;
static constexpr int kFullyOpaque                               = 255;
static constexpr int kZeroBytes                                 = 0;
static constexpr int kZeroRows                                  = 0;
static constexpr int kZeroWidth                                 = 0;
static constexpr int kZeroHeight                                = 0;
static constexpr int kFlippedOffset                             = 1;
static constexpr int kNoButton                                  = -1;
// static constexpr int kDefaultScreenNumber                       = 1;  // Temp remove unused variable warning
bool bAllowHideCursor                                           = true;
bool bHideCursor                                                = false;

// Objective-C method type encoding constants
// Type encoding for methods returning BOOL with no parameters: "c@:"
static constexpr const char* kBoolMethodTypeEncoding = "c@:";

// Type encoding for void methods with event/notification parameter: "v@:@"
static constexpr const char* kEventHandlerMethodTypeEncoding = "v@:@";

// Type encoding for drawRect method with NSRect parameter: "v@:{NSRect={NSPoint=dd}{NSSize=dd}}"
static constexpr const char* kDrawRectMethodTypeEncoding = "v@:{NSRect={NSPoint=dd}{NSSize=dd}}";

// Type encoding for void methods with no parameters: "v@:"
static constexpr const char* kVoidMethodTypeEncoding = "v@:";

// Type encoding for touch event methods: two id params (touches set + event) → "v@:@"
static constexpr const char* kTouchEventMethodTypeEncoding = "v@:@";

namespace ObjectiveCSEL {
     
// Application memory management selectors
   static SEL allocSel         = nullptr;
   static SEL initSel          = nullptr;
   static SEL setDelegateSel   = nullptr;
   static SEL releaseSel       = nullptr;
   static SEL isKindOfClassSel = nullptr;

   // NSApplication lifecycle and management selectors
   static SEL sharedApplicationSel         = nullptr;
   static SEL activateIgnoringOtherAppsSel = nullptr;
   static SEL setActivationPolicySel       = nullptr;
   static SEL runSel                       = nullptr;
   static SEL stopSel                      = nullptr;
   static SEL terminateSEL                 = nullptr;

   // Application Screen management selectors
   static SEL screenSel                    = nullptr;
   static SEL unhideSel                    = nullptr;
   static SEL hideSel                      = nullptr;
   static SEL isHiddenSel                  = nullptr;

   // NSApplicationDelegate lifecycle methods
   static SEL applicationWillFinishLaunchingSel = nullptr;
   static SEL applicationDidFinishLaunchingSel  = nullptr;
   static SEL applicationWillTerminateSel       = nullptr;
   static SEL applicationDidBecomeActiveSel     = nullptr;
   static SEL applicationWillResignActiveSel    = nullptr;

   // NSWindow creation, display, and management selectors
   static SEL initWithContentRectSel        = nullptr;
   static SEL stringWithUTF8StringSel       = nullptr;
   static SEL setTitleSel                   = nullptr;
   static SEL orderFrontRegardlessSel       = nullptr;
   static SEL setAcceptsMouseMovedEventsSel = nullptr;
   static SEL makeKeyAndOrderFrontSel       = nullptr;
   static SEL makeKeyWindowSel              = nullptr;
   static SEL frameSel                      = nullptr;
   static SEL setFrameDisplaySel            = nullptr;
   static SEL setFrameSel                   = nullptr;
   static SEL makeFirstResponderSel         = nullptr;
   static SEL styleMaskSel                  = nullptr;
   static SEL toggleFullScreenSel           = nullptr;

   // NSWindowDelegate lifecycle and event methods selectors
   static SEL windowDidResizeSel        = nullptr;
   static SEL windowWillCloseSel        = nullptr;
   static SEL windowDidBecomeKeySel     = nullptr;
   static SEL windowDidResignKeySel     = nullptr;
   static SEL windowDidMiniaturizeSel   = nullptr;
   static SEL windowDidDeminiaturizeSel = nullptr;

   // NSResponder keyboard and mouse event methods selectors
   static SEL keyDownSel                = nullptr;
   static SEL keyUpSel                  = nullptr;
   static SEL flagsChangedSel           = nullptr;
   static SEL mouseDownSel              = nullptr;
   static SEL mouseUpSel                = nullptr;
   static SEL mouseDraggedSel           = nullptr;
   static SEL mouseMovedSel             = nullptr;
   static SEL rightMouseDownSel         = nullptr;
   static SEL rightMouseUpSel           = nullptr;
   static SEL rightMouseDraggedSel      = nullptr;
   static SEL otherMouseDownSel         = nullptr;
   static SEL otherMouseUpSel           = nullptr;
   static SEL otherMouseDraggedSel      = nullptr;
   static SEL scrollWheelSel            = nullptr;
   static SEL deltaXSel                 = nullptr;
   static SEL deltaYSel                 = nullptr;
   static SEL updateTrackingAreasSel    = nullptr;
   static SEL mouseEnteredSel           = nullptr;
   static SEL mouseExitedSel            = nullptr;

   // Mouse Tracking
   static SEL addTrackingAreaSel        = nullptr;
   static SEL initWithRectSel           = nullptr;

    // Touch event selectors
   static SEL setAcceptsTouchEventsSel  = nullptr;
   static SEL setWantsRestingTouchesSel = nullptr;
   static SEL cgEventSel                = nullptr;
   static SEL touchesBeganSel           = nullptr;
   static SEL touchesMovedSel           = nullptr;
   static SEL touchesEndedSel           = nullptr;
   static SEL touchesCancelledSel       = nullptr;
   static SEL touchesMatchingPhaseSel   = nullptr;
   static SEL allObjectsSel             = nullptr;
   static SEL normalizedPositionSel     = nullptr;
   static SEL deviceSizeSel             = nullptr;
   static SEL identitySel               = nullptr;
   static SEL objectAtIndexSel          = nullptr;
   static SEL touchesCountSel           = nullptr;

   // Tablet / stylus event selectors
   static SEL tabletPointSel            = nullptr;
   static SEL tabletProximitySel        = nullptr;
   static SEL penPressureSel            = nullptr;
   static SEL penRotationSel            = nullptr;
   static SEL penTiltSel                = nullptr;
   static SEL penIsEnteringProximitySel = nullptr;
   static SEL pointingDeviceTypeSel     = nullptr;
   static SEL penTangentialPressureSel  = nullptr;
   static SEL pressureChangeSel         = nullptr;
   static SEL stageSel                  = nullptr;

   // Managing first responder status and keyboard focus selectors
   static SEL acceptsFirstResponderSel = nullptr;
   static SEL becomeFirstResponderSel  = nullptr;
   static SEL canBecomeKeyViewSel      = nullptr;
   static SEL needsPanelToBecomeKeySel = nullptr;
   static SEL drawRectSel              = nullptr;
   static SEL reshapeSel               = nullptr;
   static SEL updateSel                = nullptr;

   // NSOpenGL pixel format, view, and context management selectors
   static SEL initWithAttributesSel       = nullptr;
   static SEL initWithFramePixelFormatSel = nullptr;
   static SEL setContentViewSel           = nullptr;
   static SEL contentViewSel              = nullptr;
   static SEL boundsSel                   = nullptr;
   static SEL convertRectToBackingSel     = nullptr; // Thank you - Ben the Ultimate Guru
   static SEL convertRectFromBackingSel   = nullptr; // Thank you - Mr B the Ultimate Teacher!!!
   static SEL convertPointToBackingSel    = nullptr;
   static SEL convertPointFromBackingSel  = nullptr;
   static SEL convertPointFromViewSel     = nullptr;
   static SEL openGLContextSel            = nullptr;
   static SEL makeCurrentContextSel       = nullptr;
   static SEL setAutoresizingMaskSel      = nullptr;
   static SEL flushBufferSel              = nullptr;
   static SEL displaySel                  = nullptr;
   static SEL CGLContextObjSel            = nullptr;
   static SEL setValuesSel                = nullptr;
   static SEL backingScaleFactorSel       = nullptr;

   // Extracting data from NSEvent objects selectors
   static SEL keyCodeSel          = nullptr;
   static SEL charactersSel       = nullptr;
   static SEL locationInWindowSel = nullptr;
   static SEL buttonNumberSel     = nullptr;
   static SEL clickCountSel       = nullptr;
   static SEL modifierFlagsSel    = nullptr;
   static SEL utf8StringSel       = nullptr;

   // NSLocale selectors
   static SEL currentLocaleSel               = nullptr;
   static SEL localeIdentifierSel            = nullptr;
   // static SEL currentInputContextSel         = nullptr; // Temp remove unused variable warning
   // static SEL localizedNameSel               = nullptr; // Temp remove unused variable warning

   // Initialize all selectors - called once at startup
    void initializeSelectors() {
        if (allocSel) return; // Already initialized
        
        // Application memory management selectors
        allocSel                            = sel_registerName(kAllocSel);
        initSel                             = sel_registerName(kInitSel);
        setDelegateSel                      = sel_registerName(kSetDelegateSel);
        releaseSel                          = sel_registerName(kReleaseSel);
        isKindOfClassSel                    = sel_registerName(kIsKindOfClassSel);

        // NSApplication lifecycle and management selectors
        sharedApplicationSel                = sel_registerName(kSharedApplicationSel);
        activateIgnoringOtherAppsSel        = sel_registerName(kActivateIgnoringOtherAppsSel);
        setActivationPolicySel              = sel_registerName(kSetActivationPolicySel);
        runSel                              = sel_registerName(kRunSel);
        stopSel                             = sel_registerName(kStopSel);
        terminateSEL                        = sel_registerName(kTerminateSel);
        
        // Application Screen management selectors
        screenSel                           = sel_registerName(kScreenSel);
        unhideSel                           = sel_registerName(kUnhideSel);
        hideSel                             = sel_registerName(kHideSel);
        isHiddenSel                         = sel_registerName(kIsHiddenSel);

        // NSApplicationDelegate lifecycle methods
        applicationWillFinishLaunchingSel   = sel_registerName(kApplicationWillFinishLaunchingSel);
        applicationDidFinishLaunchingSel    = sel_registerName(kApplicationDidFinishLaunchingSel);
        applicationWillTerminateSel         = sel_registerName(kApplicationWillTerminateSel);
        applicationDidBecomeActiveSel       = sel_registerName(kApplicationDidBecomeActiveSel);
        applicationWillResignActiveSel      = sel_registerName(kApplicationWillResignActiveSel);

        // NSWindow creation, display, and management selectors
        initWithContentRectSel              = sel_registerName(kInitWithContentRectSel);
        stringWithUTF8StringSel             = sel_registerName(kStringWithUTF8StringSel);
        setTitleSel                         = sel_registerName(kSetTitleSel);
        orderFrontRegardlessSel             = sel_registerName(kOrderFrontRegardlessSel);
        setAcceptsMouseMovedEventsSel       = sel_registerName(kSetAcceptsMouseMovedEventsSel);
        makeFirstResponderSel               = sel_registerName(kMakeFirstResponderSel);
        makeKeyAndOrderFrontSel             = sel_registerName(kMakeKeyAndOrderFrontSel);
        makeKeyWindowSel                    = sel_registerName(kMakeKeyWindowSel);
        frameSel                            = sel_registerName(kFrameSel);
        setFrameDisplaySel                  = sel_registerName(kSetFrameDisplaySel);
        setFrameSel                         = sel_registerName(kSetFrameSel);
        styleMaskSel                        = sel_registerName(kStyleMaskSel);
        toggleFullScreenSel                 = sel_registerName(kToggleFullScreenSel);
        
        // NSWindowDelegate lifecycle and event methods selectors
        windowDidResizeSel                  = sel_registerName(kWindowDidResizeSel);
        windowWillCloseSel                  = sel_registerName(kWindowWillCloseSel);
        windowDidBecomeKeySel               = sel_registerName(kWindowDidBecomeKeySel);
        windowDidResignKeySel               = sel_registerName(kWindowDidResignKeySel);
        windowDidMiniaturizeSel             = sel_registerName(kWindowDidMiniaturizeSel);
        windowDidDeminiaturizeSel           = sel_registerName(kWindowDidDeminiaturizeSel);

        // NSResponder keyboard and mouse event methods selectors
        keyDownSel                          = sel_registerName(kKeyDownSel);
        keyUpSel                            = sel_registerName(kKeyUpSel);
        flagsChangedSel                     = sel_registerName(kFlagsChangedSel);
        mouseDownSel                        = sel_registerName(kMouseDownSel);
        mouseUpSel                          = sel_registerName(kMouseUpSel);
        mouseDraggedSel                     = sel_registerName(kMouseDraggedSel);
        mouseMovedSel                       = sel_registerName(kMouseMovedSel);
        rightMouseDownSel                   = sel_registerName(kRightMouseDownSel);
        rightMouseUpSel                     = sel_registerName(kRightMouseUpSel);
        rightMouseDraggedSel                = sel_registerName(kRightMouseDraggedSel);
        otherMouseDownSel                   = sel_registerName(kOtherMouseDownSel);
        otherMouseUpSel                     = sel_registerName(kOtherMouseUpSel);
        otherMouseDraggedSel                = sel_registerName(kOtherMouseDraggedSel);
        scrollWheelSel                      = sel_registerName(kScrollWheelSel);
        deltaXSel                           = sel_registerName(kDeltaXSel);
        deltaYSel                           = sel_registerName(kDeltaYSel);
        updateTrackingAreasSel              = sel_registerName(kUpdateTrackingAreasSel);
        mouseEnteredSel                     = sel_registerName(kMouseEnteredSel);
        mouseExitedSel                      = sel_registerName(kMouseExitedSel);
        addTrackingAreaSel                  = sel_registerName(kAddTrackingAreaSel);
        initWithRectSel                     = sel_registerName(kInitWithRectSel);
        
        // Touch event selectors
        setAcceptsTouchEventsSel            = sel_registerName(kSetAcceptsTouchEventsSel);
        setWantsRestingTouchesSel           = sel_registerName(kSetWantsRestingTouchesSel);
        cgEventSel                          = sel_registerName(kCGEventSel);
        touchesBeganSel                     = sel_registerName(kTouchesBeganSel);
        touchesMovedSel                     = sel_registerName(kTouchesMovedSel);
        touchesEndedSel                     = sel_registerName(kTouchesEndedSel);
        touchesCancelledSel                 = sel_registerName(kTouchesCancelledSel);
        touchesMatchingPhaseSel             = sel_registerName(kTouchesMatchingPhaseSel);
        allObjectsSel                       = sel_registerName(kAllObjectsSel);
        normalizedPositionSel               = sel_registerName(kNormalizedPositionSel);
        deviceSizeSel                       = sel_registerName(kDeviceSizeSel);
        identitySel                         = sel_registerName(kIdentitySel);
        objectAtIndexSel                    = sel_registerName(kObjectAtIndexSel);
        touchesCountSel                     = sel_registerName(kTouchCountSel);

        // Tablet / stylus event selectors
        tabletPointSel                      = sel_registerName(kTabletPointSel);
        tabletProximitySel                  = sel_registerName(kTabletProximitySel);
        penPressureSel                      = sel_registerName(kPenPressureSel);
        penRotationSel                      = sel_registerName(kPenRotationSel);
        penTiltSel                          = sel_registerName(kPenTiltSel);
        penIsEnteringProximitySel           = sel_registerName(kPenIsEnteringProximitySel);
        pointingDeviceTypeSel               = sel_registerName(kPointingDeviceTypeSel);
        penTangentialPressureSel            = sel_registerName(kPenTangentialPressureSel);
        pressureChangeSel                   = sel_registerName(kPressureChangeSel);
        stageSel                            = sel_registerName(kStageSel);

        // Managing first responder status and keyboard focus selectors
        acceptsFirstResponderSel            = sel_registerName(kAcceptsFirstResponderSel);
        becomeFirstResponderSel             = sel_registerName(kBecomeFirstResponderSel);
        canBecomeKeyViewSel                 = sel_registerName(kCanBecomeKeyViewSel);
        needsPanelToBecomeKeySel            = sel_registerName(kNeedsPanelToBecomeKeySel);
        drawRectSel                         = sel_registerName(kDrawRectSel);
        reshapeSel                          = sel_registerName(kReshapeSel);
        updateSel                           = sel_registerName(kUpdateSel);

        // NSOpenGL pixel format, view, and context management selectors
        initWithAttributesSel              = sel_registerName(kInitWithAttributesSel);
        initWithFramePixelFormatSel        = sel_registerName(kInitWithFramePixelFormatSel);
        setContentViewSel                  = sel_registerName(kSetContentViewSel);
        contentViewSel                     = sel_registerName(kContentViewSel);
        boundsSel                          = sel_registerName(kBoundsSel);
        convertRectToBackingSel            = sel_registerName(kConvertRectToBackingSel);    // Thank you - Ben the Ultimate Guru
        convertRectFromBackingSel          = sel_registerName(kConvertRectFromBackingSel);  // Thank you - Mr B the Ultimate Teacher!!!
        convertPointToBackingSel           = sel_registerName(kConvertPointToBackingSel);
        convertPointFromBackingSel         = sel_registerName(kConvertPointFromBackingSel);
        convertPointFromViewSel            = sel_registerName(kConvertPointFromViewSel);
        openGLContextSel                   = sel_registerName(kOpenGLContextSel);
        makeCurrentContextSel              = sel_registerName(kMakeCurrentContextSel);
        setAutoresizingMaskSel             = sel_registerName(kSetAutoresizingMaskSel);
        flushBufferSel                     = sel_registerName(kFlushBufferSel);
        displaySel                         = sel_registerName(kDisplaySel);
        CGLContextObjSel                   = sel_registerName(kCGLContextObjSel);
        setValuesSel                       = sel_registerName(kSetValuesSel);
        backingScaleFactorSel              = sel_registerName(kBackingScaleFactorSel);

        // Extracting data from NSEvent objects selectors
        keyCodeSel                         = sel_registerName(kKeyCodeSel);
        charactersSel                      = sel_registerName(kCharactersSel);
        locationInWindowSel                = sel_registerName(kLocationInWindowSel);
        buttonNumberSel                    = sel_registerName(kButtonNumberSel);
        clickCountSel                      = sel_registerName(kClickCountSel);
        modifierFlagsSel                   = sel_registerName(kModifierFlagsSel);
        utf8StringSel                      = sel_registerName(kUTF8StringSel);

        // NSLocale selectors
        currentLocaleSel                   = sel_registerName(kCurrentLocaleSel);
        localeIdentifierSel                = sel_registerName(kLocaleIdentifierSel);
    }

    // Ensures we only initialize selectors once (Thread-safe)
    void ensureInitialized() {
        static std::once_flag initialized;
        std::call_once(initialized, [&]() {
                        initializeSelectors();
        });
    }
}

// Single initialization function called once at startup
void initialize_macos_api() {
    ObjectiveCSEL::initializeSelectors();
}

// Concepts safety templates for Objective-C interactions
template<typename T>
concept CallbackType = std::is_function_v<std::remove_pointer_t<T>>; // Checks if T is a function type

template<typename T>
concept ObjectiveC_Id = std::is_pointer_v<T>;   // Checks if T is a pointer type (Objective-C id types are pointers)

// Autorelease pool management functions
extern "C" {
    extern void* objc_autoreleasePoolPush(void);
    extern void objc_autoreleasePoolPop(void* pool);
}

// Objective-C autorelease pools
class AutoreleasePool {
public:
    AutoreleasePool() noexcept : pool_(objc_autoreleasePoolPush()) {}
    ~AutoreleasePool() noexcept { objc_autoreleasePoolPop(pool_); }
    
    AutoreleasePool(const AutoreleasePool&) = delete;
    AutoreleasePool& operator=(const AutoreleasePool&) = delete;
    AutoreleasePool(AutoreleasePool&&) = delete;
    AutoreleasePool& operator=(AutoreleasePool&&) = delete;

private:
    void* pool_;
};


using NSPoint = CGPoint;
using NSInteger = long;
using NSUInteger = unsigned long;

// Forward declarations
struct Application;
struct Window;
struct OpenGLRenderer;
struct ApplicationDelegate;
struct CustomOpenGLView;
struct WindowDelegate;
struct ImageLoader;

// enums for OpenGL pixel format and rendering capabilities
enum class NSOpenGLPixelFormatAttribute : uint16_t {
    // Renderer selection
    AllRenderers             = 1,    // Use all available renderers
    RendererID               = 70,   // Specific renderer ID
    NoRecovery               = 72,   // No recovery from renderer failures
    Accelerated              = 73,   // Hardware accelerated rendering
    AllowOfflineRenderers    = 96,   // Allow offline renderers
    AcceleratedCompute       = 97,   // Accelerated compute support

    // Buffer configuration
    TripleBuffer             = 3,    // Triple buffering support
    DoubleBuffer             = 5,    // Double buffering (recommended)
    Stereo                   = 6,    // Stereo buffering for 3D
    AuxBuffers               = 7,    // Number of auxiliary buffers
    BackingStore             = 76,   // Backing store configuration
    
    // Color and depth configuration
    ColorSize                = 8,    // Total color buffer size (bits)
    AlphaSize                = 11,   // Alpha channel size (bits)
    DepthSize                = 12,   // Depth buffer size (bits)
    StencilSize              = 13,   // Stencil buffer size (bits)
    AccumSize                = 14,   // Accumulation buffer size (bits)
    ColorFloat               = 58,   // Floating-point color buffer

    // Antialiasing configuration
    Multisample              = 59,   // Multisampling antialiasing
    Supersample              = 60,   // Supersampling antialiasing
    SampleBuffers            = 55,   // Number of sample buffers
    Samples                  = 56,   // Samples per pixel
    SampleAlpha              = 61,   // Sample alpha to coverage
    AuxDepthStencil          = 57,   // Auxiliary depth/stencil buffer

    // Policy configuration
    MinimumPolicy            = 51,   // Minimum attribute policy
    MaximumPolicy            = 52,   // Maximum attribute policy
    ClosestPolicy            = 74,   // Closest match policy

    // Advanced configuration
    OpenGLProfile            = 99,   // OpenGL profile selection
    ScreenMask               = 84,   // Screen mask for multi-display
    VirtualScreenCount       = 128,  // Number of virtual screens
};

// Backward compatibility
static constexpr int NSOpenGLPFADoubleBuffer  = static_cast<int>(NSOpenGLPixelFormatAttribute::DoubleBuffer);
static constexpr int NSOpenGLPFAColorSize     = static_cast<int>(NSOpenGLPixelFormatAttribute::ColorSize);
static constexpr int NSOpenGLPFADepthSize     = static_cast<int>(NSOpenGLPixelFormatAttribute::DepthSize);
static constexpr int NSOpenGLPFAAccelerated   = static_cast<int>(NSOpenGLPixelFormatAttribute::Accelerated);
static constexpr int NSOpenGLPFAOpenGLProfile = static_cast<int>(NSOpenGLPixelFormatAttribute::OpenGLProfile);
static constexpr int NSOpenGLPFASampleBuffers = static_cast<int>(NSOpenGLPixelFormatAttribute::SampleBuffers);
static constexpr int NSOpenGLPFAMultisample        = static_cast<int>(NSOpenGLPixelFormatAttribute::Multisample);
static constexpr int NSOpenGLAllowOfflineRenderers = static_cast<int>(NSOpenGLPixelFormatAttribute::AllowOfflineRenderers);
// static constexpr int NSOpenGLPFAAcceleratedCompute = static_cast<int>(NSOpenGLPixelFormatAttribute::AcceleratedCompute); // Temp remove unused variable warning

// enum for OpenGL profile versions
enum class NSOpenGLProfile : int {
    VersionLegacy    = 0x1000,   // Legacy OpenGL (deprecated)
    Version3_2Core   = 0x3200,   // OpenGL 3.2 Core Profile
    Version4_1Core   = 0x4100,   // OpenGL 4.1 Core Profile
};

// enum for OpenGL context parameters
enum NSOpenGLContextParameter : int {
    NSOpenGLContextParameterSwapInterval       = 222,
    NSOpenGLContextParameterSurfaceOrder       = 235,
    NSOpenGLContextParameterSurfaceOpacity     = 236,
    NSOpenGLContextParameterSurfaceBackingSize = 237
};

// Backward compatibility
//static constexpr int NSOpenGLProfileVersion3_2Core = static_cast<int>(NSOpenGLProfile::Version3_2Core);
static constexpr int NSOpenGLProfileVersion4_1Core = static_cast<int>(NSOpenGLProfile::Version4_1Core);

// enum for window appearance and behavior bit flags
enum class NSWindowStyleMask : uint16_t {
    Titled                   = (1 << 0),     // Window has a title bar
    Closable                 = (1 << 1),     // Window can be closed
    Miniaturizable           = (1 << 2),     // Window can be minimized
    Resizable                = (1 << 3),     // Window can be resized
    UtilityWindow            = (1 << 4),     // Utility window style
    DocModalWindow           = (1 << 6),     // Document-modal window
    NonactivatingPanel       = (1 << 7),     // Non-activating panel
    HUDWindow                = (1 << 13),    // Heads-up display window
    TexturedBackground       = (1 << 8),     // Textured background
    UnifiedTitleAndToolbar   = (1 << 12),    // Unified title and toolbar
    FullScreen               = (1 << 14),    // Full-screen window
    FullSizeContentView      = (1 << 15)     // Full-size content view
};

// Backward compatibility
// static constexpr int NSWindowStyleMaskTitled         = static_cast<int>(NSWindowStyleMask::Titled);          // Temp remove unused variable warning
// static constexpr int NSWindowStyleMaskClosable       = static_cast<int>(NSWindowStyleMask::Closable);        // Temp remove unused variable warning
// static constexpr int NSWindowStyleMaskMiniaturizable = static_cast<int>(NSWindowStyleMask::Miniaturizable);  // Temp remove unused variable warning
// static constexpr int NSWindowStyleMaskResizable      = static_cast<int>(NSWindowStyleMask::Resizable);       // Temp remove unused variable warning
static constexpr int NSWindowStyleMaskFullScreen     = static_cast<int>(NSWindowStyleMask::FullScreen);

// enum for backing store types
enum class NSBackingStoreType : uint8_t {
    Retained      = 0,    // Deprecated - backing store is retained
    Nonretained   = 1,    // Deprecated - backing store is not retained
    Buffered      = 2     // Modern - double-buffered backing store (recommended)
};

static constexpr int NSBackingStoreBuffered = static_cast<int>(NSBackingStoreType::Buffered);

// enum for window creation timing options
enum class NSWindowDefer : uint8_t {
    CreateNow           = 0,    // Create window immediately (NO/false)
    DeferCreation       = 1     // Defer window creation (YES/true)
};

// Backward compatibility
static constexpr int NSWindowCreateNow = static_cast<int>(NSWindowDefer::CreateNow);

// enum for view autoresizing behavior flags
enum class NSAutoresizingMask : uint8_t {
    NotSizable            = 0,    // View maintains fixed size
    MinXMargin            = 1,    // Left margin is flexible
    WidthSizable          = 2,    // Width changes with superview width
    MaxXMargin            = 4,    // Right margin is flexible
    MinYMargin            = 8,    // Bottom margin is flexible
    HeightSizable         = 16,   // Height changes with superview height
    MaxYMargin            = 32    // Top margin is flexible
};

// Backward compatibility
static constexpr int NSViewWidthSizable  = static_cast<int>(NSAutoresizingMask::WidthSizable);
static constexpr int NSViewHeightSizable = static_cast<int>(NSAutoresizingMask::HeightSizable);

//  enum for application activation and UI visibility policies
enum class NSApplicationActivationPolicy : uint8_t {
    Regular    = 0,    // Normal app with dock icon and menu bar Recommended
    Accessory  = 1,    // Utility app, no dock icon but can have menu bar
    Prohibited = 2     // Background only, no UI elements
};

// Backward compatibility
static constexpr int NSApplicationActivationPolicyRegular = static_cast<int>(NSApplicationActivationPolicy::Regular);

// Mouse Tracking const
static constexpr const NSUInteger NSTrackingMouseEnteredAndExited = 0x01;
static constexpr const NSUInteger NSTrackingActiveInKeyWindow = 0x20;
static constexpr const NSUInteger NSTrackingInVisibleRect = 0x200;

/*
* WARNING: Global delegate pointers for Objective-C callbacks
* These global pointers are used to route Objective-C delegate callbacks
* to the appropriate C++ structures and methods.
*
* In short: Don't mess with these unless you know what you're doing!
*
* Global variables are evil, but sometimes necessary for bridging C++ and Objective-C.
*/
static Window*      gptrNSWindowEvents      = nullptr;
static Application* gptrApplicationDelegate = nullptr;
static Window*      gptrWindowDelegate      = nullptr;

// Handles NSApplication delegate methods and lifecycle events
struct ApplicationDelegate {
    Class isa;                      // Objective-C class pointer (required)
};

// Custom NSOpenGLView subclass for handling input events
struct CustomOpenGLView {
    Class isa;                      // Objective-C class pointer (required)
    void* renderer;                 // C Pointer to associated renderer
};

// Handles NSWindow delegate methods and window events
struct WindowDelegate {
    Class isa;                      // Objective-C class pointer (required)
    void* renderer;                 // Pointer to associated renderer
};

// Application management with member initialization
struct Application {
    id nsApp{nullptr};                       // NSApplication instance
    id delegate{nullptr};                    // Application delegate instance

    // Callback function pointers
    std::function<void(void*)> willFinishLaunchingCallback{nullptr};
    std::function<void(void*)> didFinishLaunchingCallback{nullptr};
    std::function<void(void*)> willTerminateCallback{nullptr};
    std::function<void(void*)> didBecomeActiveCallback{nullptr};
    std::function<void(void*)> willResignActiveCallback{nullptr};

    void* willFinishLaunchingUserData{nullptr};    // User data for will finish launching callback
    void* didFinishLaunchingUserData{nullptr};     // User data for did finish launching callback
    void* willTerminateUserData{nullptr};          // User data for will terminate callback
    void* didBecomeActiveUserData{nullptr};        // User data for did become active callback
    void* willResignActiveUserData{nullptr};       // User data for will resign active callback

    // Method function pointers
    void (*initialize)  (struct Application* self){nullptr};
    void (*activate)    (struct Application* self){nullptr};
    void (*run)         (struct Application* self){nullptr};
    void (*destroy)     (struct Application* self){nullptr};
    const char* (*getSystemLocale)(struct Application* self){nullptr};
    
    Application() = default;
    
    ~Application() {}
    
    // Delete copy constructor and assignment
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = default;
    Application& operator=(Application&&) = default;
};

// Window management with member initialization
struct Window {
    id nsWindow{nullptr};                // NSWindow instance
    id delegate{nullptr};                // Window delegate instance
    OpenGLRenderer* renderer{nullptr};   // Associated OpenGL renderer
    NSRect windowFrame{};                // Window frame rectangle
    NSRect contentViewFrame{};           // Content view frame rectangle
    const char* title{nullptr};          // Window title string

    // Event callback function pointers with nullptr initialization
    void (*keyDownCallback)          (unsigned short keyCode, const char* characters, unsigned int modifierFlags, void* userData){nullptr};
    void (*keyUpCallback)            (unsigned short keyCode, const char* characters, unsigned int modifierFlags, void* userData){nullptr};
    void (*flagsChangedCallback)     (unsigned int modifierFlags, void* userData){nullptr};
    void (*mouseDownCallback)        (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*mouseUpCallback)          (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*mouseMovedCallback)       (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*mouseDraggedCallback)     (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*rightMouseDownCallback)   (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*rightMouseUpCallback)     (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*rightMouseDraggedCallback)(double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*otherMouseDownCallback)   (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*otherMouseUpCallback)     (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*otherMouseDraggedCallback)(double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*mouseEnteredCallback)     (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*mouseExitedCallback)      (double x, double y, int buttonNumber,  unsigned int modifierFlags, void* userData){nullptr};
    void (*scrollWheelCallback)      (double x, double y, double deltaX, double deltaY, unsigned int modifierFlags, void* userData){nullptr};

     // Touch event callback function pointers with nullptr initialization
    void (*touchBeganCallback)    (uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData){nullptr};
    void (*touchMovedCallback)    (uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData){nullptr};
    void (*touchEndedCallback)    (uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData){nullptr};
    void (*touchCancelledCallback)(uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData){nullptr};
    void* touchUserData{nullptr};

    // Stylus (tablet) event callback function pointer
    void (*stylusCallback)(uint32_t touchID, double x, double y,
                           float pressure, float rotation,
                           float tiltX, float tiltY,
                           bool bPress, bool bRelease, bool bIsStylus,
                           void* userData){nullptr};
    void* stylusUserData{nullptr};
    bool bStylusInProximity{false}; // true while stylus is hovering near the surface
    
    void* eventUserData{nullptr};   // User data for event callbacks
    BOOL acceptsInputEvents{NO};    // Whether the window accepts input events
    
    // Window delegate event callback function pointers with nullptr initialization
    void (*windowDidResizeCallback)       (void* userData){nullptr};
    void (*windowWillCloseCallback)       (void* userData){nullptr};
    void (*windowDidBecomeKeyCallback)    (void* userData){nullptr};
    void (*windowDidResignKeyCallback)    (void* userData){nullptr};
    void (*windowDidMiniaturizeCallback)  (void* userData){nullptr};
    void (*windowDidDeminiaturizeCallback)(void* userData){nullptr};

    void* windowDidResizeUserData{nullptr};        // User data for window resize callback
    void* windowWillCloseUserData{nullptr};        // User data for window will close callback
    void* windowDidBecomeKeyUserData{nullptr};     // User data for window did become key callback
    void* windowDidResignKeyUserData{nullptr};     // User data for window did resign key callback
    void* windowDidMiniaturizeUserData{nullptr};   // User data for window did miniaturize callback
    void* windowDidDeminiaturizeUserData{nullptr}; // User data for window did deminiaturize callback
    
    // Method function pointers with nullptr initialization
    void (*create)          (struct Window* self, unsigned long styleMask){nullptr};
    void (*show)            (struct Window* self){nullptr};
    void (*destroy)         (struct Window* self){nullptr};
    void (*setDelegate)     (struct Window* self, id delegate){nullptr};
    void (*removeDelegate)  (struct Window* self){nullptr};
    const char* (*getTitle) (const struct Window* self){nullptr};
    void (*setTitle)        (struct Window* self, const char* title){nullptr};

    void (*setWindowSize)       (struct Window* self, double width, double height){nullptr};
    void (*getWindowSize)       (const struct Window* self, double* width, double* height){nullptr};
    void (*setWindowPosition)   (struct Window* self, double x, double y){nullptr};
    void (*getWindowPosition)   (const struct Window* self, double* x, double* y){nullptr};
    void (*setWindowFrame)      (struct Window* self, double x, double y, double width, double height){nullptr};
    void (*getWindowFrame)      (const struct Window* self, double* x, double* y, double* width, double* height){nullptr};
    void (*getCurrentFrame)     (struct Window* self){nullptr};
    void (*getContentViewFrame) (const struct Window* self, double* x, double* y, double* width, double* height){nullptr};
    void (*setContentViewFrame) (struct Window* self, double* x, double* y, double* width, double* height){nullptr};
};

// Modern OpenGL context management and rendering operations
struct OpenGLRenderer {
    id pixelFormat{nullptr};                     // NSOpenGLPixelFormat instance
    id glView{nullptr};                          // NSOpenGLView instance
    id glContext{nullptr};                       // NSOpenGLContext instance
    unsigned int MSAA_Samples{OLC_MSAA_SAMPLES}; // Multisample anti-aliasing samples
    Window* window{nullptr};                     // Associated window pointer

    // Method function pointers with nullptr initialization
    void (*initialize)            (struct OpenGLRenderer* self, Window* window){nullptr};
    void (*setupContext)          (struct OpenGLRenderer* self){nullptr};
    void (*renderYellowBackground)(struct OpenGLRenderer* self){nullptr};
    void (*renderTexturedQuad)    (struct OpenGLRenderer* self, unsigned int textureID){nullptr};
    void* (*getOpenGLContext)     (const struct OpenGLRenderer* self){nullptr};
    void* (*getCGLContextObj)     (struct OpenGLRenderer* self){nullptr};
    void* (*getCGLContextObjPtr)  (struct OpenGLRenderer* self){nullptr};
    void (*makeCurrentContext)    (struct OpenGLRenderer* self){nullptr};
    void (*setVsync)              (struct OpenGLRenderer* self, BOOL enabled){nullptr};
    void (*destroy)               (struct OpenGLRenderer* self){nullptr};
    bool (*resetContextForSize)   (struct OpenGLRenderer* self, double width, double height){nullptr};

};

// Modern image loading and pixel data extraction
struct ImageLoader {
    imageloader_pixel_t* pixelData{nullptr}; // Raw pixel data (RGBA format)
    int width{kMinValidDimension};     // Image width in pixels
    int height{kMinValidDimension};    // Image height in pixels
    int bytesPerPixel{kZeroBytes};     // Number of bytes per pixel (typically 4 for RGBA)
    int bytesPerRow{kZeroRows};        // Number of bytes per row
    BOOL hasAlpha{NO};                 // Whether image has alpha channel
    
    // Method function pointers with nullptr initialization
    BOOL (*loadFromFile)           (struct ImageLoader* self, const char* filePath){nullptr};
    BOOL (*loadFromMemory)         (struct ImageLoader* self, const uint8_t* data, size_t bytes);
    void (*destroy)                (struct ImageLoader* self){nullptr};
    unsigned char* (*getPixelData) (const struct ImageLoader* self){nullptr};
    void (*getImageInfo)           (const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel){nullptr};
    void (*getDetailedInfo)        (const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel, int* bytesPerRow, BOOL* hasAlpha){nullptr};
    BOOL (*isLoaded)               (const struct ImageLoader* self){nullptr};
    BOOL (*getPixel)               (const struct ImageLoader* self, int x, int y, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha){nullptr};
    unsigned int (*createOpenGLTexture)(const struct ImageLoader* self){nullptr};
};

// Called when application is about to finish launching
void applicationWillFinishLaunching(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification; // Remove unused parameter warnings
    if (gptrApplicationDelegate && gptrApplicationDelegate->willFinishLaunchingCallback) {
        gptrApplicationDelegate->willFinishLaunchingCallback(gptrApplicationDelegate->willFinishLaunchingUserData);
    }
}

// Called when application has finished launching
void applicationDidFinishLaunching(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;
    if (gptrApplicationDelegate && gptrApplicationDelegate->didFinishLaunchingCallback) {
        gptrApplicationDelegate->didFinishLaunchingCallback(gptrApplicationDelegate->didFinishLaunchingUserData);
    }
}

// Called when application is about to terminate
void applicationWillTerminate(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;
    if (gptrApplicationDelegate && gptrApplicationDelegate->willTerminateCallback) {
        gptrApplicationDelegate->willTerminateCallback(gptrApplicationDelegate->willTerminateUserData);
    }
}

// Called when application becomes active
void applicationDidBecomeActive(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;
    if (gptrApplicationDelegate && gptrApplicationDelegate->didBecomeActiveCallback) {
        gptrApplicationDelegate->didBecomeActiveCallback(gptrApplicationDelegate->didBecomeActiveUserData);
    }
}

// Called when application resigns active status
void applicationWillResignActive(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;
    if (gptrApplicationDelegate && gptrApplicationDelegate->willResignActiveCallback) {
        gptrApplicationDelegate->willResignActiveCallback(gptrApplicationDelegate->willResignActiveUserData);
    }
}

// Create and configure application delegate
id createApplicationDelegate() {

    // Get NSObject class as superclass using const string
    Class NSObjectClass = objc_getClass(kNSObjectClass);
    
    // Create a new class inheriting from NSObject using const string
    Class AppDelegateClass = objc_allocateClassPair(NSObjectClass, kAppDelegateClass, 0);
    
    // Add application delegate methods
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationWillFinishLaunchingSel, (IMP)applicationWillFinishLaunching, kEventHandlerMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationDidFinishLaunchingSel,  (IMP)applicationDidFinishLaunching,  kEventHandlerMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationWillTerminateSel,       (IMP)applicationWillTerminate,       kEventHandlerMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationDidBecomeActiveSel,     (IMP)applicationDidBecomeActive,     kEventHandlerMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationWillResignActiveSel,    (IMP)applicationWillResignActive,    kEventHandlerMethodTypeEncoding);

    // Register the class with the runtime
    objc_registerClassPair(AppDelegateClass);
    
    // Create an instance
    id delegate  = ((id(*)(Class, SEL))objc_msgSend)(AppDelegateClass, ObjectiveCSEL::allocSel);
    delegate     = ((id(*)(id, SEL))objc_msgSend)(delegate, ObjectiveCSEL::initSel);
    
    return delegate;
}

//====================================================================//
// Keyboard Event Handling

// Structure to hold common key event data
struct KeyEventData {
    unsigned short keyCode = 0;
    const char* characters = nullptr;
    unsigned int modifierFlags = 0;
};

// Extract common key event data from NSEvent
KeyEventData extractKeyEventData(id event) {
    KeyEventData data;
    data.keyCode = ((unsigned short(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::keyCodeSel);
    id characters = ((id(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::charactersSel);
    data.characters = ((const char*(*)(id, SEL))objc_msgSend)(characters, ObjectiveCSEL::utf8StringSel);
    data.modifierFlags = ((unsigned int(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::modifierFlagsSel);
    return data;
}

 // Handle key down events
void view_keyDown(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd; // Remove unused parameter warnings

    KeyEventData data = extractKeyEventData(event);

    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->keyDownCallback) [[likely]] {
        gptrNSWindowEvents->keyDownCallback(data.keyCode, data.characters, data.modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

// Handle key up events
void view_keyUp(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;

    KeyEventData data = extractKeyEventData(event);
    
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->keyUpCallback) [[likely]] {
        gptrNSWindowEvents->keyUpCallback(data.keyCode, data.characters, data.modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

void view_flagsChanged(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;

    unsigned int modifierFlags = ((unsigned int(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::modifierFlagsSel);

    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->flagsChangedCallback) [[likely]] {
        gptrNSWindowEvents->flagsChangedCallback(modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

//====================================================================//
// Mouse Event Handling

// All values are converted to physical backing pixels so they match the OpenGL
// viewport on HighDPI displays - AppKit reports locations in logical points, but
// PGE is in physical pixels.
void convertToContentViewCoordinates(NSPoint& location) {

    if(gptrNSWindowEvents && gptrNSWindowEvents->nsWindow) [[likely]]
    {
        // Get the content view
        id contentView = ((id(*)(id, SEL))objc_msgSend)(gptrNSWindowEvents->nsWindow, ObjectiveCSEL::contentViewSel);
        if (contentView) {

            
            // Convert from window coordinates to view coordinates (still logical points)
            NSPoint contentLocation = ((NSPoint(*)(id, SEL, NSPoint, id))objc_msgSend)(
                 contentView, ObjectiveCSEL::convertPointFromViewSel, location, nil);
            
            // Scale from logical points to physical backing pixels
            NSPoint pixelLocation = ((NSPoint(*)(id, SEL, NSPoint))objc_msgSend)(
                contentView, ObjectiveCSEL::convertPointToBackingSel, contentLocation);

            // Get content bounds in physical pixels for Y-flip
            NSRect contentBounds = ((NSRect(*)(id, SEL))objc_msgSend)(contentView, ObjectiveCSEL::boundsSel);
            NSRect pixelBounds   = ((NSRect(*)(id, SEL, NSRect))objc_msgSend)(contentView, ObjectiveCSEL::convertRectToBackingSel, contentBounds);

            // Flip Y coordinate from bottom-left to top-left
            location.x = pixelLocation.x;
            location.y = pixelBounds.height - pixelLocation.y;
            
        }
    }
    else
    {
        // Fallback: use default window height
        location.y = kDefaultWindowHeight - location.y;
    }
}

// Mouse event data structure to hold common mouse event information
struct MouseEventData {
    NSPoint location         = {0.0, 0.0};
    NSUInteger modifierFlags = 0;
    NSInteger buttonNumber   = 0;
};

// Extract common mouse event data from NSEvent
MouseEventData extractMouseEventData(id event) {
    MouseEventData data;
    data.location = ((NSPoint(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::locationInWindowSel);
    data.modifierFlags = ((NSUInteger(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::modifierFlagsSel);
    data.buttonNumber = ((NSInteger(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::buttonNumberSel);
    
    // Convert to content view coordinates and flip Y coordinate
    convertToContentViewCoordinates(data.location);
    return data;
}

// Stylus event data structure to hold common stylus event information
struct StylusEventData {

    uint32_t nTabletDeviceID   = 0;          // Unique identifier for the tablet device
    uint8_t nPointerType        = 0;         // Pointer type 0 = Mouse, 1 = Pen, 2 = Eraser
    uint8_t nPointerEventType   = 0;         // Type of pointer  0 = Mouse Event, 1 = Pen Event, 2 = Pen Proximity Event
    NSPoint nspLocation        = {0.0, 0.0}; // Location of the stylus event
    double dPressure           = 0.0;        // Pressure applied by the stylus
    double dTangentialPressure = 0.0;        // Tangential pressure applied by the stylus
    double dRotationRadians    = 0.0;        // Rotation of the stylus in radians
    double dTiltX_radians      = 0.0;        // Tilt of the stylus in the X direction in radians
    double dTiltY_radians      = 0.0;        // Tilt of the stylus in the Y direction in radians
    uint16_t buttonMask        = 0;          // Bitmask for stylus buttons (MAX 16 buttons, 1 = pressed, 0 = released)

};

// Extract common mouse event data from NSEvent
StylusEventData extractStylusEventData(id event, uint8_t nPointerEventType) {

    StylusEventData data;
    id cgEvent               = ((id(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::cgEventSel);
    data.nspLocation         = ((NSPoint(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::locationInWindowSel);
    data.nTabletDeviceID     = (uint32_t)CGEventGetIntegerValueField((CGEventRef)cgEvent, (CGEventField)kCGTabletEventDeviceID);

    data.buttonMask          = (uint16_t)CGEventGetIntegerValueField((CGEventRef)cgEvent, (CGEventField)kCGTabletEventPointButtons);
    data.nPointerEventType    = (uint8_t)nPointerEventType; // 0 = Mouse Event, 1 = Pen Event, 2 = Pen Proximity Event

    data.dPressure           = CGEventGetDoubleValueField((CGEventRef)cgEvent, (CGEventField)kCGTabletEventPointPressure);
    data.dRotationRadians    = CGEventGetDoubleValueField((CGEventRef)cgEvent, (CGEventField)kCGTabletEventRotation) * (M_PI / 180.0);
    data.dTangentialPressure = CGEventGetDoubleValueField((CGEventRef)cgEvent, (CGEventField)kCGTabletEventTangentialPressure);

    data.nPointerType        = ((uint8_t(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::pointingDeviceTypeSel); 

    double tiltX_raw         = CGEventGetDoubleValueField((CGEventRef)cgEvent, kCGTabletEventTiltX);
    double tiltY_raw         = CGEventGetDoubleValueField((CGEventRef)cgEvent, kCGTabletEventTiltY);

    // Clamp to safe math bounds (-1.0 to 1.0) to avoid domain errors in asin
    tiltX_raw                = std::fmax(-1.0, std::fmin(1.0, tiltX_raw));
    tiltY_raw                = std::fmax(-1.0, std::fmin(1.0, tiltY_raw));

    // Convert to true physical angles in RADIANS -PI/2 to +PI/2
    data.dTiltX_radians      = std::asin(tiltX_raw);
    data.dTiltY_radians      = std::asin(tiltY_raw);

    // Convert to content view coordinates and flip Y coordinate
    convertToContentViewCoordinates(data.nspLocation);
    
    return data;
}

//====================================================================//
// Touch Event Handling (macOS trackpad multi-touch via NSTouchPhase)


// NSTouchPhase bitmask values
enum NSTouchPhase : NSUInteger {
    NSTouchPhaseBegan      = 1 << 0,
    NSTouchPhaseMoved      = 1 << 1,
    NSTouchPhaseStationary = 1 << 2,
    NSTouchPhaseEnded      = 1 << 3,
    NSTouchPhaseCancelled  = 1 << 4,
    NSTouchPhaseAny        = ULONG_MAX
};

enum NSPressureStage : NSInteger {
    NSPressureStageLight   = 0,  // Light press
    NSPressureStageNormal  = 1,  // Normal press
    NSPressureStageForce   = 2   // Force click
};

enum class ForceTouchPressure : int {
    Light   = 25,   // 0.25 - Light touch
    Normal  = 50,   // 0.50 - Normal press
    Force   = 100,  // 1.00 - Force click
};

// Returns the approximated trackpad touch size in pixels based on the linear force value (0.0 to 1.0)
CGSize getApproxTrackPadTouchSize(float linearForce){
    
    CGSize cgTouchSize = {0.0, 0.0};
    const float mmToPoints = 72.0f / 25.4f;     // ~2.834 points per mm
    float minSizePoints = 2.0f  * mmToPoints;   // 3mm lower bound approx values from Google
    float maxSizePoints = 20.0f * mmToPoints;   // 16mm upper bound
    
    // Interpolate the finger footprint size within our point boundaries
    float touchSizeInPoints = minSizePoints + (linearForce * (maxSizePoints - minSizePoints) / 2.0f);

    // Retrieve the window scale factor (1.0 for Standard, 2.0 for Retina, 3.0 for bannana displays, etc.)
    CGFloat pixelScale = 1.0f;
    if (gptrNSWindowEvents && gptrNSWindowEvents->nsWindow) {
        // Get backing scale factor from the content view
        id contentView = ((id(*)(id, SEL))objc_msgSend)(gptrWindowDelegate->nsWindow, ObjectiveCSEL::contentViewSel);
        if (contentView) {
            pixelScale = ((CGFloat(*)(id, SEL))objc_msgSend)(contentView, ObjectiveCSEL::backingScaleFactorSel);
        }
    }
    cgTouchSize = CGSizeMake(touchSizeInPoints * pixelScale, touchSizeInPoints * pixelScale);
    return cgTouchSize;
};

// Mouse event data structure to hold common mouse event information
struct TouchEventData {
    uint32_t touchID    = 0;          // Stable sequential touch ID
    NSPoint nspLocation = {0.0,0.0};  // X coordinate in content-view pixel coordinates
    double pressure     = 0.0;        // Pressure value
    CGSize nsTouchSize  = {0.0, 0.0}; // Touch Size X (not available on macOS trackpad)
    CGSize cgDeviceSize = {0.0, 0.0}; // Device size (width, height) in ponits
    bool bIsActive      = true;       // Touch is active (true for Began/Moved, false for Ended/Cancelled)
    
};

std::unordered_map<uintptr_t, TouchEventData> sTouchIDEvents;
static std::unordered_map<uintptr_t, uint32_t> sTouchIDMap;
static uint32_t sNextTouchID = 0;
CGSize cgsApproxTouchSize{0.0, 0.0}; // Stores the approx width and height of a touch event in CG coordinates

//Iterate through Touches, updates and fires the callback.
static void updateTouchData(id event, NSUInteger phase,
    void (*callback)(uint32_t, double, double, double, double, void*), void* userData, bool bRemove = false)
{
    if (!callback) return;
    
    NSUInteger count = 0;
    id touchArray = nullptr;
    id touchCollection = ((id(*)(id, SEL, NSUInteger, id))objc_msgSend)( event, ObjectiveCSEL::touchesMatchingPhaseSel, phase, nil);
    
    // Edge case, there can be an touch event before a pressure event, therefore we need to check if the approximate touch size has been initialized
    if(cgsApproxTouchSize.width == 0.0 || cgsApproxTouchSize.height == 0.0)
    {
        cgsApproxTouchSize = getApproxTrackPadTouchSize(NSPressureStageNormal);
    }
        
    
    if (touchCollection)
    {
        count      = ((NSUInteger(*)(id, SEL))objc_msgSend)(touchCollection, ObjectiveCSEL::touchesCountSel);
        touchArray = ((id(*)(id, SEL))objc_msgSend)(touchCollection, ObjectiveCSEL::allObjectsSel);
        
        // Get current screen the window is on
        id currentScreen = ((id(*)(id, SEL))objc_msgSend)(gptrWindowDelegate->nsWindow, ObjectiveCSEL::screenSel);
        NSRect screenFrame = ((NSRect(*)(id, SEL))objc_msgSend)(currentScreen, ObjectiveCSEL::frameSel);
        
        for (NSUInteger i = 0; i < count; ++i) {
            
            id touch = ((id(*)(id, SEL, NSUInteger))objc_msgSend)(touchArray, ObjectiveCSEL::objectAtIndexSel, i);
            if (!touch) continue;
            TouchEventData data;
            
            // normalizedPosition is NSPoint in [0,1] range on the trackpad surface
            NSPoint normPos = ((NSPoint(*)(id, SEL))objc_msgSend)(touch, ObjectiveCSEL::normalizedPositionSel);
            
            // Convert normalized [0,1] → content-view pixel coordinates (flip Y to top-left origin)
            data.nspLocation.x = normPos.x * screenFrame.width;
            data.nspLocation.y = (1.0 - normPos.y) * screenFrame.height;
            
            // Get Device Size
            data.cgDeviceSize = ((CGSize(*)(id, SEL))objc_msgSend)(touch, ObjectiveCSEL::deviceSizeSel);
            // Approximate touch size based on pressure stage (macOS does not provide actual touch size)
            data.nsTouchSize = cgsApproxTouchSize;
            data.bIsActive = !bRemove;

            // Assign a stable sequential uint32_t IDs same as iOS does
            id        identity = ((id(*)(id, SEL))objc_msgSend)(touch, ObjectiveCSEL::identitySel);
            uintptr_t key      = reinterpret_cast<uintptr_t>(identity);
            
            auto [it, inserted] = sTouchIDMap.emplace(key, sNextTouchID);
            if (inserted) ++sNextTouchID;
            uint32_t tid = it->second;
            data.touchID = tid;
            sTouchIDEvents.emplace(key, data);
            callback(tid, data.nspLocation.x, data.nspLocation.y, data.nsTouchSize.width,data.nsTouchSize.height, userData);

            // Clean up map entry once the touch has ended or been cancelled
            if (bRemove)
            {
                sTouchIDEvents.erase(key);
                sTouchIDMap.erase(key);
            }
            
        }
        
    }

    // A bit painful, but we need to check of any keys that have been removed as the touchleave event happened outside of our application
    std::vector<uintptr_t> vFoundKeys;
    std::vector<uintptr_t> vMissingKeys;
    touchCollection = ((id(*)(id, SEL, NSUInteger, id))objc_msgSend)( event, ObjectiveCSEL::touchesMatchingPhaseSel, NSTouchPhaseAny, nil);
    
    // If we have a touch collection and the phase is not cancelled, we can check for missing keys, otherwise we will just remove all keys from the map
    if (touchCollection && phase != NSTouchPhaseCancelled)
    {
        count       = ((NSUInteger(*)(id, SEL))objc_msgSend)(touchCollection, ObjectiveCSEL::touchesCountSel);
        touchArray  = ((id(*)(id, SEL))objc_msgSend)(touchCollection, ObjectiveCSEL::allObjectsSel);
        for (NSUInteger i = 0; i < count; ++i) {
            
            id touch = ((id(*)(id, SEL, NSUInteger))objc_msgSend)(touchArray, ObjectiveCSEL::objectAtIndexSel, i);
            if (!touch) continue;
            id identity = ((id(*)(id, SEL))objc_msgSend)(touch, ObjectiveCSEL::identitySel);
            uintptr_t key = reinterpret_cast<uintptr_t>(identity);
            vFoundKeys.push_back(key);

        }
    }
    
    // Find the missing keys and fire the touchEndedCallback for them
    for(auto& [key, data] : sTouchIDEvents) {
        
        if(vFoundKeys.size() == 0 || std::find(vFoundKeys.begin(), vFoundKeys.end(), key) == vFoundKeys.end()) {
            gptrNSWindowEvents->touchEndedCallback(data.touchID, data.nspLocation.x, data.nspLocation.y, data.nsTouchSize.width ,data.nsTouchSize.height, userData);
            vMissingKeys.push_back(key);
        }
       
    }
    
    // Remove the missing keys from the maps
    for(auto& fKeys: vMissingKeys) {
        sTouchIDEvents.erase(fKeys);
        sTouchIDMap.erase(fKeys);
    }
    
    vFoundKeys.clear(); vMissingKeys.clear();
    
    // Finally reset the next touch ID if there are no active touches remaining
    if (sTouchIDMap.size() == 0) sNextTouchID = 0;
}

// TODO move to new location
// Handle Force Touch pressure changes on trackpad (single-finger press)
void view_pressureChange(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
    
    auto toNormalizedPressure = [](ForceTouchPressure pressure) -> double {
        return static_cast<double>(static_cast<int>(pressure)) / 100.0;
    };
       
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents) [[likely]] {
        
        // Get CGEvent to determine event subtype
        CGEventRef cgEvent = ((CGEventRef(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::cgEventSel);
        uint8_t nPointerEventType = CGEventGetIntegerValueField(cgEvent, kCGMouseEventSubtype);
           
        // A force touch trackpad event will have a subtype of 0 (default) or greater than 2 (tablet proximity)
        if (nPointerEventType == kCGEventMouseSubtypeDefault || nPointerEventType > kCGEventMouseSubtypeTabletProximity) {

            // KRespondsToSelector
            BOOL supportsStage = ((BOOL(*)(id, SEL, SEL))objc_msgSend)(event,sel_getUid(kRespondsToSelector),ObjectiveCSEL::stageSel);

            NSInteger stage = NSPressureStageNormal;
            if(supportsStage)
            {
                // stage returns: 0 = normal click, 1 = light press, 2 = force click
                stage = ((NSInteger(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::stageSel);
            }
            
            // Map stage to normalized pressure values as requested
            double pressure = toNormalizedPressure(ForceTouchPressure::Normal);
                       
            switch (stage) {
                case NSPressureStageLight:
                    pressure = toNormalizedPressure(ForceTouchPressure::Light);
                    break;
                case NSPressureStageNormal:
                    pressure = toNormalizedPressure(ForceTouchPressure::Normal);
                    break;
                case NSPressureStageForce:
                    pressure = toNormalizedPressure(ForceTouchPressure::Force);
                    break;
                default:
                    pressure = toNormalizedPressure(ForceTouchPressure::Normal);
                    break;
            }
            
            // now we need to work out the size
            cgsApproxTouchSize = getApproxTrackPadTouchSize(pressure);
            
            for(auto& [key, data] : sTouchIDEvents) {
                data.pressure = pressure;
                data.nsTouchSize = cgsApproxTouchSize;
            }
            
        }
    }
}

// Handle left mouse down events
void view_mouseDown(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd; // Remove unused parameter warnings
    
    CGEventRef cgEvent        = ((CGEventRef(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::cgEventSel);
    uint8_t nPointerEventType = CGEventGetIntegerValueField(cgEvent, kCGMouseEventSubtype);

    // Is it a mouse drag event (subtype 0) or a tablet pointer event (subtype 1 or 2)? Thanks Mr B
    if (nPointerEventType != kCGEventMouseSubtypeTabletPoint && nPointerEventType != kCGEventMouseSubtypeTabletProximity) [[likely]] {

        if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->mouseDownCallback) [[likely]] {
            MouseEventData data = extractMouseEventData(event);
            gptrNSWindowEvents->mouseDownCallback(data.location.x, data.location.y, (int)data.buttonNumber,
                                                (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
        }
    }
    else if (nPointerEventType == 1 || nPointerEventType == 2)
    {
        // Tablet Pointer is Subtype 1 or 2 (1: Pen Event, 2: Proximity Event)
        gptrNSWindowEvents->bStylusInProximity = false;
        StylusEventData data = extractStylusEventData(event, nPointerEventType);
        gptrNSWindowEvents->stylusCallback(
                        data.nTabletDeviceID,
                        data.nspLocation.x, data.nspLocation.y,
                        data.dPressure, data.dRotationRadians, data.dTiltX_radians, data.dTiltY_radians,
                        true, false, true,
                        gptrNSWindowEvents->stylusUserData);

    }

}

// Handle left mouse up events
void view_mouseUp(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;

    CGEventRef cgEvent        = ((CGEventRef(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::cgEventSel);
    uint8_t nPointerEventType = CGEventGetIntegerValueField(cgEvent, kCGMouseEventSubtype);

    // Is it a mouse drag event (subtype 0) or a tablet pointer event (subtype 1 or 2)? Thanks Mr B
    if (nPointerEventType != kCGEventMouseSubtypeTabletPoint && nPointerEventType != kCGEventMouseSubtypeTabletProximity) [[likely]] {

        MouseEventData data = extractMouseEventData(event);
        if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->mouseUpCallback) [[likely]] {
            gptrNSWindowEvents->mouseUpCallback(data.location.x, data.location.y, (int)data.buttonNumber,
                                                (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
        }
    }
    else if(nPointerEventType == 1 || nPointerEventType == 2)
    {
        // Tablet Pointer is Subtype 1 or 2 (1: Pen Event, 2: Proximity Event)
        gptrNSWindowEvents->bStylusInProximity = false;
        StylusEventData data = extractStylusEventData(event, nPointerEventType);
        gptrNSWindowEvents->stylusCallback(
                        data.nTabletDeviceID,
                        data.nspLocation.x, data.nspLocation.y,
                        data.dPressure, data.dRotationRadians, data.dTiltX_radians, data.dTiltY_radians,
                        false, true, true,
                        gptrNSWindowEvents->stylusUserData);

    }

}

// Handle mouse drag events
void view_mouseDragged(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;

    CGEventRef cgEvent        = ((CGEventRef(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::cgEventSel);
    uint8_t nPointerEventType = CGEventGetIntegerValueField(cgEvent, kCGMouseEventSubtype);

    // Is it a mouse drag event (subtype 0) or a tablet pointer event (subtype 1 or 2)? Thanks Mr B
    if (nPointerEventType != kCGEventMouseSubtypeTabletPoint && nPointerEventType != kCGEventMouseSubtypeTabletProximity) [[likely]] {

        if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->mouseDraggedCallback) [[likely]] {
            MouseEventData data = extractMouseEventData(event);
            gptrNSWindowEvents->mouseDraggedCallback(data.location.x, data.location.y, (int)data.buttonNumber, (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
        }
    }
    else if (nPointerEventType == 1 || nPointerEventType == 2)
    {
        // Tablet Pointer is Subtype 1 or 2 (1: Pen Event, 2: Proximity Event)
        gptrNSWindowEvents->bStylusInProximity = false;
        StylusEventData data = extractStylusEventData(event, nPointerEventType);
        gptrNSWindowEvents->stylusCallback(
                        data.nTabletDeviceID,
                        data.nspLocation.x, data.nspLocation.y,
                        data.dPressure, data.dRotationRadians, data.dTiltX_radians, data.dTiltY_radians,
                        true, false, true,
                        gptrNSWindowEvents->stylusUserData);
   }

}


// Handle mouse movement events
void view_mouseMoved(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
   
    // a Mouse move event and Stlus roximity Event have the same properties.
    NSPoint location         = ((NSPoint(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::locationInWindowSel);
    NSUInteger modifierFlags = ((NSUInteger(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::modifierFlagsSel);
    convertToContentViewCoordinates(location);
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->mouseMovedCallback) [[likely]] {
        gptrNSWindowEvents->mouseMovedCallback(location.x, location.y, kNoButton, (unsigned int)modifierFlags, gptrNSWindowEvents->eventUserData);
    }
    
    
}

// Handle right mouse down events
void view_rightMouseDown(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
       
    MouseEventData data = extractMouseEventData(event);
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->rightMouseDownCallback) [[likely]] {
        gptrNSWindowEvents->rightMouseDownCallback(data.location.x, data.location.y, (int)data.buttonNumber, (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

// Handle right mouse up events
void view_rightMouseUp(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
    
    MouseEventData data = extractMouseEventData(event);
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->rightMouseUpCallback) [[likely]] {
        gptrNSWindowEvents->rightMouseUpCallback(data.location.x, data.location.y, (int)data.buttonNumber, (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

// Handle right mouse drag events
void view_rightMouseDragged(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
    
    MouseEventData data = extractMouseEventData(event);
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->rightMouseDraggedCallback) [[likely]] {
        gptrNSWindowEvents->rightMouseDraggedCallback(data.location.x, data.location.y, (int)data.buttonNumber, (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
    }
   
}

// Handle other mouse button down events
void view_otherMouseDown(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
    
    MouseEventData data = extractMouseEventData(event);
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->otherMouseDownCallback) [[likely]] {
        gptrNSWindowEvents->otherMouseDownCallback(data.location.x, data.location.y, (int)data.buttonNumber, (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

// Handle other mouse button up events
void view_otherMouseUp(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
    
    MouseEventData data = extractMouseEventData(event);
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->otherMouseUpCallback) {
        gptrNSWindowEvents->otherMouseUpCallback(data.location.x, data.location.y, (int)data.buttonNumber, (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

void view_otherMouseDragged(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
    
    MouseEventData data = extractMouseEventData(event);
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->otherMouseDraggedCallback) [[likely]] {
        gptrNSWindowEvents->otherMouseDraggedCallback(data.location.x, data.location.y, (int)data.buttonNumber, (unsigned int)data.modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

// Handle scroll wheel events
void view_scrollWheel(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;
  
    NSPoint location         = ((NSPoint(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::locationInWindowSel);
    NSUInteger modifierFlags = ((NSUInteger(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::modifierFlagsSel);
    double deltaX            = ((double(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::deltaXSel);
    double deltaY            = ((double(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::deltaYSel);

     convertToContentViewCoordinates(location);

    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->scrollWheelCallback) [[likely]] {
        gptrNSWindowEvents->scrollWheelCallback(location.x, location.y, deltaX, deltaY, (unsigned int)modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

// New events for mouse entered/exited
void view_mouseEntered(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;(void)event;
    
    // Allow hiding cursor when mouse is inside window
    bAllowHideCursor = true;
    
    // If the cursor is currently hidden, hide it again to ensure it stays hidden while inside the window
    if(bHideCursor)
        ((void (*)(Class, SEL))objc_msgSend)(objc_getClass(kNSCursorClass), ObjectiveCSEL::hideSel);
    
    NSPoint location         = ((NSPoint(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::locationInWindowSel);
    NSUInteger modifierFlags = ((NSUInteger(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::modifierFlagsSel);

    convertToContentViewCoordinates(location);

    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->mouseEnteredCallback) [[likely]] {
        gptrNSWindowEvents->mouseEnteredCallback(location.x, location.y, kNoButton, (unsigned int)modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}

void view_mouseExited(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;(void)event;
    
    // Ensure cursor is visible when leaving window
    bAllowHideCursor = false;
    ((void (*)(Class, SEL))objc_msgSend)(objc_getClass(kNSCursorClass), ObjectiveCSEL::unhideSel);
    
    NSPoint location         = ((NSPoint(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::locationInWindowSel);
    NSUInteger modifierFlags = ((NSUInteger(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::modifierFlagsSel);

    convertToContentViewCoordinates(location);

    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents && gptrNSWindowEvents->mouseExitedCallback) [[likely]] {
        gptrNSWindowEvents->mouseExitedCallback(location.x, location.y, kNoButton, (unsigned int)modifierFlags, gptrNSWindowEvents->eventUserData);
    }
}


void view_touchesBegan(id self, SEL _cmd, id event) {
    (void)_cmd;
    
    // Ensure event is not null before parsing
    if (!event) return;
    
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents) [[likely]] {
        updateTouchData(event, NSTouchPhaseBegan,
            gptrNSWindowEvents->touchBeganCallback, gptrNSWindowEvents->touchUserData);
    }
}

void view_touchesMoved(id self, SEL _cmd, id event) {
    (void)_cmd;
    // Ensure event is not null before parsing
    if (!event) return;
    
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents) [[likely]] {
        updateTouchData(event, NSTouchPhaseMoved,
            gptrNSWindowEvents->touchMovedCallback, gptrNSWindowEvents->touchUserData);
    }
}

void view_touchesEnded(id self, SEL _cmd, id event) {
    (void)_cmd;
    // Ensure event is not null before parsing
    if (!event) return;
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents) [[likely]] {
        updateTouchData(event, NSTouchPhaseEnded,
            gptrNSWindowEvents->touchEndedCallback, gptrNSWindowEvents->touchUserData, true);
    }
}

void view_touchesCancelled(id self, SEL _cmd, id event) {
    (void)_cmd;
    // Ensure event is not null before parsing
    if (!event) return;
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents) [[likely]] {
        updateTouchData(event, NSTouchPhaseCancelled,
            gptrNSWindowEvents->touchCancelledCallback, gptrNSWindowEvents->touchUserData, true);
    }
}

//====================================================================//
// Stylus / Tablet Event Handling (NSTabletPoint / NSTabletProximity)

// This should only be called when a Stylus driver is not installed or it an Apple device
void view_tabletProximity(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;(void)event;
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents) [[likely]] {
        // Handle tablet proximity events here
        BOOL entering = ((BOOL(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::penIsEnteringProximitySel);
        // For furture support should call a stylusCallback with proximity event data, but for now just set the flag
        gptrNSWindowEvents->bStylusInProximity = (entering == YES);
    }
}

// This should only be called when a Stylus driver is not installed or it an Apple device
void view_tabletPoint(id self, SEL _cmd, id event) {
    (void)self;(void)_cmd;(void)event;
    if (gptrNSWindowEvents && gptrNSWindowEvents->acceptsInputEvents) [[likely]] {
        // Handle tablet point events here
    }
}


void view_updateTrackingAreas(id self, SEL _cmd) {
    (void)self;(void)_cmd;

    // Create new tracking area
    NSRect bounds = ((NSRect(*)(id, SEL))objc_msgSend)(self, ObjectiveCSEL::boundsSel);
    
    // Correct tracking options: mouse entered/exited + active in key window
    NSUInteger options = NSTrackingMouseEnteredAndExited | NSTrackingActiveInKeyWindow | NSTrackingInVisibleRect;
    
    Class NSTrackingAreaClass = objc_getClass(kTrackingAreaClass);
    id trackingArea = ((id(*)(id, SEL, NSRect, NSUInteger, id, id))objc_msgSend)(
        ((id(*)(Class, SEL))objc_msgSend)(NSTrackingAreaClass, ObjectiveCSEL::allocSel),
        ObjectiveCSEL::initWithRectSel, bounds, options, self, nil);
    
    ((void(*)(id, SEL, id))objc_msgSend)(self, ObjectiveCSEL::addTrackingAreaSel, trackingArea);
}

// Determine if view can accept first responder status
id view_acceptsFirstResponder(id self, SEL _cmd) {
    (void)self;(void)_cmd;
    return (id)(uintptr_t)YES; // Return YES to accept first responder status
}

// Make the view the first responder
id view_becomeFirstResponder(id self, SEL _cmd) {
    (void)self;(void)_cmd;
    return (id)(uintptr_t)YES; // Return YES to become first responder
}

// Determine if the view can become key view
id view_canBecomeKeyView(id self, SEL _cmd) {
    (void)self;(void)_cmd;
    return (id)(uintptr_t)YES;
}

// Determine if the view needs the panel to become key
id view_needsPanelToBecomeKey(id self, SEL _cmd) {
    (void)self;(void)_cmd;
    return (id)(uintptr_t)YES;
}

// Empty drawRect method - prevents automatic drawing on main thread
void view_drawRect(id self, SEL _cmd, NSRect dirtyRect) {
    (void)self;(void)_cmd;(void)dirtyRect;
}

// Empty reshape method - prevents automatic context reshaping on main thread
void view_reshape(id self, SEL _cmd) {
    (void)self;(void)_cmd;
}

// Empty update method - prevents automatic context updates on main thread
void view_update(id self, SEL _cmd) {
    (void)self;(void)_cmd;
}

// Create custom OpenGL view class
Class createCustomOpenGLViewClass() {
    Class existingClass = objc_getClass(kCustomOpenGLViewClass);
    if (existingClass) {
        return existingClass;
    }
    
    // Get NSOpenGLView class as superclass
    Class NSOpenGLViewClass = objc_getClass(kNSOpenGLViewClass);
    
    // Create a new class inheriting from NSOpenGLView
    Class CustomViewClass = objc_allocateClassPair(NSOpenGLViewClass, kCustomOpenGLViewClass, 0);
    
    if (!CustomViewClass) {
        return NULL;
    }
    
    // Keyboard event handler methods
    class_addMethod(CustomViewClass, ObjectiveCSEL::keyDownSel, (IMP)view_keyDown, kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::keyUpSel,   (IMP)view_keyUp,   kEventHandlerMethodTypeEncoding);
    
    // Flags Changed event handler
    class_addMethod(CustomViewClass, ObjectiveCSEL::flagsChangedSel, (IMP)view_flagsChanged, kEventHandlerMethodTypeEncoding);
    
    // Mouse event handler methods
    class_addMethod(CustomViewClass, ObjectiveCSEL::mouseDownSel,         (IMP)view_mouseDown,         kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::mouseUpSel,           (IMP)view_mouseUp,           kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::mouseDraggedSel,      (IMP)view_mouseDragged,      kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::mouseMovedSel,        (IMP)view_mouseMoved,        kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::rightMouseDownSel,    (IMP)view_rightMouseDown,    kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::rightMouseUpSel,      (IMP)view_rightMouseUp,      kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::rightMouseDraggedSel, (IMP)view_rightMouseDragged, kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::otherMouseDownSel,    (IMP)view_otherMouseDown,    kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::otherMouseUpSel,      (IMP)view_otherMouseUp,      kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::otherMouseDraggedSel, (IMP)view_otherMouseDragged, kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::scrollWheelSel,       (IMP)view_scrollWheel,       kEventHandlerMethodTypeEncoding);
    
    // Touch event handler methods
    class_addMethod(CustomViewClass, ObjectiveCSEL::touchesBeganSel,     (IMP)view_touchesBegan,     kTouchEventMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::touchesMovedSel,     (IMP)view_touchesMoved,     kTouchEventMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::touchesEndedSel,     (IMP)view_touchesEnded,     kTouchEventMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::touchesCancelledSel, (IMP)view_touchesCancelled, kTouchEventMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::pressureChangeSel, (IMP)view_pressureChange, kEventHandlerMethodTypeEncoding);
    
    // Tablet / stylus event handler methods
    class_addMethod(CustomViewClass, ObjectiveCSEL::tabletProximitySel, (IMP)view_tabletProximity, kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::tabletPointSel,     (IMP)view_tabletPoint,     kEventHandlerMethodTypeEncoding);
        
    // Area tracking for Mouse entered/exited event handler methods
    class_addMethod(CustomViewClass, ObjectiveCSEL::updateTrackingAreasSel, (IMP)view_updateTrackingAreas, kVoidMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::mouseEnteredSel,      (IMP)view_mouseEntered,      kEventHandlerMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::mouseExitedSel,       (IMP)view_mouseExited,       kEventHandlerMethodTypeEncoding);
    
    // First responder methods
    class_addMethod(CustomViewClass, ObjectiveCSEL::acceptsFirstResponderSel,  (IMP)view_acceptsFirstResponder, kBoolMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::becomeFirstResponderSel,   (IMP)view_becomeFirstResponder,  kBoolMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::canBecomeKeyViewSel,       (IMP)view_canBecomeKeyView,      kBoolMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::needsPanelToBecomeKeySel,  (IMP)view_needsPanelToBecomeKey, kBoolMethodTypeEncoding);

    // DrawRect method
    class_addMethod(CustomViewClass, ObjectiveCSEL::drawRectSel, (IMP)view_drawRect, kDrawRectMethodTypeEncoding);

    // Block automatic OpenGL operations on main thread
    class_addMethod(CustomViewClass, ObjectiveCSEL::reshapeSel, (IMP)view_reshape, kVoidMethodTypeEncoding);
    class_addMethod(CustomViewClass, ObjectiveCSEL::updateSel,  (IMP)view_update,  kVoidMethodTypeEncoding);

    // Register the class with the runtime
    objc_registerClassPair(CustomViewClass);
    
    return CustomViewClass;
}

// Forward declaration for C API function used by window event handlers
extern "C" void window_updateFrameFromOSX(Window* self);

// handle window resize events
void windowDidResize(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;

    if (gptrWindowDelegate && gptrWindowDelegate->nsWindow) {

        // Update frame data only
        window_updateFrameFromOSX(gptrWindowDelegate);

         // Get the new content view size
        double width, height;
        window_getContentViewFrame(gptrWindowDelegate, nullptr, nullptr, &width, &height);

        if (gptrWindowDelegate->windowDidResizeCallback) {
            gptrWindowDelegate->windowDidResizeCallback(gptrWindowDelegate->windowDidResizeUserData);
        }
        
    }
}

// handle window will close events
void windowWillClose(id self, SEL _cmd, id notification) {
   (void)self;(void)_cmd;(void)notification;
    dispatch_async(dispatch_get_main_queue(), ^{
       // Ensure all pending events are processed before closing the window
       gptrWindowDelegate->acceptsInputEvents = NO; // Stop accepting input events immediately to prevent processing events for a closing window
       gptrWindowDelegate->removeDelegate(gptrWindowDelegate); // remove delegate to ensure no more events are processed for this window
       if (gptrWindowDelegate && gptrWindowDelegate->windowWillCloseCallback) {
           gptrWindowDelegate->windowWillCloseCallback(gptrWindowDelegate->windowWillCloseUserData);
       }
   });
}

// handle window did become key events
void windowDidBecomeKey(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;
    if (gptrWindowDelegate && gptrWindowDelegate->windowDidBecomeKeyCallback) {
        gptrWindowDelegate->windowDidBecomeKeyCallback(gptrWindowDelegate->windowDidBecomeKeyUserData);
    }
}

// handle window did resign key events
void windowDidResignKey(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;
    if (gptrWindowDelegate && gptrWindowDelegate->windowDidResignKeyCallback) {
        gptrWindowDelegate->windowDidResignKeyCallback(gptrWindowDelegate->windowDidResignKeyUserData);
    }
}

// Handle window did miniaturize events
void windowDidMiniaturize(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;
    if (gptrWindowDelegate && gptrWindowDelegate->windowDidMiniaturizeCallback) {
        gptrWindowDelegate->windowDidMiniaturizeCallback(gptrWindowDelegate->windowDidMiniaturizeUserData);
    }
}

// Handle window did deminiaturize events
void windowDidDeminiaturize(id self, SEL _cmd, id notification) {
    (void)self;(void)_cmd;(void)notification;
    if (gptrWindowDelegate && gptrWindowDelegate->windowDidDeminiaturizeCallback) {
        gptrWindowDelegate->windowDidDeminiaturizeCallback(gptrWindowDelegate->windowDidDeminiaturizeUserData);
    }
}

// Create and configure window delegate
id createWindowDelegate(OpenGLRenderer* renderer) {
    // Get NSObject class as superclass using const string
    Class NSObjectClass = objc_getClass(kNSObjectClass);
    
    // Create a new class inheriting from NSObject using const string
    Class WindowDelegateClass = objc_allocateClassPair(NSObjectClass, kWindowDelegateClass, kZeroBytes);
    
    // Add window delegate methods
    class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidResizeSel,        (IMP)windowDidResize,        kEventHandlerMethodTypeEncoding);
    class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowWillCloseSel,        (IMP)windowWillClose,        kEventHandlerMethodTypeEncoding);
    class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidBecomeKeySel,     (IMP)windowDidBecomeKey,     kEventHandlerMethodTypeEncoding);
    class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidResignKeySel,     (IMP)windowDidResignKey,     kEventHandlerMethodTypeEncoding);
    class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidMiniaturizeSel,   (IMP)windowDidMiniaturize,   kEventHandlerMethodTypeEncoding);
    class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidDeminiaturizeSel, (IMP)windowDidDeminiaturize, kEventHandlerMethodTypeEncoding);

    // Register the class with the runtime
    objc_registerClassPair(WindowDelegateClass);
    
    // Create an instance
    id delegate  = ((id(*)(Class, SEL))objc_msgSend)(WindowDelegateClass, ObjectiveCSEL::allocSel);
    delegate     = ((id(*)(id, SEL))objc_msgSend)(delegate, ObjectiveCSEL::initSel);
    
    // Store renderer reference in delegate
    struct WindowDelegate* windowDelegate = (struct WindowDelegate*)delegate;
    windowDelegate->renderer = renderer;
    
    return delegate;
}

// Create a window delegate for general window events
id createWindowDelegateForWindow(void) {

    // Check if WindowDelegate class already exists to avoid duplicate registration
    static Class WindowDelegateClass = NULL;
    if (!WindowDelegateClass) {
        // Get NSObject class as superclass using const string
        Class NSObjectClass = objc_getClass(kNSObjectClass);
        
        // Create a new class inheriting from NSObject using const string
        WindowDelegateClass = objc_allocateClassPair(NSObjectClass, kGeneralWindowDelegateClass, kZeroBytes);

        if (!WindowDelegateClass) {
            return NULL;
        }

        // Window delegate methods
        class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidResizeSel,        (IMP)windowDidResize,        kEventHandlerMethodTypeEncoding);
        class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowWillCloseSel,        (IMP)windowWillClose,        kEventHandlerMethodTypeEncoding);
        class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidBecomeKeySel,     (IMP)windowDidBecomeKey,     kEventHandlerMethodTypeEncoding);
        class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidResignKeySel,     (IMP)windowDidResignKey,     kEventHandlerMethodTypeEncoding);
        class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidMiniaturizeSel,   (IMP)windowDidMiniaturize,   kEventHandlerMethodTypeEncoding);
        class_addMethod(WindowDelegateClass, ObjectiveCSEL::windowDidDeminiaturizeSel, (IMP)windowDidDeminiaturize, kEventHandlerMethodTypeEncoding);

        // Register the class with the runtime
        objc_registerClassPair(WindowDelegateClass);
    }
    
    // Create an instance
    id delegate  = ((id(*)(Class, SEL))objc_msgSend)(WindowDelegateClass, ObjectiveCSEL::allocSel);
    delegate     = ((id(*)(id, SEL))objc_msgSend)(delegate, ObjectiveCSEL::initSel);
    
    return delegate;
}

    // Forward declarations for C API functions used by internal helpers
    extern "C" void window_setDelegate(Window* self, id delegate);

extern "C" {
    // Initialize the application
    void application_initialize(Application* self) {

        // Initialize cached selectors for performance
        initialize_macos_api();
        
        // Set global application reference for delegate callbacks
        gptrApplicationDelegate = self;
        
        // Get NSApplication
        Class NSApplicationClass = objc_getClass(kNSApplicationClass);
        self->nsApp = ((id(*)(Class, SEL))objc_msgSend)(NSApplicationClass, ObjectiveCSEL::sharedApplicationSel);
        
        // Set activation policy to regular
        ((void(*)(id, SEL, NSInteger))objc_msgSend)(self->nsApp, ObjectiveCSEL::setActivationPolicySel, NSApplicationActivationPolicyRegular);
        
        // Create and set application delegate
        self->delegate = createApplicationDelegate();
        ((void(*)(id, SEL, id))objc_msgSend)(self->nsApp, ObjectiveCSEL::setDelegateSel, self->delegate);
    }

    // Activate the application
    void application_activate(Application* self) {
        ((void(*)(id, SEL, BOOL))objc_msgSend)(self->nsApp, ObjectiveCSEL::activateIgnoringOtherAppsSel, YES);
    }

    // Run the application event loop
    void application_run(Application* self) {
        ((void(*)(id, SEL))objc_msgSend)(self->nsApp, ObjectiveCSEL::runSel);
    }

    // Request to OS to gracefully terminate the application (RAII compatible)
    void application_stop(Application* self) {
        if (self && self->nsApp) {
            ((void(*)(id, SEL, id))objc_msgSend)(self->nsApp, ObjectiveCSEL::stopSel, self->nsApp);
        }
    }

    // Destroy the application
    void application_destroy(Application* self) {
        if (self) {
            delete self;
        }
    }

     // Get system locale identifier
    const char* application_getSystemLocale(Application* self) {
        (void)self;
        
        // Get NSLocale class
        Class NSLocaleClass = objc_getClass(kNSLocaleClass);
        
        // Get current locale
        id currentLocale = ((id(*)(Class, SEL))objc_msgSend)(NSLocaleClass, ObjectiveCSEL::currentLocaleSel);
        
        // Get locale identifier
        id localeIdentifierNS = ((id(*)(id, SEL))objc_msgSend)(currentLocale, ObjectiveCSEL::localeIdentifierSel);
        
        // Convert to C string (en-US, en-GB, en-IE etc)
        const char* localeIdentifier = ((const char*(*)(id, SEL))objc_msgSend)(localeIdentifierNS, ObjectiveCSEL::utf8StringSel);
        
        return localeIdentifier;
    }


    // Ensure window has a delegate for window events
    void ensureWindowDelegate(struct Window* self) {

        // If the windows is created but has no delegate, create and set one
        if (self && !self->delegate) {
            // Create a window delegate
            id delegate = createWindowDelegateForWindow();
            if (delegate) {
                // Set it on the window
                window_setDelegate(self, delegate);
            }
        }
    }

    // Application constructor
    Application* application_init() {

        ObjectiveCSEL::ensureInitialized(); // Ensure selectors are initialized

        Application* app = new Application();

        // Assign the method pointers
        app->initialize      = application_initialize;
        app->activate        = application_activate;
        app->run             = application_run;
        app->destroy         = application_destroy;
        app->getSystemLocale = application_getSystemLocale;

        return app;
    }

    // Create the NSWindow instance
    void window_create(Window* self, unsigned long styleMask) {
        ObjectiveCSEL::ensureInitialized(); // Ensure selectors are initialized

        // Get classes using const strings
        Class NSWindowClass = objc_getClass(kNSWindowClass);
        Class NSStringClass = objc_getClass(kNSStringClass);

        // Create window
        id windowAlloc = ((id(*)(Class, SEL))objc_msgSend)(NSWindowClass, ObjectiveCSEL::allocSel);
        
        self->nsWindow = ((id(*)(id, SEL, NSRect, unsigned long, unsigned long, BOOL))objc_msgSend)(
                        windowAlloc, ObjectiveCSEL::initWithContentRectSel, self->windowFrame, styleMask,
                        NSBackingStoreBuffered, NSWindowCreateNow); // Use modern buffered backing store, create immediately
        
        // Set window title using stored title or default
        const char* titleToUse = self->title ? self->title : kWindowTitle;
        id titleString = ((id(*)(Class, SEL, const char*))objc_msgSend)(
                        NSStringClass, ObjectiveCSEL::stringWithUTF8StringSel, titleToUse);
        ((void(*)(id, SEL, id))objc_msgSend)(self->nsWindow, ObjectiveCSEL::setTitleSel, titleString);

        // Set the content view frame
        NSRect contentFrame = {self->windowFrame.x, self->windowFrame.y, self->windowFrame.width, self->windowFrame.height};
        if (self->nsWindow) {
            id contentView = ((id(*)(id, SEL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::contentViewSel);
            if (contentView) {
                ((void(*)(id, SEL, NSRect))objc_msgSend)(contentView, ObjectiveCSEL::setFrameSel, contentFrame);
            }
        }

        window_getContentViewFrame(self, &self->contentViewFrame.x, &self->contentViewFrame.y,
                                   &self->contentViewFrame.width, &self->contentViewFrame.height);
    }

    // Show the window and set up event handling
    void window_show(Window* self) {

        // Show window
        ((void(*)(id, SEL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::orderFrontRegardlessSel);
        
        // Enable mouse moved events
        ((void(*)(id, SEL, BOOL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::setAcceptsMouseMovedEventsSel, YES);
        
        // Ensure window delegate is created and set
        ensureWindowDelegate(self);
        if (self->delegate) {
            ((void(*)(id, SEL, id))objc_msgSend)(self->nsWindow, ObjectiveCSEL::setDelegateSel, self->delegate);
        }
        
        // Make the window key to ensure it can receive keyboard events
        ((void(*)(id, SEL, id))objc_msgSend)(self->nsWindow, ObjectiveCSEL::makeKeyAndOrderFrontSel, NULL);
    }

    // Destroy the window
    void window_destroy(Window* self) {

        // Thanks Mr B!
        if (self) {
            self->acceptsInputEvents = NO;
            if (gptrNSWindowEvents == self) {
                gptrNSWindowEvents = NULL;
            }
            if (gptrWindowDelegate == self) {
                gptrWindowDelegate = NULL;
            }
        }
        // NSWindow will be cleaned up by autorelease pool
    }

    // Window delegate setter
    void window_setDelegate(Window* self, id delegate) {
        self->delegate = delegate;
        
        // Set global window reference for delegate callbacks
        gptrWindowDelegate = self;
        
        // Set the delegate on the actual NSWindow
        if (self->nsWindow && delegate) {
            ((void(*)(id, SEL, id))objc_msgSend)(self->nsWindow, ObjectiveCSEL::setDelegateSel, delegate);
        }
    }

    // Removes the delegate from the window and clears the reference to prevent accidental use after close
    void window_removeDelegate(Window* self) {
        
        if (self->nsWindow) {
            ((void(*)(id, SEL, id))objc_msgSend)(self->nsWindow, ObjectiveCSEL::setDelegateSel, nil);
        }
        self->delegate = NULL;
        
    }

    // Window title getter and setter
    const char* window_getTitle(const Window* self) {
        return self->title;
    }

    // Window title setter
    void window_setTitle(Window* self, const char* title) {
        self->title = title;

        if (self->nsWindow) {
            Class NSStringClass = objc_getClass(kNSStringClass);

            id titleString = ((id(*)(Class, SEL, const char*))objc_msgSend)(
                                NSStringClass, ObjectiveCSEL::stringWithUTF8StringSel, title);

            ((void(*)(id, SEL, id))objc_msgSend)(self->nsWindow, ObjectiveCSEL::setTitleSel, titleString);
        }
    }

    // Refresh internal frame representation from actual NSWindow (OSX)
    void window_updateFrameFromOSX(Window* self) {
       self->windowFrame = ((NSRect(*)(id, SEL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::frameSel);
    }

    // Get the content view size (excludes title bar and borders)
    void window_getContentViewFrame(const Window* self, double* x, double* y, double* width, double* height) {
        if (!self || !self->nsWindow) {
            if (x) *x = kMinValidDimension;
            if (y) *y = kMinValidDimension;
            if (width) *width = kDefaultWindowWidth;
            if (height) *height = kDefaultWindowHeight;
            return;
        }
        
        // Get the content view
        id contentView = ((id(*)(id, SEL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::contentViewSel);
        if (!contentView) {
            if (x) *x = kMinValidDimension;
            if (y) *y = kMinValidDimension;
            if (width) *width = kDefaultWindowWidth;
            if (height) *height = kDefaultWindowHeight;
            return;
        }
        
        // Thank you, Ben the ultimate Guru,
        // Get the content view bounds in points (logical coordinates).
        // On macOS, AppKit uses points rather than physical pixels. On HighDPI
        // displays (that are scaling) a point maps to >1 pixels.
        // convertRectToBacking: converts the point-based rect to physical pixels,
        // which is what OpenGL expects
        NSRect contentBounds = ((NSRect(*)(id, SEL))objc_msgSend)(contentView, ObjectiveCSEL::boundsSel);
        NSRect pixelBounds   = ((NSRect(*)(id, SEL, NSRect))objc_msgSend)(contentView, ObjectiveCSEL::convertRectToBackingSel, contentBounds);
        
        if (x) *x = pixelBounds.x;
        if (y) *y = pixelBounds.y;
        if (width) *width = pixelBounds.width;
        if (height) *height = pixelBounds.height;
        
    }

    // Get the content view size (excludes title bar and borders)
    void window_setContentViewFrame(struct Window* self, double* x, double* y, double* width, double* height) {
        if (!self || !self->nsWindow) {
            if (x) *x = kMinValidDimension;
            if (y) *y = kMinValidDimension;
            if (width) *width = kDefaultWindowWidth;
            if (height) *height = kDefaultWindowHeight;
            return;
        }

       // Set the content view frame
       NSRect contentFrame = {x ? *x : 0, y ? *y : 0, width ? *width : 0, height ? *height : 0};
       if (self->nsWindow) {
           id contentView = ((id(*)(id, SEL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::contentViewSel);
           if (contentView) {
               ((void(*)(id, SEL, NSRect))objc_msgSend)(contentView, ObjectiveCSEL::setFrameSel, contentFrame);
           }
       }
    }

    // Get Window frame (x, y, width, height)
    void window_getWindowFrame(const Window* self, double* x, double* y, double* width, double* height) {
        
        window_updateFrameFromOSX(const_cast<Window*>(self));

        auto posX = self ? self->windowFrame.x : kMinValidDimension;
        auto posY = self ? self->windowFrame.y : kMinValidDimension;
        auto w = self ? self->windowFrame.width : kDefaultWindowWidth;
        auto h = self ? self->windowFrame.height : kDefaultWindowHeight;
        if (x) *x = posX;
        if (y) *y = posY;
        if (width) *width = w;
        if (height) *height = h;
    }

    // Get window position (x, y)
    void window_getWindowPosition(const struct Window* self, double* x, double* y) {
        window_getWindowFrame(self, x, y, nullptr, nullptr);
    }

    // Get Window size (width, height)
    void window_getWindowSize(const Window* self, double* width, double* height) {
        window_getWindowFrame(self, nullptr, nullptr, width, height);
    }

    // Set window frame (x, y, width, height)
    void window_setWindowFrame(Window* self, double x, double y, double width, double height) {
        if (self) {
            // Update internal frame representation
            self->windowFrame.x      = x;
            self->windowFrame.y      = y;
            self->windowFrame.width  = width;
            self->windowFrame.height = height;

            // Create new NSRect for the frame
            NSRect newFrame = {x, y, width, height};
            
            // Set the frame on the actual NSWindow
            if (self->nsWindow) {
                ((void(*)(id, SEL, NSRect, BOOL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::setFrameDisplaySel, newFrame, YES);
            }
        }
    }

    // Set window position (x, y)
    void window_setWindowPosition(Window* self, double x, double y) {
        window_setWindowFrame(self, x, y, self->windowFrame.width, self->windowFrame.height);
    }

    // Set window size (width, height)
    void window_setWindowSize(Window* self, double width, double height) {
        window_setWindowFrame(self, self->windowFrame.x, self->windowFrame.y, width, height);
    }

    // Set the cusor position relative to the content view
    void window_setCursorPosition(Window* self, double x, double y) {
        if (!self || !self->nsWindow) return;
        
        // Get current screen the window is on
        id currentScreen = ((id(*)(id, SEL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::screenSel);
        NSRect screenFrame = ((NSRect(*)(id, SEL))objc_msgSend)(currentScreen, ObjectiveCSEL::frameSel);
        
        // Update internal frame representation to get the latest window position
        window_updateFrameFromOSX(self);
        NSPoint location = {self->windowFrame.x, self->windowFrame.y};
        
        // Get the content view
        id contentView = ((id(*)(id, SEL))objc_msgSend)(gptrNSWindowEvents->nsWindow, ObjectiveCSEL::contentViewSel);
        
        if (contentView) {
        
            // x,y are in physical backing pixels. CGWarpMouseCursorPosition
            // and all AppKit frame/bounds values use logical points, so convert the incoming
            // pixel position to points.
            NSPoint pixelPos  = { x, y };
            NSPoint pointPos  = ((NSPoint(*)(id, SEL, NSPoint))objc_msgSend)( contentView, ObjectiveCSEL::convertPointFromBackingSel, pixelPos);
            
            // Get content bounds in points for clamping and Y-flip
            NSRect contentBounds = ((NSRect(*)(id, SEL))objc_msgSend)(contentView, ObjectiveCSEL::boundsSel);
            
            // NOTE: clamp to keep the cursor within the window content area
            auto posX = std::clamp(location.x + pointPos.x, location.x, location.x + contentBounds.width);
            auto posY = std::clamp(screenFrame.height - location.y - contentBounds.height + pointPos.y,
                                                screenFrame.height - location.y - contentBounds.height,
                                                screenFrame.height - location.y);
            CGWarpMouseCursorPosition(CGPointMake(posX, posY));
            
        }
        
    }

    // Set cursor visibility
    void window_setCursorVisibility(Window* self, BOOL visible) {
        if (!self || !self->nsWindow) return;
        bHideCursor = !visible;
        if (bHideCursor && bAllowHideCursor) {
            ((void (*)(Class, SEL))objc_msgSend)(objc_getClass(kNSCursorClass), ObjectiveCSEL::hideSel);
        } else {
            ((void (*)(Class, SEL))objc_msgSend)(objc_getClass(kNSCursorClass), ObjectiveCSEL::unhideSel);
        }
    }

    void window_toggleFullScreen(Window* self) {
        // Toggle fullscreen
        ((void (*)(id, SEL, id))objc_msgSend)(self->nsWindow, ObjectiveCSEL::toggleFullScreenSel, nil);
    }

    bool window_isFullScreen(Window* self) {
        unsigned long mask = ((unsigned long (*)(id, SEL))objc_msgSend)(self->nsWindow, ObjectiveCSEL::styleMaskSel);
        return (mask & NSWindowStyleMaskFullScreen);
    }

    // Initialize OpenGL renderer
    void opengl_initialize(OpenGLRenderer* self, Window* window) {
        self->window = window;
        window->renderer = self;
        
        // Get classes using const strings
        Class NSOpenGLPixelFormatClass  = objc_getClass(kNSOpenGLPixelFormatClass);
        Class CustomOpenGLViewClass     = createCustomOpenGLViewClass(); // Use our custom class
        
        unsigned int sampleBuffers = (self->MSAA_Samples > 0) ? 1 : 0;
        std::vector<unsigned int> preferredSamples = {32, 16, 8, 4, 2, 0};
        if(self->MSAA_Samples <= 0) {
            // If no multisampling requested, only try 0 samples
            preferredSamples.clear();
            preferredSamples.resize(1);
            preferredSamples.push_back(0);
        }

        for(unsigned int samples: preferredSamples)
        {
            // Create pixel format attributes array using enum values
            const unsigned int attrs[] = {
                NSOpenGLPFADoubleBuffer,                                        // Enable double buffering
                NSOpenGLPFADepthSize,        32,                                // 32-bit depth buffer
                NSOpenGLPFAColorSize,        24,                                // 24-bit color
                NSOpenGLPFAAccelerated,                                         // Hardware acceleration
                NSOpenGLAllowOfflineRenderers,                                  // Allow offline renderers
                NSOpenGLPFAOpenGLProfile,    NSOpenGLProfileVersion4_1Core,     // OpenGL 4.1 Core Profile
                NSOpenGLPFAMultisample,      samples,                           // 0x --> 32X Multisampling
                NSOpenGLPFASampleBuffers,    sampleBuffers,                     // Number of sample buffers
                0                                                               // null terminator
            };
            
            // Create pixel format
            self->pixelFormat = ((id(*)(id, SEL, const unsigned int*))objc_msgSend)(
                ((id(*)(Class, SEL))objc_msgSend)(NSOpenGLPixelFormatClass, ObjectiveCSEL::allocSel),
                ObjectiveCSEL::initWithAttributesSel, attrs);
            
            // Check if pixel format was created successfully
            if (self->pixelFormat) {
                self->MSAA_Samples = samples;
                break; // Successfully created pixel format
            }
        }
        
        // Create custom OpenGL view with event handling
        NSRect glViewFrame = {0.0, 0.0, window->contentViewFrame.width, window->contentViewFrame.height};
        self->glView = ((id(*)(id, SEL, NSRect, id))objc_msgSend)(
            ((id(*)(Class, SEL))objc_msgSend)(CustomOpenGLViewClass, ObjectiveCSEL::allocSel),
            ObjectiveCSEL::initWithFramePixelFormatSel, glViewFrame, self->pixelFormat);
        
        // Set autoresizing mask to make the view resize with the window
        unsigned int autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        ((void(*)(id, SEL, unsigned int))objc_msgSend)(self->glView, ObjectiveCSEL::setAutoresizingMaskSel, autoresizingMask);
        
        // Enable touch events (NSTouch)
        ((void(*)(id, SEL, BOOL))objc_msgSend)(self->glView, ObjectiveCSEL::setAcceptsTouchEventsSel, YES);
        
        // Enable Listening for fingers that are resting but not moving (Ensure second touch point is detected as a touch not right click)
        ((void(*)(id, SEL, BOOL))objc_msgSend)(self->glView, ObjectiveCSEL::setWantsRestingTouchesSel, YES);
        
        
    }

    // Set up OpenGL context and make view first responder
    void opengl_setupContext(OpenGLRenderer* self) {
        // Set the OpenGL view as the window's content view
        ((void(*)(id, SEL, id))objc_msgSend)(self->window->nsWindow, ObjectiveCSEL::setContentViewSel, self->glView);
        
        // Make the OpenGL context current
        self->glContext = ((id(*)(id, SEL))objc_msgSend)(self->glView, ObjectiveCSEL::openGLContextSel);
        ((void(*)(id, SEL))objc_msgSend)(self->glContext, ObjectiveCSEL::makeCurrentContextSel);
        
        // Make sure the window is key first
        ((void(*)(id, SEL))objc_msgSend)(self->window->nsWindow, ObjectiveCSEL::makeKeyWindowSel);
        
        // Make the OpenGL view the first responder so it can receive keyboard and mouse events
        ((BOOL(*)(id, SEL, id))objc_msgSend)(self->window->nsWindow, ObjectiveCSEL::makeFirstResponderSel, self->glView);
        
        // Additional debug: check if view can become key view
        ((BOOL(*)(id, SEL))objc_msgSend)(self->glView, ObjectiveCSEL::canBecomeKeyViewSel);
    }

    // Implementation function
    bool opengl_resetContextForSize(OpenGLRenderer* self, double width, double height) {
        if (!self || !self->glContext || !self->glView) {
            return false;
        }
        
        // Make context current
        ((void(*)(id, SEL))objc_msgSend)(self->glContext, ObjectiveCSEL::makeCurrentContextSel);
        
        // Update the view frame Without folks like Mr B the Ultimate Teacher, this would have been tricky
        NSRect pixelFrame = {0.0, 0.0, width, height};
        NSRect newFrame = ((NSRect(*)(id, SEL, NSRect))objc_msgSend)(self->glView, ObjectiveCSEL::convertRectFromBackingSel, pixelFrame);
        ((void(*)(id, SEL, NSRect))objc_msgSend)(self->glView, ObjectiveCSEL::setFrameSel, newFrame);
        
        // Force context to reshape
        ((void(*)(id, SEL))objc_msgSend)(self->glView, ObjectiveCSEL::reshapeSel);
        ((void(*)(id, SEL))objc_msgSend)(self->glContext, ObjectiveCSEL::updateSel);
        
        // Update viewport
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
        
        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        return true;
    }


    // Get the underlying OpenGL context
    void* opengl_getOpenGLContext(const struct OpenGLRenderer* self) {
            return (void*)self->glContext;
    }

    // Get the underlying CGLContextObj from the OpenGL renderer
    void* opengl_getCGLContextObj(struct OpenGLRenderer* self) {
        if (!self || !self->glContext) { return nullptr;}
        
        return (void*)((void*(*)(id, SEL))objc_msgSend)(self->glContext, ObjectiveCSEL::CGLContextObjSel);
    }
    
    // Get the underlying CGLContextObj (C API version)
    void* opengl_getCGLContextObjPtr(struct OpenGLRenderer* self) {
        return (void*)opengl_getCGLContextObj(self);
    }

    // Make the OpenGL context current
    void opengl_makeCurrentContext(struct OpenGLRenderer* self) {
        if (self && self->glContext) {
            ((void(*)(id, SEL))objc_msgSend)(self->glContext, ObjectiveCSEL::makeCurrentContextSel);
        }
    }

    // Enable or disable vertical synchronization (vsync)
    void opengl_setVsync(OpenGLRenderer* self, BOOL enabled) {
        if (!self || !self->glContext) { return; }
        
        // Make context current first
        ((void(*)(id, SEL))objc_msgSend)(self->glContext, ObjectiveCSEL::makeCurrentContextSel);
        
        // Set swap interval: 1 for vsync enabled, 0 for disabled
        GLint swapInterval = enabled ? 1 : 0;
        
        // Use NSOpenGLContext setValues:forParameter: to set swap interval
        const GLint parameter = NSOpenGLContextParameterSwapInterval;
        ((void(*)(id, SEL, const GLint*, GLint))objc_msgSend)(self->glContext, ObjectiveCSEL::setValuesSel, &swapInterval, parameter);
    }

    // Destroy the OpenGL renderer
    void opengl_destroy(OpenGLRenderer* self) {
        if (self) {
            // Release OpenGL resources
            if (self->glContext) {
                ((void(*)(id, SEL))objc_msgSend)(self->glContext, ObjectiveCSEL::releaseSel);
            }
            free(self);
        }
    }

    // OpenGL renderer constructor
    OpenGLRenderer* opengl_init() {

        OpenGLRenderer* renderer = (OpenGLRenderer*)malloc(sizeof(OpenGLRenderer));
        renderer->pixelFormat = NULL;
        renderer->glView      = NULL;
        renderer->glContext   = NULL;
        renderer->window      = NULL;

        // Assign method pointers
        renderer->initialize             = opengl_initialize;
        renderer->setupContext           = opengl_setupContext;
        renderer->getOpenGLContext       = opengl_getOpenGLContext;
        renderer->getCGLContextObj       = opengl_getCGLContextObj;
        renderer->getCGLContextObjPtr    = opengl_getCGLContextObjPtr;
        renderer->makeCurrentContext     = opengl_makeCurrentContext;
        renderer->setVsync               = opengl_setVsync;
        renderer->destroy                = opengl_destroy;
        renderer->resetContextForSize    = opengl_resetContextForSize;

        return renderer;
    }

    static BOOL imageloader_decodeImage(struct ImageLoader* self, CGImageRef image)
    {
        if(!image) return NO;

        // Clear any existing data
        if (self->pixelData) {
            free(self->pixelData);
            self->pixelData = NULL;
        }

        CGDataProviderRef provider = CGImageGetDataProvider(image);
        CFDataRef rawData = CGDataProviderCopyData(provider);

        //CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(image);                               // Temp remove unused variable warning
        //CGImageAlphaInfo alphaInfo = (CGImageAlphaInfo)(bitmapInfo & kCGBitmapAlphaInfoMask); // Temp remove unused variable warning
        //CGBitmapInfo byteOrder = bitmapInfo & kCGBitmapByteOrderMask;                         // Temp remove unused variable warning
        
        self->width         = (int)CGImageGetWidth(image); 
        self->height        = (int)CGImageGetHeight(image);
        self->bytesPerPixel = kRGBABytesPerPixel; // 4 bytes for RGBA
        self->bytesPerRow   = self->width * self->bytesPerPixel;
        self->hasAlpha      = YES;

        const uint8_t* imageData = CFDataGetBytePtr(rawData);
        self->pixelData = (imageloader_pixel_t*)malloc(self->width * self->height * self->bytesPerPixel);
        if(!self->pixelData)
        {
            return NO;
        }
        memcpy(self->pixelData, imageData, self->width * self->height * self->bytesPerPixel);
        
        // NOTE from Moros1138
        // 
        // On Apple Silicon and x86 Macs kCGBitmapByteOrder32Little is by far
        // the most common case, so in practice this block of code will never
        // be run. However, if we find that there is a need to adjust the
        // pixel data, this will need fleshing out.

        /*
        if(byteOrder != kCGBitmapByteOrder32Little)
        {
            int pixelCount = self->width * self->height;
            for(int i = 0; i < pixelCount; ++i)
            {
                auto p = self->pixelData[i];
                
                // TODO: detect byte order, adjust as appropriate
                
                self->pixelData[i] = p;
            }
        }
        */

        CFRelease(rawData);
        CGImageRelease(image);

        return YES;
    }

    BOOL imageloader_loadFromFile(struct ImageLoader* self, const char* filePath) {
        // Clear any existing data
        if (self->pixelData) {
            free(self->pixelData);
            self->pixelData = NULL;
        }

        CFStringRef pathStr = CFStringCreateWithCString(nullptr, filePath, kCFStringEncodingUTF8);
        CFURLRef    url     = CFURLCreateWithFileSystemPath(nullptr, pathStr, kCFURLPOSIXPathStyle, false);
        CFRelease(pathStr);

        if(!url)
        {
            printf("loadImageFromFile: bad path '%s'\n", filePath);
            return NO;
        }

        CGImageSourceRef src = CGImageSourceCreateWithURL(url, nullptr);
        CFRelease(url);

        if(!src)
        {
            printf("loadImageFromFile: couldn't open '%s'\n", filePath);
            return NO;
        }
        
        CGImageRef image = CGImageSourceCreateImageAtIndex(src, 0, nullptr);
        CFRelease(src);

        return imageloader_decodeImage(self, image);
    }

    BOOL imageloader_loadFromMemory(struct ImageLoader* self, const uint8_t* data, size_t bytes) {
        
        CFDataRef cfData = CFDataCreateWithBytesNoCopy(
            nullptr,
            reinterpret_cast<const UInt8*>(data),
            (CFIndex)bytes,
            kCFAllocatorNull          // we own the buffer, CF must not free it
        );

        if (!cfData)
        {
            printf("loadImageFromMemory: CFData creation failed\n");
            return NO;
        }

        CGImageSourceRef src = CGImageSourceCreateWithData(cfData, nullptr);
        CFRelease(cfData);

        if (!src)
        {
            printf("loadImageFromMemory: src creation failed\n");
            return NO;
        }

        CGImageRef image = CGImageSourceCreateImageAtIndex(src, 0, nullptr);
        CFRelease(src);        
        
        return imageloader_decodeImage(self, image);
    }

    // Get raw pixel data pointer
    unsigned char* imageloader_getPixelData(const struct ImageLoader* self) {
        return (unsigned char*)self->pixelData;
    }

    // Get image information
    void imageloader_getImageInfo(const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel) {
        if (width) *width                 = self->width;
        if (height) *height               = self->height;
        if (bytesPerPixel) *bytesPerPixel = self->bytesPerPixel;
    }

    // Get detailed image format information
    void imageloader_getDetailedInfo(const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel, int* bytesPerRow, BOOL* hasAlpha) {
        if (width) *width                 = self->width;
        if (height) *height               = self->height;
        if (bytesPerPixel) *bytesPerPixel = self->bytesPerPixel;
        if (bytesPerRow) *bytesPerRow     = self->bytesPerRow;
        if (hasAlpha) *hasAlpha           = self->hasAlpha;
    }

    // Check if image is loaded
    BOOL imageloader_isLoaded(const struct ImageLoader* self) {
        return (self->pixelData != NULL && self->width > kMinValidDimension && self->height > kMinValidDimension);
    }

    // Get pixel at specific coordinates
    BOOL imageloader_getPixel(const struct ImageLoader* self, int x, int y,
                            unsigned char* red, unsigned char* green,
                            unsigned char* blue, unsigned char* alpha) {
        if (!imageloader_isLoaded(self) || x < kMinValidDimension || x >= self->width || y < kMinValidDimension || y >= self->height) {
            return NO;
        }
        
        // Calculate pixel offset (macOS uses bottom-left origin, so flip Y)
        int flippedY = self->height - kFlippedOffset - y;
        imageloader_pixel_t* pixel = self->pixelData + (flippedY * self->width) + x;

        // Extract color components (assuming RGBA or RGB format)
        if (red) *red     = pixel->r;
        if (green) *green = pixel->g;
        if (blue) *blue   = pixel->b;
        if (alpha && self->bytesPerPixel >= kRGBABytesPerPixel)  *alpha = pixel->a;
        else if (alpha) *alpha = kFullyOpaque; // Fully opaque if no alpha channel
        
        return YES;
    }

    // Cleanup ImageLoader resources
    void imageloader_destroy(struct ImageLoader* self) {
        if (self->pixelData) {
            free(self->pixelData);
            self->pixelData = NULL;
        }
        self->width           = kZeroWidth;
        self->height          = kZeroHeight;
        self->bytesPerPixel   = kZeroBytes;
        self->bytesPerRow     = kZeroRows;
        self->hasAlpha        = NO;
    }
 
    // ImageLoader constructor
    struct ImageLoader* imageloader_init() {
        struct ImageLoader* loader = (struct ImageLoader*)malloc(sizeof(struct ImageLoader));
        
        // Initialize properties
        loader->pixelData       = NULL;
        loader->width           = kZeroWidth;
        loader->height          = kZeroHeight;
        loader->bytesPerPixel   = kZeroBytes;
        loader->bytesPerRow     = kZeroRows;
        loader->hasAlpha        = NO;
        
        // Assign method pointers
        loader->loadFromFile        = imageloader_loadFromFile;
        loader->loadFromMemory      = imageloader_loadFromMemory;
        loader->destroy             = imageloader_destroy;
        loader->getPixelData        = imageloader_getPixelData;
        loader->getImageInfo        = imageloader_getImageInfo;
        loader->getDetailedInfo     = imageloader_getDetailedInfo;
        loader->isLoaded            = imageloader_isLoaded;
        loader->getPixel            = imageloader_getPixel;
        
        return loader;
    }

    // Window constructor
    Window* window_init(double x, double y, double width, double height) {
        Window* window = (Window*)malloc(sizeof(Window));
        
        // Initialize window properties
        window->nsWindow    = NULL;
        window->delegate    = NULL;
        window->title       = NULL;
        
        // Set initial window frame
        window->windowFrame.x      = x;
        window->windowFrame.y      = y;
        window->windowFrame.width  = width;
        window->windowFrame.height = height;

        // Set window content view frame
        window->contentViewFrame.x      = x;
        window->contentViewFrame.y      = y;
        window->contentViewFrame.width  = width;
        window->contentViewFrame.height = height;

        // Initialize callback pointers
        window->keyDownCallback         = NULL;
        window->keyUpCallback           = NULL;
        window->mouseDownCallback       = NULL;
        window->mouseUpCallback         = NULL;
        window->mouseMovedCallback      = NULL;
        window->mouseDraggedCallback    = NULL;
        window->rightMouseDownCallback  = NULL;
        window->rightMouseUpCallback    = NULL;
        window->otherMouseDownCallback  = NULL;
        window->otherMouseUpCallback    = NULL;
        window->scrollWheelCallback     = NULL;

        // Event handling disabled by default
        window->acceptsInputEvents = NO;

        // Initialize window delegate callback pointers
        window->windowDidResizeCallback         = NULL;
        window->windowWillCloseCallback         = NULL;
        window->windowDidBecomeKeyCallback      = NULL;
        window->windowDidResignKeyCallback      = NULL;
        window->windowDidMiniaturizeCallback    = NULL;
        window->windowDidDeminiaturizeCallback  = NULL;
        window->windowDidResizeUserData         = NULL;
        window->windowWillCloseUserData         = NULL;
        window->windowDidBecomeKeyUserData      = NULL;
        window->windowDidResignKeyUserData      = NULL;
        window->windowDidMiniaturizeUserData    = NULL;
        window->windowDidDeminiaturizeUserData  = NULL;

        // Assign method pointers
        window->create            = window_create;
        window->show              = window_show;
        window->destroy           = window_destroy;
        window->setDelegate       = window_setDelegate;
        window->removeDelegate    = window_removeDelegate;
        window->getTitle          = window_getTitle;
        window->setTitle          = window_setTitle;
        window->setWindowSize     = window_setWindowSize;
        window->getWindowSize     = window_getWindowSize;
        window->setWindowPosition = window_setWindowPosition;
        window->getWindowPosition = window_getWindowPosition;
        window->setWindowFrame    = window_setWindowFrame;
        window->getWindowFrame    = window_getWindowFrame;
        window->getCurrentFrame   = window_updateFrameFromOSX;
        window->getContentViewFrame = window_getContentViewFrame;
        window->setContentViewFrame = window_setContentViewFrame;

        return window;
    }

    // Event callback setter functions
    void window_setKeyDownCallback(Window* self, void (*callback)(unsigned short, const char*, unsigned int, void*), void* userData) {
        self->keyDownCallback = callback;
        self->eventUserData = userData;
    }

    void window_setKeyUpCallback(Window* self, void (*callback)(unsigned short, const char*, unsigned int, void*), void* userData) {
        self->keyUpCallback = callback;
        self->eventUserData = userData;
    }
    
    void window_setFlagsChangedCallback(Window* self, void (*callback)(unsigned int, void*), void* userData) {
        self->flagsChangedCallback = callback;
        self->eventUserData = userData;
    }

    void window_setMouseDownCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->mouseDownCallback = callback;
        self->eventUserData = userData;
    }

    void window_setMouseUpCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->mouseUpCallback = callback;
        self->eventUserData = userData;
    }

    void window_setMouseMovedCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->mouseMovedCallback = callback;
        self->eventUserData = userData;
    }

    void window_setMouseDraggedCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->mouseDraggedCallback = callback;
        self->eventUserData = userData;
    }

    void window_setRightMouseDownCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->rightMouseDownCallback = callback;
        self->eventUserData = userData;
    }

    void window_setRightMouseDraggedCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->rightMouseDraggedCallback = callback;
        self->eventUserData = userData;
    }

    void window_setRightMouseUpCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->rightMouseUpCallback = callback;
        self->eventUserData = userData;
    }

    void window_setOtherMouseDownCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->otherMouseDownCallback = callback;
        self->eventUserData = userData;
    }

    void window_setOtherMouseUpCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->otherMouseUpCallback = callback;
        self->eventUserData = userData;
    }

    void window_setOtherMouseDraggedCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->otherMouseDraggedCallback = callback;
        self->eventUserData = userData;
    }

    void window_setScrollWheelCallback(Window* self, void (*callback)(double, double, double, double, unsigned int, void*), void* userData) {
        self->scrollWheelCallback = callback;
        self->eventUserData = userData;
    }

    void window_setMouseEnteredCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->mouseEnteredCallback = callback;
        self->eventUserData = userData;
    }

    void window_setMouseExitedCallback(Window* self, void (*callback)(double, double, int, unsigned int, void*), void* userData) {
        self->mouseExitedCallback = callback;
        self->eventUserData = userData;
    }

    void window_setTouchBeganCallback(Window* self, void (*callback)(uint32_t, double, double, double, double, void*), void* userData) {
        self->touchBeganCallback = callback;
        self->touchUserData = userData;
    }

    void window_setTouchMovedCallback(Window* self, void (*callback)(uint32_t, double, double, double, double, void*), void* userData) {
        self->touchMovedCallback = callback;
        self->touchUserData = userData;
    }

    void window_setTouchEndedCallback(Window* self, void (*callback)(uint32_t, double, double, double, double, void*), void* userData) {
        self->touchEndedCallback = callback;
        self->touchUserData = userData;
    }

    void window_setTouchCancelledCallback(Window* self, void (*callback)(uint32_t, double, double, double, double, void*), void* userData) {
        self->touchCancelledCallback = callback;
        self->touchUserData = userData;
    }

    void window_setStylusCallback(Window* self,
        void (*callback)(uint32_t, double, double, float, float, float, float, bool, bool, bool, void*),
        void* userData)
    {
        self->stylusCallback = callback;
        self->stylusUserData = userData;
    }

    void window_enableEventHandling(Window* self) {
        if (self) {
            self->acceptsInputEvents = YES;
            gptrNSWindowEvents = self; // Set global reference for event handlers
        }
    }

    void window_disableEventHandling(Window* self) {
        if (self) {
            self->acceptsInputEvents = NO;
            if (gptrNSWindowEvents == self) {
                gptrNSWindowEvents = NULL;
            }
        }
    }

    // Window delegate callback setter functions
    void window_setWindowDidResizeCallback(Window* self, void (*callback)(void*), void* userData) {
        self->windowDidResizeCallback = callback;
        self->windowDidResizeUserData = userData;

    }

    void window_setWindowWillCloseCallback(Window* self, void (*callback)(void*), void* userData) {
        self->windowWillCloseCallback = callback;
        self->windowWillCloseUserData = userData;
    }

    void window_setWindowDidBecomeKeyCallback(Window* self, void (*callback)(void*), void* userData) {
        self->windowDidBecomeKeyCallback = callback;
        self->windowDidBecomeKeyUserData = userData;
    }

    void window_setWindowDidResignKeyCallback(Window* self, void (*callback)(void*), void* userData) {
        self->windowDidResignKeyCallback = callback;
        self->windowDidResignKeyUserData = userData;
    }

    void window_setWindowDidMiniaturizeCallback(Window* self, void (*callback)(void*), void* userData) {
        self->windowDidMiniaturizeCallback = callback;
        self->windowDidMiniaturizeUserData = userData;
    }

    void window_setWindowDidDeminiaturizeCallback(Window* self, void (*callback)(void*), void* userData) {
        self->windowDidDeminiaturizeCallback = callback;
        self->windowDidDeminiaturizeUserData = userData;
    }

    // Application delegate callback setter functions
    void application_setWillFinishLaunchingCallback(Application* self, void (*callback)(void*), void* userData) {
        self->willFinishLaunchingCallback = callback;
        self->willFinishLaunchingUserData = userData;
    }

    void application_setDidFinishLaunchingCallback(Application* self, void (*callback)(void*), void* userData) {
        self->didFinishLaunchingCallback = callback;
        self->didFinishLaunchingUserData = userData;
    }

    void application_setWillTerminateCallback(Application* self, void (*callback)(void*), void* userData) {
        self->willTerminateCallback = callback;
        self->willTerminateUserData = userData;
    }

    void application_setDidBecomeActiveCallback(Application* self, void (*callback)(void*), void* userData) {
        self->didBecomeActiveCallback = callback;
        self->didBecomeActiveUserData = userData;
    }

    void application_setWillResignActiveCallback(Application* self, void (*callback)(void*), void* userData) {
        self->willResignActiveCallback = callback;
        self->willResignActiveUserData = userData;
    }

} // extern "C"

//! END IMPLEMENTATION

