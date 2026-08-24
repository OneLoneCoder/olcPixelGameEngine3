#include "api_ios.h"

//! START IMPLEMENTATION

// UIApplication and related class names
static constexpr const char* kNSAutoreleasePoolClass            = "NSAutoreleasePool";
static constexpr const char* kUIApplicationClass                = "UIApplication";
static constexpr const char* kUIViewControllerClass             = "UIViewController";
static constexpr const char* kSetRootViewControllerSel          = "setRootViewController:";
static constexpr const char* kUIViewClass                       = "UIView";
static constexpr const char* kUIWindowClass                     = "UIWindow";
static constexpr const char* kUIScreenClass                     = "UIScreen";
static constexpr const char* kNSStringClass                     = "NSString";
static constexpr const char* kEAGLContextClass                  = "EAGLContext";
static constexpr const char* kGLKViewClass                      = "GLKView";
static constexpr const char* kGLKViewControllerClass            = "GLKViewController";
static constexpr const char* kNSObjectClass                     = "NSObject";
static constexpr const char* kUIImageClass                      = "UIImage";
static constexpr const char* kAppDelegateClass                  = "AppDelegate";            // This is the OS default AppDelegate class name
static constexpr const char* kolcPGE3AppDelegateClass           = "olcPGE3AppDelegate";     // Important: Custom AppDelegate subclass name
static constexpr const char* kViewControllerDelegateClass       = "ViewControllerDelegate";
static constexpr const char* kCustomGLKViewClass                = "CustomGLKView";
static constexpr const char* kNSBundleClass                     = "NSBundle";               // Class for handling app bundles i.e. getting app resource paths

// UIWindow and UIScreen selectors
static constexpr const char* kmainScreenSel                     = "mainScreen";
static constexpr const char* kBoundsSelScreen                   = "bounds";
static constexpr const char* kNativeBounds                      = "nativeBounds";
static constexpr const char* kScaleSelScreen                    = "scale";
static constexpr const char* kInitWithFrameSel                  = "initWithFrame:";
static constexpr const char* kMakeKeyAndVisibleSel              = "makeKeyAndVisible";
static constexpr const char* kMaximumFramesPerSecond            = "maximumFramesPerSecond";
static constexpr const char* kSetAutoresizingMaskSel            = "setAutoresizingMask:";

// Memory management selectors
static constexpr const char* kAllocSel                          = "alloc";
static constexpr const char* kInitSel                           = "init";
static constexpr const char* kDrainSel                          = "drain";
static constexpr const char* kSetDelegateSel                    = "setDelegate:";
static constexpr const char* kReleaseSel                        = "release";
static constexpr const char* kRetainSel                         = "retain";
static constexpr const char* kIsKindOfClassSel                  = "isKindOfClass:";

// UIApplication lifecycle and management selectors
static constexpr const char* kSharedApplicationSel              = "sharedApplication";
static constexpr const char* kRunSel                            = "run";
static constexpr const char* kSetIdleTimerDisabledSel           = "setIdleTimerDisabled:";
static constexpr const char* kSetPausedSel                      = "setPaused:";

// UIApplicationDelegate lifecycle methods
static constexpr const char* kApplicationDidFinishLaunchingSel  = "application:didFinishLaunchingWithOptions:";
static constexpr const char* kApplicationWillTerminateSel       = "applicationWillTerminate:";
static constexpr const char* kApplicationDidBecomeActiveSel     = "applicationDidBecomeActive:";
static constexpr const char* kApplicationWillResignActiveSel    = "applicationWillResignActive:";
static constexpr const char* kApplicationDidEnterBackgroundSel  = "applicationDidEnterBackground:";
static constexpr const char* kApplicationWillEnterForegroundSel = "applicationWillEnterForeground:";

// UIViewController creation, display, and management selectors
static constexpr const char* kInitWithNibNameSel                = "initWithNibName:bundle:";
static constexpr const char* kStringWithUTF8StringSel           = "stringWithUTF8String:";
static constexpr const char* kSetTitleSel                       = "setTitle:";
static constexpr const char* kViewSel                           = "view";
static constexpr const char* kSetViewSel                        = "setView:";
static constexpr const char* kPresentViewControllerSel          = "presentViewController:animated:completion:";
static constexpr const char* kDismissViewControllerSel          = "dismissViewControllerAnimated:completion:";
static constexpr const char* kFrameSel                          = "frame";
static constexpr const char* kSetFrameSel                       = "setFrame:";
static constexpr const char* kBoundsSel                         = "bounds";
static constexpr const char* kSetBoundsSel                      = "setBounds:";
static constexpr const char* kUpdateView                        = "update";

// GLKViewController lifecycle methods
static constexpr const char* kViewDidLoadSel                    = "viewDidLoad";
static constexpr const char* kLoadViewSel                       = "loadView";
static constexpr const char* kViewWillAppearSel                 = "viewWillAppear:";
static constexpr const char* kViewDidAppearSel                  = "viewDidAppear:";
static constexpr const char* kViewWillDisappearSel              = "viewWillDisappear:";
static constexpr const char* kViewDidDisappearSel               = "viewDidDisappear:";
static constexpr const char* kViewDidLayoutSubviewsSel          = "viewDidLayoutSubviews";

// Touch event methods selectors
static constexpr const char* kTouchesBeganSel                   = "touchesBegan:withEvent:";
static constexpr const char* kTouchesMovedSel                   = "touchesMoved:withEvent:";
static constexpr const char* kTouchesEndedSel                   = "touchesEnded:withEvent:";
static constexpr const char* kTouchesCancelledSel               = "touchesCancelled:withEvent:";
static constexpr const char* kAllObjectsSel                     = "allObjects";
static constexpr const char* kMajorRadius                       = "majorRadius";

// GLKView drawing methods selectors
static constexpr const char* kDrawRectSel                       = "drawRect:";

// Keyboard event methods selectors (for hardware keyboards)
static constexpr const char* kPressesBeganSel                   = "pressesBegan:withEvent:";
static constexpr const char* kPressesEndedSel                   = "pressesEnded:withEvent:";
static constexpr const char* kKeySel                            = "key";
static constexpr const char* kKeyCodeSel                        = "keyCode";
static constexpr const char* kCharactersSel                     = "characters";
static constexpr const char* kModifierFlagsSel                  = "modifierFlags";

// Touch and press event property selectors
static constexpr const char* kLocationInViewSel                 = "locationInView:";
static constexpr const char* kAllTouchesSel                     = "allTouches";
static constexpr const char* kCountSel                          = "count";
static constexpr const char* kObjectAtIndexSel                  = "objectAtIndex:";


// EAGL and OpenGL ES management selectors
static constexpr const char* kInitWithAPISel                    = "initWithAPI:";
static constexpr const char* kSetCurrentContextSel              = "setCurrentContext:";
static constexpr const char* kContextSel                        = "context";
static constexpr const char* kSetContextSel                     = "setContext:";
static constexpr const char* kDrawableColorFormatSel            = "drawableColorFormat";
static constexpr const char* kSetDrawableColorFormatSel         = "setDrawableColorFormat:";
static constexpr const char* kDrawableDepthFormatSel            = "drawableDepthFormat";
static constexpr const char* kSetDrawableDepthFormatSel         = "setDrawableDepthFormat:";
static constexpr const char* kDrawableMultisampleSel            = "drawableMultisample";
static constexpr const char* kSetDrawableMultisampleSel         = "setDrawableMultisample:";
static constexpr const char* kSetOpaqueSel                      = "setOpaque:";
static constexpr const char* kSetContentScaleFactorSel          = "setContentScaleFactor:";
static constexpr const char* kContentScaleFactorSel             = "contentScaleFactor";
static constexpr const char* kSetEnableSetNeedsDisplaySel       = "setEnableSetNeedsDisplay:";
static constexpr const char* kSetAutoResizeDrawableSel          = "setAutoResizeDrawable:";
static constexpr const char* kSetPreferredFramesPerSecondSel    = "setPreferredFramesPerSecond:";
static constexpr const char* kBindDrawableSel                   = "bindDrawable";
static constexpr const char* kDisplaySel                        = "display";
static constexpr const char* kPresentRenderbufferSel            = "presentRenderbuffer:";
static constexpr const char* kSetNeedsDisplaySel                = "setNeedsDisplay";

// Device and orientation selectors
static constexpr const char* kCurrentDeviceSel                  = "currentDevice";
static constexpr const char* kOrientationSel                    = "orientation";
static constexpr const char* kIsMultitouchEnabledSel            = "isMultipleTouchEnabled";
static constexpr const char* kSetMultipleTouchEnabledSel        = "setMultipleTouchEnabled:";
static constexpr const char* kSetUserInteractionEnabledSel      = "setUserInteractionEnabled:";
static constexpr const char* kIsUserInteractionEnabledSel       = "isUserInteractionEnabled";
static constexpr const char* kDeviceOrientationDidChangeSel     = "deviceOrientationDidChange:";
static constexpr const char* kBeginGenDeviceOrientationNotifSel = "beginGeneratingDeviceOrientationNotifications";

// Notification classes and selectors
static constexpr const char* kNSNotificationCenterClass         = "NSNotificationCenter";
static constexpr const char* kDefaultCenterSel                  = "defaultCenter";
static constexpr const char* kAddObserverSel                    = "addObserver:selector:name:object:";

// Constants
static constexpr const char* kUIDeviceOrientationDidChangeNoti  = "UIDeviceOrientationDidChangeNotification";
static constexpr const char* kUIDeviceClass                     = "UIDevice";

// UIImage and image data access selectors
static constexpr const char* kInitWithContentsOfFileSel         = "initWithContentsOfFile:";
static constexpr const char* kRepresentationsSel                = "representations";
static constexpr const char* kPixelsWideSel                     = "pixelsWide";
static constexpr const char* kPixelsHighSel                     = "pixelsHigh";
static constexpr const char* kBitsPerPixelSel                   = "bitsPerPixel";
static constexpr const char* kBytesPerRowSel                    = "bytesPerRow";
static constexpr const char* kHasAlphaSel                       = "hasAlpha";
static constexpr const char* kBitmapDataSel                     = "bitmapData";
static constexpr const char* kNSBitmapImageRepClass             = "NSBitmapImageRep";
static constexpr const char* kCGImageSel                        = "CGImage";

// OpenGL ES constants
static constexpr int kEAGLRenderingAPIOpenGLES3                 = 3;
static constexpr int kEAGLColorFormatRGBA8                      = 1;
static constexpr int kEAGLDepthFormat24                         = 1;
static constexpr int kGLRenderbufferColor                       = 0x8CE0; // GL_COLOR_ATTACHMENT0

// NS Bundle selectors
static constexpr const char* kMainBundleSel                     = "mainBundle";
static constexpr const char* kUTF8StringSel                     = "UTF8String";
static constexpr const char* kbundlePathSel                     = "bundlePath";

// NSLocale class and method names
static constexpr const char* kNSLocaleClass                     = "NSLocale";
static constexpr const char* kCurrentLocaleSel                  = "currentLocale";
static constexpr const char* kLocaleIdentifierSel               = "localeIdentifier";

// Default values and configuration settings
static constexpr const char* kWindowTitle                       = "C iOS OpenGLES Framework";
static constexpr const char* kDefaultAppPath                    = "olcPGE3_iOS";
static constexpr double kDefaultWindowWidth                     = 1280.0;    // TODO: Upgate to the default ios screen size I think it is 538X402
static constexpr double kDefaultWindowHeight                    = 728.0;
static constexpr int kDefaultFrameRate                          = 60;
static constexpr int kPreferredFrameRate                        = 120;
static constexpr int kBitsPerByte                               = 8;
static constexpr int kMinValidDimension                         = 0;
static constexpr int kRGBABytesPerPixel                         = 4;
static constexpr int kFullyOpaque                               = 255;
static constexpr int kZeroBytes                                 = 0;
static constexpr int kZeroRows                                  = 0;
static constexpr int kZeroWidth                                 = 0;
static constexpr int kZeroHeight                                = 0;
static constexpr int kFlippedOffset                             = 1;
static constexpr int kNoButton                                  = -1;

// Objective-C method type encoding constants
static constexpr const char* kVoidMethodTypeEncoding            = "v@:";    // Type encoding for void methods with no parameters: "v@:"
static constexpr const char* kBoolMethodTypeEncoding            = "c@:";    // Type encoding for methods returning BOOL with no parameters: "c@:"
static constexpr const char* kEventHandlerMethodTypeEncoding    = "v@:@";   // Type encoding for methods with event/notification parameter: "v@:@"
static constexpr const char* kIOSAppDelegateMethodTypeEncoding  = "c@:@@";  // Type encoding for iOS app delegate methods returning BOOL: "c@:@@" (BOOL, self, _cmd, app, options)
static constexpr const char* kIOSAppMethodTypeEncoding          = "v@:@";   // Type encoding for iOS app delegate methods with app parameter: "v@:@" (void, self, _cmd, app)
static constexpr const char* kTouchEventMethodTypeEncoding      = "v@:@@";  // Type encoding for touch event methods with two object parameters: "v@:@@"
static constexpr const char* kClassMethodTypeEncoding           = "#@:";    // Type encoding for class methods returning Class: "#@:"
static constexpr const char* kDrawRectMethodTypeEncoding        = "v@:{NSRect={NSPoint=dd}{NSSize=dd}}"; // Type encoding for drawRect method with NSRect parameter: "v@:{CGRect={CGPoint=dd}{CGSize=dd}}"
static constexpr const char* kDrawGCectMethodTypeEncoding       = "v@:{CGRect={CGPoint=dd}{CGSize=dd}}"; // Type encoding for drawRect method with CGRect parameter: "v@:{CGRect={CGPoint=dd}{CGSize=dd}}"
static constexpr const char* kMethodSignatureMethodTypeEncoding = "@@::";   // Type encoding for methodSignatureForSelector: returning NSMethodSignature: "@@::"


namespace ObjectiveCSEL {
    
    // Application memory management selectors
    static SEL allocSel                             = nullptr;
    static SEL initSel                              = nullptr;
    static SEL drainSel                             = nullptr;
    static SEL setDelegateSel                       = nullptr;
    static SEL releaseSel                           = nullptr;
    static SEL retainSel                            = nullptr;
    static SEL isKindOfClassSel                     = nullptr;

    // UIApplication lifecycle and management selectors
    static SEL sharedApplicationSel                 = nullptr;
    static SEL runSel                               = nullptr;
    static SEL setIdleTimerDisabledSel              = nullptr;
    static SEL setPausedSel                         = nullptr;

    // UIApplicationDelegate lifecycle methods
    static SEL applicationDidFinishLaunchingSel     = nullptr;
    static SEL applicationWillTerminateSel          = nullptr;
    static SEL applicationDidBecomeActiveSel        = nullptr;
    static SEL applicationWillResignActiveSel       = nullptr;
    static SEL applicationDidEnterBackgroundSel     = nullptr;
    static SEL applicationWillEnterForegroundSel    = nullptr;

    // UIWindow and UIScreen selectors
    static SEL mainScreenSel                        = nullptr;
    static SEL boundsSelScreen                      = nullptr;
    static SEL nativeBounds                         = nullptr;
    static SEL scaleSelScreen                       = nullptr;
    static SEL initWithFrameSel                     = nullptr;
    static SEL makeKeyAndVisibleSel                 = nullptr;
    static SEL maximumFramesPerSecond               = nullptr;
    static SEL setAutoresizingMaskSel               = nullptr;

    // UIViewController creation, display, and management selectors
    static SEL initWithNibNameSel                   = nullptr;
    static SEL stringWithUTF8StringSel              = nullptr;
    static SEL setTitleSel                          = nullptr;
    static SEL viewSel                              = nullptr;
    static SEL setViewSel                           = nullptr;
    static SEL presentViewControllerSel             = nullptr;
    static SEL dismissViewControllerSel             = nullptr;
    static SEL frameSel                             = nullptr;
    static SEL setFrameSel                          = nullptr;
    static SEL boundsSel                            = nullptr;
    static SEL setBoundsSel                         = nullptr;
    static SEL updateView                           = nullptr;

    // Root view controller selector
    static SEL setRootViewControllerSel             = nullptr;

    // GLKViewController lifecycle methods
    static SEL viewDidLoadSel                       = nullptr;
    static SEL loadViewSel                          = nullptr;
    static SEL viewWillAppearSel                    = nullptr;
    static SEL viewDidAppearSel                     = nullptr;
    static SEL viewWillDisappearSel                 = nullptr;
    static SEL viewDidDisappearSel                  = nullptr;
    static SEL viewDidLayoutSubviewsSel             = nullptr;
    static SEL drawRectSel                          = nullptr;
    static SEL setContentScaleFactor                = nullptr;
    static SEL contentScaleFactor                   = nullptr;

    // Touch event methods selectors
    static SEL touchesBeganSel                      = nullptr;
    static SEL touchesMovedSel                      = nullptr;
    static SEL touchesEndedSel                      = nullptr;
    static SEL touchesCancelledSel                  = nullptr;
    static SEL allObjectsSel                        = nullptr;
    static SEL majorRadius                          = nullptr;

    // Keyboard event methods selectors (for hardware keyboards)
    static SEL pressesBeganSel                      = nullptr;
    static SEL pressesEndedSel                      = nullptr;

    // Touch and press event property selectors
    static SEL locationInViewSel                    = nullptr;
    static SEL allTouchesSel                        = nullptr;
    static SEL countSel                             = nullptr;
    static SEL objectAtIndexSel                     = nullptr;
    static SEL keySel                               = nullptr;
    static SEL keyCodeSel                           = nullptr;
    static SEL charactersSel                        = nullptr;
    static SEL modifierFlagsSel                     = nullptr;

    // EAGL and OpenGL ES management selectors
    static SEL initWithAPISel                       = nullptr;
    static SEL setCurrentContextSel                 = nullptr;
    static SEL contextSel                           = nullptr;
    static SEL setContextSel                        = nullptr;
    static SEL drawableColorFormatSel               = nullptr;
    static SEL setDrawableColorFormatSel            = nullptr;
    static SEL drawableDepthFormatSel               = nullptr;
    static SEL setDrawableDepthFormatSel            = nullptr;
    static SEL drawableMultisampleSel               = nullptr;
    static SEL setDrawableMultisampleSel            = nullptr;
    static SEL setEnableSetNeedsDisplaySel          = nullptr;
    static SEL bindDrawableSel                      = nullptr;
    static SEL displaySel                           = nullptr;
    static SEL presentRenderbufferSel               = nullptr;
    static SEL setNeedsDisplaySel                   = nullptr;
    static SEL setPreferredFramesPerSecondSel       = nullptr;

    // Device and orientation selectors
    static SEL currentDeviceSel                     = nullptr;
    static SEL orientationSel                       = nullptr;
    static SEL isMultitouchEnabledSel               = nullptr;
    static SEL setMultipleTouchEnabledSel           = nullptr;
    static SEL setUserInteractionEnabledSel         = nullptr;
    static SEL isUserInteractionEnabledSel          = nullptr;
    static SEL deviceOrientationDidChangeSel        = nullptr;
    static SEL beginGenDeviceOrientationNotifSel    = nullptr;
    
    // UI Image
    static SEL initWithContentsOfFileSel            = nullptr;
    static SEL representationsSel                   = nullptr;
    static SEL pixelsWideSel                        = nullptr;
    static SEL pixelsHighSel                        = nullptr;
    static SEL bitsPerPixelSel                      = nullptr;
    static SEL bytesPerRowSel                       = nullptr;
    static SEL hasAlphaSel                          = nullptr;
    static SEL bitmapDataSel                        = nullptr;
    static SEL cgImageSel                           = nullptr;

    // NS Bundle selectors
    static SEL mainBundleSel                        = nullptr;
    static SEL UTF8StringSel                        = nullptr;
    static SEL bundlePathSel                        = nullptr;

    // NSLocale selectors
    static SEL currentLocaleSel                     = nullptr;
    static SEL localeIdentifierSel                  = nullptr;

    // Notification classes and selectors
    static SEL defaultCenterSel                     = nullptr;
    static SEL addObserverSel                       = nullptr;



    void initializeSelectors() {
        if (allocSel) return; // Already initialized
        
        // Application memory management selectors
        allocSel                            = sel_registerName(kAllocSel);
        initSel                             = sel_registerName(kInitSel);
        drainSel                            = sel_registerName(kDrainSel);
        setDelegateSel                      = sel_registerName(kSetDelegateSel);
        releaseSel                          = sel_registerName(kReleaseSel);
        retainSel                           = sel_registerName(kRetainSel);
        isKindOfClassSel                    = sel_registerName(kIsKindOfClassSel);
        
        // UIApplication lifecycle and management selectors
        sharedApplicationSel                = sel_registerName(kSharedApplicationSel);
        runSel                              = sel_registerName(kRunSel);
        setIdleTimerDisabledSel             = sel_registerName(kSetIdleTimerDisabledSel);
        setPausedSel                        = sel_registerName(kSetPausedSel);
        
        // UIApplicationDelegate lifecycle methods
        applicationDidFinishLaunchingSel    = sel_registerName(kApplicationDidFinishLaunchingSel);
        applicationWillTerminateSel         = sel_registerName(kApplicationWillTerminateSel);
        applicationDidBecomeActiveSel       = sel_registerName(kApplicationDidBecomeActiveSel);
        applicationWillResignActiveSel      = sel_registerName(kApplicationWillResignActiveSel);
        applicationDidEnterBackgroundSel    = sel_registerName(kApplicationDidEnterBackgroundSel);
        applicationWillEnterForegroundSel   = sel_registerName(kApplicationWillEnterForegroundSel);
        
        // UIWindow and UIScreen selectors
        mainScreenSel                       = sel_registerName(kmainScreenSel);
        boundsSelScreen                     = sel_registerName(kBoundsSelScreen);
        nativeBounds                        = sel_registerName(kNativeBounds);
        scaleSelScreen                      = sel_registerName(kScaleSelScreen);
        initWithFrameSel                    = sel_registerName(kInitWithFrameSel);
        makeKeyAndVisibleSel                = sel_registerName(kMakeKeyAndVisibleSel);
        maximumFramesPerSecond              = sel_registerName(kMaximumFramesPerSecond);
        setAutoresizingMaskSel              = sel_registerName(kSetAutoresizingMaskSel);
        
        // UIViewController creation, display, and management selectors
        initWithNibNameSel                  = sel_registerName(kInitWithNibNameSel);
        stringWithUTF8StringSel             = sel_registerName(kStringWithUTF8StringSel);
        setTitleSel                         = sel_registerName(kSetTitleSel);
        viewSel                             = sel_registerName(kViewSel);
        setViewSel                          = sel_registerName(kSetViewSel);
        presentViewControllerSel            = sel_registerName(kPresentViewControllerSel);
        dismissViewControllerSel            = sel_registerName(kDismissViewControllerSel);
        frameSel                            = sel_registerName(kFrameSel);
        setFrameSel                         = sel_registerName(kSetFrameSel);
        boundsSel                           = sel_registerName(kBoundsSel);
        setBoundsSel                        = sel_registerName(kSetBoundsSel);
        updateView                          = sel_registerName(kUpdateView);
        
        // Root view controller selector
        setRootViewControllerSel            = sel_registerName(kSetRootViewControllerSel);
        
        // GLKViewController lifecycle methods
        viewDidLoadSel                      = sel_registerName(kViewDidLoadSel);
        loadViewSel                         = sel_registerName(kLoadViewSel);
        viewWillAppearSel                   = sel_registerName(kViewWillAppearSel);
        viewDidAppearSel                    = sel_registerName(kViewDidAppearSel);
        viewWillDisappearSel                = sel_registerName(kViewWillDisappearSel);
        viewDidDisappearSel                 = sel_registerName(kViewDidDisappearSel);
        viewDidLayoutSubviewsSel            = sel_registerName(kViewDidLayoutSubviewsSel);
        drawRectSel                         = sel_registerName(kDrawRectSel);
        
        // Touch event methods selectors
        touchesBeganSel                     = sel_registerName(kTouchesBeganSel);
        touchesMovedSel                     = sel_registerName(kTouchesMovedSel);
        touchesEndedSel                     = sel_registerName(kTouchesEndedSel);
        touchesCancelledSel                 = sel_registerName(kTouchesCancelledSel);
        allObjectsSel                       = sel_registerName(kAllObjectsSel);
        majorRadius                         = sel_registerName(kMajorRadius);
        
        // Keyboard event methods selectors (for hardware keyboards)
        pressesBeganSel                     = sel_registerName(kPressesBeganSel);
        pressesEndedSel                     = sel_registerName(kPressesEndedSel);
        keySel                              = sel_registerName(kKeySel);
        keyCodeSel                          = sel_registerName(kKeyCodeSel);
        charactersSel                       = sel_registerName(kCharactersSel);
        modifierFlagsSel                    = sel_registerName(kModifierFlagsSel);
        
        // Touch and press event property selectors
        locationInViewSel                   = sel_registerName(kLocationInViewSel);
        allTouchesSel                       = sel_registerName(kAllTouchesSel);
        countSel                            = sel_registerName(kCountSel);
        objectAtIndexSel                    = sel_registerName(kObjectAtIndexSel);
        keySel                              = sel_registerName(kKeySel);
        charactersSel                       = sel_registerName(kCharactersSel);
        
        // EAGL and OpenGL ES management selectors
        initWithAPISel                      = sel_registerName(kInitWithAPISel);
        setCurrentContextSel                = sel_registerName(kSetCurrentContextSel);
        contextSel                          = sel_registerName(kContextSel);
        setContextSel                       = sel_registerName(kSetContextSel);
        drawableColorFormatSel              = sel_registerName(kDrawableColorFormatSel);
        setDrawableColorFormatSel           = sel_registerName(kSetDrawableColorFormatSel);
        drawableDepthFormatSel              = sel_registerName(kDrawableDepthFormatSel);
        setDrawableDepthFormatSel           = sel_registerName(kSetDrawableDepthFormatSel);
        drawableMultisampleSel              = sel_registerName(kDrawableMultisampleSel);
        setDrawableMultisampleSel           = sel_registerName(kSetDrawableMultisampleSel);
        setEnableSetNeedsDisplaySel         = sel_registerName(kSetEnableSetNeedsDisplaySel);
        bindDrawableSel                     = sel_registerName(kBindDrawableSel);
        displaySel                          = sel_registerName(kDisplaySel);
        presentRenderbufferSel              = sel_registerName(kPresentRenderbufferSel);
        setNeedsDisplaySel                  = sel_registerName(kSetNeedsDisplaySel);
        setPreferredFramesPerSecondSel      = sel_registerName(kSetPreferredFramesPerSecondSel);
        setContentScaleFactor               = sel_registerName(kSetContentScaleFactorSel);
        contentScaleFactor                  = sel_registerName(kContentScaleFactorSel);
        
        // Device and orientation selectors
        currentDeviceSel                    = sel_registerName(kCurrentDeviceSel);
        orientationSel                      = sel_registerName(kOrientationSel);
        isMultitouchEnabledSel              = sel_registerName(kIsMultitouchEnabledSel);
        setMultipleTouchEnabledSel          = sel_registerName(kSetMultipleTouchEnabledSel);
        setUserInteractionEnabledSel        = sel_registerName(kSetUserInteractionEnabledSel);
        isUserInteractionEnabledSel         = sel_registerName(kIsUserInteractionEnabledSel);
        deviceOrientationDidChangeSel       = sel_registerName(kDeviceOrientationDidChangeSel);
        beginGenDeviceOrientationNotifSel   = sel_registerName(kBeginGenDeviceOrientationNotifSel);
        
        // UI Image
        initWithContentsOfFileSel           = sel_registerName(kInitWithContentsOfFileSel);
        representationsSel                  = sel_registerName(kRepresentationsSel);
        pixelsWideSel                       = sel_registerName(kPixelsWideSel);
        pixelsHighSel                       = sel_registerName(kPixelsHighSel);
        bitsPerPixelSel                     = sel_registerName(kBitsPerPixelSel);
        bytesPerRowSel                      = sel_registerName(kBytesPerRowSel);
        hasAlphaSel                         = sel_registerName(kHasAlphaSel);
        bitmapDataSel                       = sel_registerName(kBitmapDataSel);
        cgImageSel                          = sel_registerName(kCGImageSel);
        
        // NS Bundle selectors
        mainBundleSel                       = sel_registerName(kMainBundleSel);
        UTF8StringSel                       = sel_registerName(kUTF8StringSel);
        bundlePathSel                       = sel_registerName(kbundlePathSel);
        
        // NSLocale selectors
        currentLocaleSel                   = sel_registerName(kCurrentLocaleSel);
        localeIdentifierSel                = sel_registerName(kLocaleIdentifierSel);
        
        // Notification classes and selectors
        defaultCenterSel                    = sel_registerName(kDefaultCenterSel);
        addObserverSel                      = sel_registerName(kAddObserverSel);
        
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
void initialize_ios_api() {
    ObjectiveCSEL::initializeSelectors();
}

// Concepts safety templates for Objective-C interactions
template<typename T>
concept CallbackType = std::is_function_v<std::remove_pointer_t<T>>; // Checks if T is a function type

template<typename T>
concept ObjectiveC_Id = std::is_pointer_v<T>;   // Checks if T is a pointer type (Objective-C id types are pointers)

// TODO: May not be required as iOS handles memory management more automatically than macOS in most cases
// Autorelease pool management functions
extern "C" {
    extern void* objc_autoreleasePoolPush(void);
    extern void objc_autoreleasePoolPop(void* pool);
    extern void* NSStringFromClass(Class aClass);
    // Add UIApplicationMain declaration
    extern int UIApplicationMain(int argc, char* argv[], id principalClassName, id delegateClassName);
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

// TODO: End

// Keeps our C --> Objective-C function pointer types clean
using NSInteger = long;
using NSUInteger = unsigned long;

/*
* WARNING: Global delegate pointers for Objective-C callbacks
* These global pointers are used to route Objective-C delegate callbacks
* to the appropriate C++ structures and methods.
*
* In short: Don't mess with these unless you know what you're doing!
*
* Global variables are evil, but sometimes necessary for bridging C++ and Objective-C.
*/
static ViewController*  gptrVeiwControllerEvents    = nullptr;
static GLKView*         gptrGLKViewEvents          = nullptr;
static Application*     gptrApplicationDelegate     = nullptr;
static ViewController*  gptrViewControllerDelegate  = nullptr;

// TODO: May not be needed
// Forward declarations of internal structures
struct Application;
struct ApplicationDelegate;
void updateView(id self, SEL _cmd);
void drawRect(id self, SEL _cmd, CGRect rect);
id createCustomGLKView(CGRect frame);
void viewcontroller_enableFlexibleSizing(struct ViewController* self);
struct ViewController;
struct ViewControllerDelegate;
struct OpenGLRenderer;
struct ImageLoader;

// Forward declarations of internal structures
struct Application {
    id uiApp{nullptr};      // UIApplication instance
    id delegate{nullptr};   // ApplicationDelegate
    id uiWindow{nullptr};   // UIWindow instance
    
    // Callback function pointers
    std::function<void(void*)> willFinishLaunchingCallback{nullptr};
    std::function<void(void*)> didFinishLaunchingCallback{nullptr};
    std::function<void(void*)> willTerminateCallback{nullptr};
    std::function<void(void*)> didBecomeActiveCallback{nullptr};
    std::function<void(void*)> willResignActiveCallback{nullptr};
    std::function<void(void*)> didEnterBackgroundCallback{nullptr};
    std::function<void(void*)> willEnterForegroundCallback{nullptr};
    
    void* willFinishLaunchingUserData{nullptr};    // User data for will finish launching callback
    void* didFinishLaunchingUserData{nullptr};     // User data for did finish launching callback
    void* willTerminateUserData{nullptr};          // User data for will terminate callback
    void* didBecomeActiveUserData{nullptr};        // User data for did become active callback
    void* willResignActiveUserData{nullptr};       // User data for will resign active callback
    void* didEnterBackgroundUserData{nullptr};     // User data for did enter background callback
    void* willEnterForegroundUserData{nullptr};    // User data for will enter foreground
    
    // Method function pointers
    // TODO: Update order of start upo events to suit iOS lifecycle
    void (*initialize)  (struct Application* self){nullptr};
    void (*activate)    (struct Application* self){nullptr};
    void (*run)         (struct Application* self){nullptr};
    void (*destroy)     (struct Application* self){nullptr};
    
    std::string appPath = "";  // Application path
    const char* (*getAppPath)  (struct Application* self);
    
    Application() = default;
    
    ~Application() {
        if (destroy) {
            destroy(this);
        }
    }
    
    // Delete copy constructor and assignment
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    // Allow move semantics
    Application(Application&&) = default;
    Application& operator=(Application&&) = default;
 
};

// Handles UIApplication delegate methods and lifecycle events
struct ApplicationDelegate {
    Class isa;                      // Objective-C class pointer (required)
};

struct ViewController {
    id viewController;             // UIViewController instance
    id glkview;                    // GLKView instance
    id delegate;                   // ViewControllerDelegate instance
    char* title;                   // Title of the view controller
    CGRect frame;                  // Frame of the view controller
    struct GLKView* glkViewStruct; // Reference to GLKView struct for touch handling
    
    // Lifecycle callbacks
    std::function<void(void*)>       viewDidLoadCallback{nullptr};
    std::function<void(void*)>       viewWillAppearCallback{nullptr};
    std::function<void(void*)>       viewDidAppearCallback{nullptr};
    std::function<void(void*)>       viewWillDisappearCallback{nullptr};
    std::function<void(void*)>       viewDidDisappearCallback{nullptr};
    std::function<void(void*)>       viewDidLayoutSubviewsCallback{nullptr};
    std::function<void(int, void*)>  orientationChangedCallback{nullptr};
    std::function<void(void*)>       updateCallback{nullptr};
  
    
    void* viewDidLoadUserData{nullptr};             // User data for viewDidLoad callback
    void* viewWillAppearUserData{nullptr};          // User data for viewWillAppear callback
    void* viewDidAppearUserData{nullptr};           // User data for viewDidAppear callback
    void* viewWillDisappearUserData{nullptr};       // User data for viewWillDisappear callback
    void* viewDidDisappearUserData{nullptr};        // User data for viewDidDisappear callback
    void* viewDidLayoutSubviewsUserData{nullptr};   // User data for viewDidLayout
    void* orientationChangedUserData{nullptr};      // User data for orientation changed callback
    void* updateUserData{nullptr};                  // User data for update callback
            
};

struct GLKView {
    id glkView;                                  // GLKView instance
    struct ViewController* parentViewController; // Reference to parent view controller
    
    // Touch callbacks
    std::function<void(uint32_t, double, double, double, double, void*)>  touchBeganCallback{nullptr};
    std::function<void(uint32_t, double, double, double, double, void*)>  touchMovedCallback{nullptr};
    std::function<void(uint32_t, double, double, double, double, void*)>  touchEndedCallback{nullptr};
    std::function<void(uint32_t, double, double, double, double, void*)>  touchCancelledCallback{nullptr};

    
    void* touchBeganUserData     {nullptr};               // User data for touch began callback
    void* touchMovedUserData     {nullptr};               // User data for touch moved callback
    void* touchEndedUserData     {nullptr};               // User data for touch ended callback
    void* touchCancelledUserData {nullptr};               // User data for touch cancelled callback

    // Event callback function pointers with nullptr initialization
    std::function<void(uint16_t, const char*, uint, void*)>  keyDownCallback{nullptr};
    std::function<void(uint16_t, const char*, uint, void*)>  keyUpCallback{nullptr};
    
    void* keyDownUserData{nullptr};      // User data for key down callback
    void* keyUpUserData  {nullptr};      // User data for key up callback
    
    // Draw callback function pointer
    std::function<void(double, double, double, double, void*)>  drawCallback{nullptr};
    void* drawUserData{nullptr};       // User data for draw callback
};

struct ViewControllerDelegate {
    Class isa;                          // Objective-C class pointer (required)
};

struct OpenGLRenderer {
    id eaglContext   {nullptr};         // EAGLContext instance
    id eaglView      {nullptr};         // Custom UIView with CAEAGLLayer
    id viewController{nullptr};         // UIViewController instance
    id glkView       {nullptr};         // GLKView
    id displayLink   {nullptr};         // CADisplayLink instance
    BOOL vsyncEnabled{YES};             // VSync enabled flag
    unsigned int framebuffer{0};        // OpenGL framebuffer ID
    unsigned int colorRenderbuffer{0};  // Color renderbuffer ID
    unsigned int depthRenderbuffer{0};  // Depth renderbuffer ID

};

// Modern image loading and pixel data extraction
struct ImageLoader {
    
    unsigned char* pixelData    {nullptr};             // Raw pixel data (RGBA format)
    int width                   {kMinValidDimension};  // Image width in pixels
    int height                  {kMinValidDimension};  // Image height in pixels
    int bytesPerPixel           {kZeroBytes};          // Number of bytes per pixel (typically 4 for RGBA)
    int bytesPerRow             {kZeroRows};           // Number of bytes per row
    BOOL hasAlpha               {NO};                  // Whether image has alpha channel
    std::string appPath         = "";  // Application path

    // Method function pointers with nullptr initialization
    char (*getAppPath)                  (const struct ImageLoader* self){nullptr};
    BOOL (*loadFromFile)                (struct ImageLoader* self, const char* filePath){nullptr};
    void (*destroy)                     (struct ImageLoader* self){nullptr};
    unsigned char* (*getPixelData)      (const struct ImageLoader* self){nullptr};
    void (*getImageInfo)                (const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel){nullptr};
    void (*getDetailedInfo)             (const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel, int* bytesPerRow, BOOL* hasAlpha){nullptr};
    BOOL (*isLoaded)                    (const struct ImageLoader* self){nullptr};
    BOOL (*getPixel)                    (const struct ImageLoader* self, int x, int y, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha){nullptr};
    unsigned int (*createOpenGLTexture) (const struct ImageLoader* self){nullptr};
};

// TODO: Update MacOS to use these safe Objective-C functions for better error handling and safety
static Class safe_objc_getClass(const char* name) {
    Class cls = objc_getClass(name);
    if (!cls) {
        printf("Error: Could not find class %s\n", name);
        return nil;
    }
    return cls;
}

static SEL safe_sel_registerName(const char* name) {
    SEL sel = sel_registerName(name);
    if (!sel) {
        printf("Error: Could not register selector %s\n", name);
        return NULL;
    }
    return sel;
}

static id safe_objc_msgSend(id self, SEL op) {
    if (!self || !op) return nil;
    return ((id(*)(id, SEL))objc_msgSend)(self, op);
}

static id safe_objc_msgSend_id(id self, SEL op, id arg) {
    if (!self || !op) return nil;
    return ((id(*)(id, SEL, id))objc_msgSend)(self, op, arg);
}

static void safe_objc_msgSend_void(id self, SEL op) {
    if (!self || !op) return;
    ((void(*)(id, SEL))objc_msgSend)(self, op);
}

static void safe_objc_msgSend_void_bool(id self, SEL op, BOOL arg) {
    if (!self || !op) return;
    ((void(*)(id, SEL, BOOL))objc_msgSend)(self, op, arg);
}

// TODO: End


// ============================================================================
// IMPLEMENTATION OF C API FUNCTIONS
// ============================================================================


// Called when application is about to terminate
void applicationWillTerminate(id self, SEL _cmd, id application) {
    (void)self;(void)_cmd;(void)application;
    if (gptrApplicationDelegate && gptrApplicationDelegate->willTerminateCallback) {
        gptrApplicationDelegate->willTerminateCallback(gptrApplicationDelegate->willTerminateUserData);
    }
}



// Called when application becomes active
void applicationDidBecomeActive(id self, SEL _cmd, id application) {
    (void)self;(void)_cmd;(void)application;
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


// Called when application resigns active status
void applicationDidEnterBackground(id self, SEL _cmd, id application) {
    (void)self;(void)_cmd;(void)application;
    if (gptrApplicationDelegate && gptrApplicationDelegate->willResignActiveCallback) {
        gptrApplicationDelegate->didEnterBackgroundCallback(gptrApplicationDelegate->didEnterBackgroundUserData);
    }
}

// Called when application resigns active status
void applicationWillEnterForeground(id self, SEL _cmd, id application) {
    (void)self;(void)_cmd;(void)application;
    if (gptrApplicationDelegate && gptrApplicationDelegate->willResignActiveCallback) {
        gptrApplicationDelegate->willEnterForegroundCallback(gptrApplicationDelegate->willEnterForegroundUserData);
    }
}



// Called when application has finished launching - iOS version
BOOL applicationDidFinishLaunchingWithOptions(id self, SEL _cmd, id application, id launchOptions) {
    (void)self;(void)_cmd;(void)application;(void)launchOptions;
    
    if (gptrApplicationDelegate && gptrApplicationDelegate->didFinishLaunchingCallback) {
        gptrApplicationDelegate->didFinishLaunchingCallback(gptrApplicationDelegate->didFinishLaunchingUserData);
    }
    
    return YES; // Must return BOOL for iOS delegate method
}



// Create and configure application delegate
id createApplicationDelegate(void) {

    // Get NSObject class as superclass
    Class NSObjectClass = objc_getClass(kNSObjectClass);
    // Create a new class inheriting from NSObject using const string
    Class AppDelegateClass = objc_allocateClassPair(NSObjectClass, kolcPGE3AppDelegateClass, 0);
    
    // Add application delegate methods
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationWillTerminateSel,          (IMP)applicationWillTerminate,                 kIOSAppMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationDidBecomeActiveSel,        (IMP)applicationDidBecomeActive,               kEventHandlerMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationWillResignActiveSel,       (IMP)applicationWillResignActive,              kEventHandlerMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationDidEnterBackgroundSel,     (IMP)applicationDidEnterBackground,            kEventHandlerMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationWillEnterForegroundSel,    (IMP)applicationWillEnterForeground,           kEventHandlerMethodTypeEncoding);
    class_addMethod(AppDelegateClass, ObjectiveCSEL::applicationDidFinishLaunchingSel,     (IMP)applicationDidFinishLaunchingWithOptions, kIOSAppDelegateMethodTypeEncoding);

    // Register the class with the runtime
    objc_registerClassPair(AppDelegateClass);
    
    // Create an instance
    id delegate  = ((id(*)(Class, SEL))objc_msgSend)(AppDelegateClass, ObjectiveCSEL::allocSel);
    delegate     = ((id(*)(id, SEL))objc_msgSend)(delegate, ObjectiveCSEL::initSel);
    
    // Retain the delegate to ensure it stays alive
    if (delegate) {
        delegate = ((id(*)(id, SEL))objc_msgSend)(delegate, ObjectiveCSEL::retainSel);
    }
    
    return delegate;
}



// Application API Implementation
struct Application* application_init(void) {
    
    ObjectiveCSEL::ensureInitialized(); // Ensure selectors are initialized

    Application* app = new Application();

    // Assign the method pointers
    app->initialize = application_initialize;
    app->activate   = application_activate;
    app->run        = application_run;
    app->destroy    = application_destroy;
    app->getAppPath = application_getApplicationPath;
    
    return app;
}




char* get_application_path(void)
{
    const char* pathCString = kDefaultAppPath;
    
    // Get application path using NSBundle
    Class bundleClass = objc_getClass(kNSBundleClass);
    
    // Get main bundle
    id mainBundle = ((id(*)(Class, SEL))objc_msgSend)(bundleClass, ObjectiveCSEL::mainBundleSel);
    
    if (mainBundle) {
        // Get bundle path as NSString
        id bundlePath = ((id(*)(id, SEL))objc_msgSend)(mainBundle, ObjectiveCSEL::bundlePathSel);
        
        if (bundlePath) {
            // Convert NSString to C string
            pathCString = ((const char*(*)(id, SEL))objc_msgSend)(bundlePath, ObjectiveCSEL::UTF8StringSel);
        }
    }
    return const_cast<char*>(pathCString);
}

// iOS specific application start and run function
int run_ios_application(int argc, char* argv[]) {
    
    // Get NSAutoreleasePool for memory management
    Class poolClass = objc_getClass(kNSAutoreleasePoolClass);

    // Get the application pool instance
    id poolInstance = ((id(*)(Class, SEL))objc_msgSend)(poolClass, ObjectiveCSEL::allocSel);
    id pool = ((id(*)(id, SEL))objc_msgSend)(poolInstance, ObjectiveCSEL::initSel);

    // Get PGE AppDelegate class so that iOS Event fires on our host_apple_ios
    Class appDelegateClass = objc_getClass(kolcPGE3AppDelegateClass);
    id delegateClassName = (id)NSStringFromClass(appDelegateClass);

    // Call UIApplicationMain to start running with our delegate
    int result = UIApplicationMain(argc, argv, nil, delegateClassName);

    // Clean up
    ((void(*)(id, SEL))objc_msgSend)(pool, ObjectiveCSEL::drainSel);

    return result;
}

void application_initialize(struct Application* self) {
        
    // Initialize cached selectors
    initialize_ios_api();
    
    // Set global application reference
    gptrApplicationDelegate = self;
    
    // Create and set application delegate
    self->delegate = createApplicationDelegate();
    
    // Set the appPath
    self->appPath = get_application_path();
    
    // Set the correct args
    int argc = 1;
    char* argv[2];
    argv[0] = const_cast<char*>(self->appPath.c_str());
    argv[1] = nullptr;
    
    // Start and run the iOS application
    int exitCode = run_ios_application(argc, argv);
    (void)exitCode; // Currently unused, but could be logged or handled
    
 }



void application_activate(struct Application* self) {
    if(!self) return;
    
    // Create window programmatically
    Class windowClass = objc_getClass(kUIWindowClass);
    Class screenClass = objc_getClass(kUIScreenClass);
       
    // Get main screen bounds
    id mainScreen =         ((id(*)(Class, SEL))objc_msgSend)(screenClass,ObjectiveCSEL::mainScreenSel);
    CGRect screenBounds =   ((CGRect(*)(id, SEL))objc_msgSend)(mainScreen, ObjectiveCSEL::boundsSel);
    
    // Create window
    id window =             ((id(*)(Class, SEL))objc_msgSend)(windowClass, ObjectiveCSEL::allocSel);
    window =                ((id(*)(id, SEL, CGRect))objc_msgSend)(window, ObjectiveCSEL::initWithFrameSel, screenBounds);

    // Create a basic view controller as root view controller
    Class vcClass =         objc_getClass(kUIViewControllerClass);
    id rootViewController = ((id(*)(Class, SEL))objc_msgSend)(vcClass, ObjectiveCSEL::allocSel);
    rootViewController =    ((id(*)(id, SEL))objc_msgSend)(rootViewController, ObjectiveCSEL::initSel);
    
    // Set root view controller
    ((void(*)(id, SEL, id))objc_msgSend)(window, ObjectiveCSEL::setRootViewControllerSel, rootViewController);
    
    // Make window visible
    ((void(*)(id, SEL))objc_msgSend)     (window, ObjectiveCSEL::makeKeyAndVisibleSel);

    // Store window reference
    self->uiWindow = window;
    
    // Activate the UIApplication once (There can only be one!)
    if(!self->uiApp)
    {
        Class UIApplicationClass = objc_getClass(kUIApplicationClass);
        self->uiApp =              ((id(*)(Class, SEL))objc_msgSend)(UIApplicationClass, ObjectiveCSEL::sharedApplicationSel);
        
        // Disable idle timer to prevent screen from dimming during gameplay
        safe_objc_msgSend_void_bool(self->uiApp, ObjectiveCSEL::setIdleTimerDisabledSel, YES);
    }
   

}
void application_run(struct Application* self) {
    if (!self || !self->uiApp) return;

    // In iOS, we don't typically call UIApplicationMain from here
    // The main run loop is handled by the system
    // This is more of a placeholder for consistency with macOS API
    // Left in for potential future use or custom run loop handling if needed
}

void application_destroy(struct Application* self) {
    if (!self) return;
    
    if (self->delegate) {
        safe_objc_msgSend_void(self->delegate, ObjectiveCSEL::releaseSel);
    }
    
    free(self);
}

const char* application_getApplicationPath(Application* self) {
   (void)self;
    return get_application_path();
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
   const char* localeIdentifier = ((const char*(*)(id, SEL))objc_msgSend)(localeIdentifierNS, ObjectiveCSEL::UTF8StringSel);
   
   return localeIdentifier;
}

// ============================================================================
// TOUCH AND KEYBOARD EVENT HANDLER IMPLEMENTATIONS
// ============================================================================


// iOS Touch Event Data Structure
struct iOSTouchEventData {
    uint32_t touchID = 0;             // Stable sequential touch ID
    CGPoint location = {0.0, 0.0};    // Touch location in view coordinates
    CGSize touchSize = {1.0, 1.0};    // Touch size (width, height)
    bool bIsActive =   true;          // Touch is active (true for Began/Moved, false for Ended/Cancelled)
};

// Maps UITouch object pointers to stable uint32_t touch IDs
static std::unordered_map<uintptr_t, uint32_t> siOSTouchIDMap;
static uint32_t siOSNextTouchID = 0;
static std::unordered_map<uintptr_t, iOSTouchEventData> siOSTouchIDEvents;

// Convert touch location to physical pixel coordinates for PGE
void convertToContentViewCoordinates(CGPoint& location, id view) {
    if (!view) return;
    
    // Get the content scale factor for Retina displays (1.0, 2.0, or 3.0)
    CGFloat scale = ((CGFloat(*)(id, SEL))objc_msgSend)(view, ObjectiveCSEL::contentScaleFactor);
     
    // Convert from logical points to physical pixels
    location.x *= scale;
    location.y *= scale;
    
}

// Iterate through touches, extract data, and fire the callback
// bRemove: true when processing Ended or Cancelled events (clean up touch tracking)
static void updateiOSTouchData(id touches,
    const std::function<void(uint32_t, double, double, double, double, void*)>& callback,
    void* userData, bool bRemove = false)
{
    if (!callback) return;
    
    NSUInteger count =  ((NSUInteger(*)(id, SEL))objc_msgSend)(touches, ObjectiveCSEL::countSel);
    if (count == 0) return;
    
    id touchArray =     ((id(*)(id, SEL))objc_msgSend)(touches, ObjectiveCSEL::allObjectsSel);
    
    for (NSUInteger i = 0; i < count; ++i) {
        id touch =      ((id(*)(id, SEL, NSUInteger))objc_msgSend)(touchArray, ObjectiveCSEL::objectAtIndexSel, i);
        if (!touch)     continue;
        
        iOSTouchEventData data;
        
        // Get the view from the touch
        id touchView =      ((id(*)(id, SEL))objc_msgSend)(touch, ObjectiveCSEL::viewSel);
        
        // Get touch location in view coordinates (logical points)
        CGPoint location = ((CGPoint(*)(id, SEL, id))objc_msgSend)(touch, ObjectiveCSEL::locationInViewSel, touchView);
        
        // Convert to physical pixels and flip Y coordinate for OpenGL/PGE
        convertToContentViewCoordinates(location, touchView);
        
        // get the UITouch Major and Minor radius (touch size) if available, otherwise default to 1.0
        CGFloat majorRadius = 1.0;
        if (touch && ObjectiveCSEL::majorRadius) {
            majorRadius =   ((CGFloat(*)(id, SEL))objc_msgSend)(touch, ObjectiveCSEL::majorRadius);
        }
        
              
        data.location  = location;
        data.touchSize = {majorRadius, majorRadius};
        data.bIsActive = !bRemove;
        
        // Assign a stable sequential uint32_t ID using the UITouch object pointer as key
        uintptr_t key = reinterpret_cast<uintptr_t>(touch);
        
        auto [it, inserted] = siOSTouchIDMap.emplace(key, siOSNextTouchID);
        if (inserted)       ++siOSNextTouchID;
        uint32_t tid        = it->second;
        
        data.touchID        = tid;
        siOSTouchIDEvents.emplace(key, data);
        
        // Fire the callback with the stable touch ID and coordinates
        callback(tid, data.location.x, data.location.y, data.touchSize.width, data.touchSize.height, userData);
        
        // Clean up map entry once the touch has ended or been cancelled
        if (bRemove) {
            siOSTouchIDEvents.erase(key);
            siOSTouchIDMap.erase(key);
        }
    }
    
    // Reset touch ID counter when all touches are released
    if (bRemove && siOSTouchIDMap.size() == 0) {
        siOSNextTouchID = 0;
    }
}

// Touch event handlers - called by Objective-C runtime
void touchesBegan(id self, SEL _cmd, id touches, id event) {
    (void)self; (void)_cmd; (void)event;
    
    if (!gptrGLKViewEvents || !gptrGLKViewEvents->touchBeganCallback) return;
    
    updateiOSTouchData(touches, gptrGLKViewEvents->touchBeganCallback, gptrGLKViewEvents->touchBeganUserData, false);
}

void touchesMoved(id self, SEL _cmd, id touches, id event) {
    (void)self; (void)_cmd; (void)event;
    
    if (!gptrGLKViewEvents || !gptrGLKViewEvents->touchMovedCallback) return;
    
    updateiOSTouchData(touches, gptrGLKViewEvents->touchMovedCallback, gptrGLKViewEvents->touchMovedUserData, false);
}

void touchesEnded(id self, SEL _cmd, id touches, id event) {
    (void)self; (void)_cmd; (void)event;
    
    if (!gptrGLKViewEvents || !gptrGLKViewEvents->touchEndedCallback) return;
    
    updateiOSTouchData(touches, gptrGLKViewEvents->touchEndedCallback, gptrGLKViewEvents->touchEndedUserData, true);
}

void touchesCancelled(id self, SEL _cmd, id touches, id event) {
    (void)self; (void)_cmd; (void)event;
    
    if (!gptrGLKViewEvents || !gptrGLKViewEvents->touchCancelledCallback) return;
    
    updateiOSTouchData(touches, gptrGLKViewEvents->touchCancelledCallback, gptrGLKViewEvents->touchCancelledUserData, true);
}

// Structure to hold common key event data
struct KeyEventData {
    uint16_t keyCode = 0;
    const char* characters = nullptr;
    uint modifierFlags = 0;
};

// Extract common key event data from NSEvent and press
KeyEventData extractKeyEventData(id event, id press) {
    KeyEventData data;
    id key = ((id(*)(id, SEL))objc_msgSend)(press, ObjectiveCSEL::keySel);
    data.keyCode = ((unsigned short(*)(id, SEL))objc_msgSend)(key, ObjectiveCSEL::keyCodeSel);
    id characters = ((id(*)(id, SEL))objc_msgSend)(key, ObjectiveCSEL::charactersSel);
    data.characters = ((const char*(*)(id, SEL))objc_msgSend)(characters, ObjectiveCSEL::UTF8StringSel);
    data.modifierFlags = ((unsigned int(*)(id, SEL))objc_msgSend)(event, ObjectiveCSEL::modifierFlagsSel);
    // printf("Key Event: keyCode=%u, characters=%s, modifierFlags=%u\n", data.keyCode, data.characters ? data.characters : "null", data.modifierFlags);
    return data;
}

// Keyboard event handlers
void pressesBegan(id self, SEL _cmd, id presses, id event) {
    (void)self; (void)_cmd; (void)event;
                
    if (!gptrGLKViewEvents || !presses) return; // edge case with presses been null when bluetooth keyboard is disconnected
    
    // Get an NS array of all presses from the NSSet
    id pressesArray = ((id(*)(id, SEL))objc_msgSend)(presses, ObjectiveCSEL::allObjectsSel);
    
    NSUInteger pressCount = ((NSUInteger(*)(id, SEL))objc_msgSend)(presses, ObjectiveCSEL::countSel);
    if (pressCount == 0) return;
    
    for(NSUInteger i = 0; i < pressCount; i++)
    {
        id press = ((id(*)(id, SEL, NSUInteger))objc_msgSend)(pressesArray, ObjectiveCSEL::objectAtIndexSel, i);
        
        KeyEventData data = extractKeyEventData(event, press);
        
        if (gptrGLKViewEvents->keyDownCallback) {
            gptrGLKViewEvents->keyDownCallback(data.keyCode, data.characters, data.modifierFlags, gptrGLKViewEvents->keyDownUserData);
        }
    }

}

void pressesEnded(id self, SEL _cmd, id presses, id event) {
    (void)self; (void)_cmd; (void)event;
    
    if (!gptrGLKViewEvents || !presses) return; // edge case with presses been null when bluetooth keyboard is disconnected
    
    // Get an NS array of all presses from the NSSet that have ended
    id pressesArray = ((id(*)(id, SEL))objc_msgSend)(presses, ObjectiveCSEL::allObjectsSel);
    
    NSUInteger pressCount = ((NSUInteger(*)(id, SEL))objc_msgSend)(presses, ObjectiveCSEL::countSel);
    if (pressCount == 0) return;
    
    for(NSUInteger i = 0; i < pressCount; i++)
    {
        id press = ((id(*)(id, SEL, NSUInteger))objc_msgSend)(pressesArray, ObjectiveCSEL::objectAtIndexSel, i);
        
        KeyEventData data = extractKeyEventData(event, press);
        
        if (gptrGLKViewEvents->keyUpCallback) {
            gptrGLKViewEvents->keyUpCallback(data.keyCode, data.characters, data.modifierFlags, gptrGLKViewEvents->keyUpUserData);
            
        }
        
    }
}


// View Controller lifecycle event handlers
void viewDidLoad(id self, SEL _cmd) {
    (void)self; (void)_cmd;
    
    if (gptrViewControllerDelegate && gptrViewControllerDelegate->viewDidLoadCallback) {
        gptrViewControllerDelegate->viewDidLoadCallback(gptrViewControllerDelegate->viewDidLoadUserData);
    } else {
        printf("No viewDidLoadCallback set or gptrViewControllerDelegate is null\n");
    }
}

void device_getScreenBounds(CGRect* rect) {
    
    Class screenClass = safe_objc_getClass(kUIScreenClass);
    
    id mainScreen     = safe_objc_msgSend((id)screenClass, ObjectiveCSEL::mainScreenSel);
    if (!mainScreen) {
        return;
    }
    
    // Try nativeBounds first (returns points in native orientation)
    CGRect nativeBounds = ((CGRect(*)(id, SEL))objc_msgSend)(mainScreen, ObjectiveCSEL::nativeBounds);
    
    if (nativeBounds.size.width > 0 && nativeBounds.size.height > 0) {
        *rect = nativeBounds;
    } else {
        // Fallback to regular bounds
        CGRect bounds = ((CGRect(*)(id, SEL))objc_msgSend)(mainScreen, ObjectiveCSEL::boundsSel);
        *rect = bounds;
    }
}

void loadView(id self, SEL _cmd) {
    (void)self; (void)_cmd;

    device_getScreenBounds(&gptrVeiwControllerEvents->frame);

    if (gptrVeiwControllerEvents) {
        // Create the GLKView with the stored frame
        gptrVeiwControllerEvents->glkview = createCustomGLKView(gptrVeiwControllerEvents->frame);
        if (gptrVeiwControllerEvents->glkview) {
            // Set it as the view
            safe_objc_msgSend_id(self, ObjectiveCSEL::setViewSel, gptrVeiwControllerEvents->glkview);
            
            // Initialize GLKView struct for touch event handling
            gptrVeiwControllerEvents->glkViewStruct = glkview_init(gptrVeiwControllerEvents);
            if (gptrVeiwControllerEvents->glkViewStruct) {
                // Enable auto-sizing
                viewcontroller_enableFlexibleSizing(gptrVeiwControllerEvents);
            } else {
                printf("Failed to initialize GLKView struct in loadView\n");
            }
        } else {
            printf("Failed to create GLKView in loadView\n");
        }
    } else {
        printf("gptrVeiwControllerEvents is null in loadView\n");
    }
}

void viewWillAppear(id self, SEL _cmd, BOOL animated) {
    (void)self; (void)_cmd; (void)animated;
    
    if (gptrViewControllerDelegate && gptrViewControllerDelegate->viewWillAppearCallback) {
        gptrViewControllerDelegate->viewWillAppearCallback(gptrViewControllerDelegate->viewWillAppearUserData);
    }
}

void viewDidAppear(id self, SEL _cmd, BOOL animated) {
    (void)self; (void)_cmd; (void)animated;
    
    if (gptrViewControllerDelegate && gptrViewControllerDelegate->viewDidAppearCallback) {
        gptrViewControllerDelegate->viewDidAppearCallback(gptrViewControllerDelegate->viewDidAppearUserData);
    }
}

void viewWillDisappear(id self, SEL _cmd, BOOL animated) {
    (void)self; (void)_cmd; (void)animated;
    
    if (gptrViewControllerDelegate && gptrViewControllerDelegate->viewWillDisappearCallback) {
        gptrViewControllerDelegate->viewWillDisappearCallback(gptrViewControllerDelegate->viewWillDisappearUserData);
    }
}

void viewDidDisappear(id self, SEL _cmd, BOOL animated) {
    (void)self; (void)_cmd; (void)animated;
    
    if (gptrViewControllerDelegate && gptrViewControllerDelegate->viewDidDisappearCallback) {
        gptrViewControllerDelegate->viewDidDisappearCallback(gptrViewControllerDelegate->viewDidDisappearUserData);
    }
}

void viewDidLayoutSubviews(id self, SEL _cmd) {
    (void)self; (void)_cmd;

    device_getScreenBounds(&gptrVeiwControllerEvents->frame);
    if (gptrViewControllerDelegate && gptrViewControllerDelegate->viewDidLayoutSubviewsCallback) {
        gptrViewControllerDelegate->viewDidLayoutSubviewsCallback(gptrViewControllerDelegate->viewDidLayoutSubviewsUserData);
    }
}

void updateView(id self, SEL _cmd) {
    (void)self; (void)_cmd;
    
    if (gptrViewControllerDelegate && gptrViewControllerDelegate->updateCallback) {
        gptrViewControllerDelegate->updateCallback(gptrViewControllerDelegate->updateUserData);
    }
}

// Update the orientation change handler to receive NSNotification
void viewOrientationChanged(id self, SEL _cmd, id notification) {
    (void)self; (void)_cmd; (void)notification;
    
    if (!gptrViewControllerDelegate) return;
    
    // Get current device orientation
    Class deviceClass = objc_getClass(kUIDeviceClass);
    
    id currentDevice = ((id(*)(Class, SEL))objc_msgSend)(deviceClass, ObjectiveCSEL::currentDeviceSel);
    int orientation = ((int(*)(id, SEL))objc_msgSend)(currentDevice, ObjectiveCSEL::orientationSel);
    
    if (gptrViewControllerDelegate && gptrViewControllerDelegate->orientationChangedCallback) {
        gptrViewControllerDelegate->orientationChangedCallback(orientation, gptrViewControllerDelegate->orientationChangedUserData);
    }
}

// Create custom GLKViewController subclass with event handling
id createCustomViewController(void) {
    // Check if class already exists to avoid duplicate registration
    Class CustomViewControllerClass = objc_getClass(kViewControllerDelegateClass);
    
    if (!CustomViewControllerClass) {
        // Create custom GLKViewController subclass only if it doesn't exist
        Class GLKViewControllerClass = objc_getClass(kGLKViewControllerClass);
        CustomViewControllerClass = objc_allocateClassPair(GLKViewControllerClass, kViewControllerDelegateClass, 0);
        
        if (!CustomViewControllerClass) {
            printf("Failed to create custom GLKViewController class\n");
            return nil;
        }
        
        // Add keyboard event methods
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::pressesBeganSel,               (IMP)pressesBegan,          kTouchEventMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::pressesEndedSel,               (IMP)pressesEnded,          kTouchEventMethodTypeEncoding);

        // Add lifecycle methods
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::updateView,                    (IMP)updateView,             kVoidMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::loadViewSel,                   (IMP)loadView,               kVoidMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::viewDidLoadSel,                (IMP)viewDidLoad,            kVoidMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::viewWillAppearSel,             (IMP)viewWillAppear,         kBoolMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::viewDidAppearSel,              (IMP)viewDidAppear,          kBoolMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::viewWillDisappearSel,          (IMP)viewWillDisappear,      kBoolMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::viewDidDisappearSel,           (IMP)viewDidDisappear,       kBoolMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::viewDidLayoutSubviewsSel,      (IMP)viewDidLayoutSubviews,  kVoidMethodTypeEncoding);
        class_addMethod(CustomViewControllerClass, ObjectiveCSEL::deviceOrientationDidChangeSel, (IMP)viewOrientationChanged, kEventHandlerMethodTypeEncoding);
        
        
        // Register the class
        objc_registerClassPair(CustomViewControllerClass);
        
    }
    
    // Create instance
    id viewController = ((id(*)(Class, SEL))objc_msgSend)(CustomViewControllerClass, ObjectiveCSEL::allocSel);
    viewController =    ((id(*)(id, SEL))objc_msgSend)(viewController, ObjectiveCSEL::initSel);
    
    if (!viewController) {
        printf("Failed to create GLKViewController instance\n");
        return nil;
    }
    return viewController;
}

void drawRect(id self, SEL _cmd, CGRect rect) {
    (void)self; (void)_cmd; (void)rect;
       
    if (gptrGLKViewEvents && gptrGLKViewEvents->drawCallback) {
        gptrGLKViewEvents->drawCallback(rect.origin.x, rect.origin.y,
                                        rect.size.width, rect.size.height,
                                        gptrGLKViewEvents->drawUserData);
    }
}

// Create custom GLKView subclass for touch handling
id createCustomGLKView(CGRect frame) {
    // Check if class already exists to avoid duplicate registration
    Class CustomGLKViewClass = objc_getClass(kCustomGLKViewClass);
    
    if (!CustomGLKViewClass) {
        // Create custom GLKView subclass only if it doesn't exist
        Class GLKViewClass = objc_getClass(kGLKViewClass);
        CustomGLKViewClass = objc_allocateClassPair(GLKViewClass, kCustomGLKViewClass, 0);
        
        if (!CustomGLKViewClass) {
            printf("Failed to create custom GLKView class\n");
            return nil;
        }
        
        // Add touch event methods to the view
        class_addMethod(CustomGLKViewClass, ObjectiveCSEL::touchesBeganSel,        (IMP)touchesBegan,       kTouchEventMethodTypeEncoding);
        class_addMethod(CustomGLKViewClass, ObjectiveCSEL::touchesMovedSel,        (IMP)touchesMoved,       kTouchEventMethodTypeEncoding);
        class_addMethod(CustomGLKViewClass, ObjectiveCSEL::touchesEndedSel,        (IMP)touchesEnded,       kTouchEventMethodTypeEncoding);
        class_addMethod(CustomGLKViewClass, ObjectiveCSEL::touchesCancelledSel,    (IMP)touchesCancelled,   kTouchEventMethodTypeEncoding);
        
        // Add draw methods
        class_addMethod(CustomGLKViewClass, ObjectiveCSEL::drawRectSel,            (IMP)drawRect,           kDrawRectMethodTypeEncoding);

        // Register the class
        objc_registerClassPair(CustomGLKViewClass);

    }
    
    // Create instance
    id view = ((id(*)(Class, SEL))objc_msgSend)(CustomGLKViewClass, ObjectiveCSEL::allocSel);
    view =    ((id(*)(id, SEL, CGRect))objc_msgSend)(view, ObjectiveCSEL::initWithFrameSel, frame);
    
    if (!view) {
        printf("Failed to create GLKView instance\n");
        return nil;
    }
    
    // Enable user interaction (disabled by default on GLKView)
    ((void(*)(id, SEL, BOOL))objc_msgSend)(view, ObjectiveCSEL::setUserInteractionEnabledSel, YES);
    
    // Enable multiple touch support
    ((void(*)(id, SEL, BOOL))objc_msgSend)(view, ObjectiveCSEL::setMultipleTouchEnabledSel, YES);
    
    return view;
}

// GLKView API Implementation
struct GLKView* glkview_init(struct ViewController* viewController)
{
    // TODO: Remove second call to this
    if(gptrVeiwControllerEvents->glkViewStruct != nullptr)
        return gptrVeiwControllerEvents->glkViewStruct;
    
    if (!viewController || !viewController->glkview) {
        printf("Error: Invalid view controller or GLKView not initialized\n");
        return nullptr;
    }
    
    struct GLKView* glkViewStruct = (struct GLKView*)malloc(sizeof(struct GLKView));
    if (!glkViewStruct) {
        printf("Error: Failed to allocate GLKView struct\n");
        return nullptr;
    }
    
    memset(glkViewStruct, 0, sizeof(struct GLKView));
    glkViewStruct->glkView              = viewController->glkview;
    glkViewStruct->parentViewController = viewController;

    // Set up global pointer for touch event routing
    gptrGLKViewEvents = glkViewStruct;
    
    return glkViewStruct;
}

void glkview_destroy(struct GLKView* self) {
    if (!self) return;
    
    if (gptrGLKViewEvents == self) {
        gptrGLKViewEvents = nullptr;
    }
    
    free(self);
}

void* glkview_getNativeView(const struct GLKView* self) {
    if (!self) return nullptr;
    return self->glkView;
}

void glkview_setFrame(struct GLKView* self, double x, double y, double width, double height) {
    if (!self || !self->glkView) return;
    
    CGRect frame = CGRectMake(x, y, width, height);
    typedef void (*SetFrameFunc)(id, SEL, CGRect);
    ((SetFrameFunc)objc_msgSend)(self->glkView, ObjectiveCSEL::setFrameSel, frame);
}

void glkview_getFrame(const struct GLKView* self, double* x, double* y, double* width, double* height) {
    if (!self || !self->glkView) return;
    
    CGRect frame = ((CGRect(*)(id, SEL))objc_msgSend)(self->glkView, ObjectiveCSEL::setFrameSel);
    if (x) *x = frame.origin.x;
    if (y) *y = frame.origin.y;
    if (width) *width = frame.size.width;
    if (height) *height = frame.size.height;
}

void glkview_setMultipleTouchEnabled(struct GLKView* self, BOOL enabled) {
    if (!self || !self->glkView) return;
    safe_objc_msgSend_void_bool(self->glkView, ObjectiveCSEL::setMultipleTouchEnabledSel, enabled);
}

BOOL glkview_isMultipleTouchEnabled(const struct GLKView* self) {
    if (!self || !self->glkView) return NO;
    return ((BOOL(*)(id, SEL))objc_msgSend)(self->glkView, ObjectiveCSEL::isMultitouchEnabledSel);
}

void glkview_enableTouchHandling(struct GLKView* self) {
    if (!self || !self->glkView) return;
    safe_objc_msgSend_void_bool(self->glkView, ObjectiveCSEL::setUserInteractionEnabledSel, YES);
}

void glkview_disableTouchHandling(struct GLKView* self) {
    if (!self || !self->glkView) return;
    safe_objc_msgSend_void_bool(self->glkView, ObjectiveCSEL::setUserInteractionEnabledSel, NO);
}

void glkview_debugTouchSetup(struct GLKView* self) {
    if (!self || !self->glkView) {
        printf("GLKView Debug: Invalid GLKView struct or view\n");
        return;
    }
        
    BOOL touchEnabled = ((BOOL(*)(id, SEL))objc_msgSend)(self->glkView, ObjectiveCSEL::isUserInteractionEnabledSel);
    BOOL multiTouchEnabled = ((BOOL(*)(id, SEL))objc_msgSend)(self->glkView, ObjectiveCSEL::isMultitouchEnabledSel);
    
    printf("  GLKView Touch Debug:\n");
    printf("  Touch Enabled: %s\n", touchEnabled ? "YES" : "NO");
    printf("  Multi-touch Enabled: %s\n", multiTouchEnabled ? "YES" : "NO");
    printf("  Touch Began Callback: %s\n", self->touchBeganCallback ? "Set" : "Not Set");
    printf("  Touch Moved Callback: %s\n", self->touchMovedCallback ? "Set" : "Not Set");
    printf("  Touch Ended Callback: %s\n", self->touchEndedCallback ? "Set" : "Not Set");
    printf("  Touch Cancelled Callback: %s\n", self->touchCancelledCallback ? "Set" : "Not Set");
}

void glkview_setNeedsDisplay(struct GLKView* self) {
    if (!self || !self->glkView) return;
    safe_objc_msgSend_void(self->glkView, ObjectiveCSEL::setNeedsDisplaySel);
}


// View Controller API Implementation
struct ViewController* viewcontroller_init(double width, double height) {
    ObjectiveCSEL::ensureInitialized(); // Ensure selectors are initialized
    
    struct ViewController* pViewController = (struct ViewController*)malloc(sizeof(struct ViewController));
    if (!pViewController) return NULL;
    
    memset(pViewController, 0, sizeof(struct ViewController));
    
    // Set initial frame
    pViewController->frame.origin.x = 0.0;
    pViewController->frame.origin.y = 0.0;
    pViewController->frame.size.width = width;
    pViewController->frame.size.height = height;
    
    // Create custom GLKViewController with event handling
    pViewController->viewController = createCustomViewController();
    if (!pViewController->viewController) {
        free(pViewController);
        return NULL;
    }
    
    // Set up global pointer for event routing
    gptrVeiwControllerEvents = pViewController;
    gptrViewControllerDelegate = pViewController;
    
    return pViewController;
}

void setupOrientationNotifications(struct ViewController* self) {
    if (!self || !self->viewController) return;
    
    // Get notification center
    Class notificationCenterClass = objc_getClass(kNSNotificationCenterClass);
    id notificationCenter = ((id(*)(Class, SEL))objc_msgSend)(notificationCenterClass, ObjectiveCSEL::defaultCenterSel);
    
    // Create notification name NSString
    Class stringClass = objc_getClass(kNSStringClass);
    id notificationName = ((id(*)(Class, SEL, const char*))objc_msgSend)(
        stringClass, ObjectiveCSEL::stringWithUTF8StringSel, kUIDeviceOrientationDidChangeNoti);
    
    // Register for orientation notifications
    ((void(*)(id, SEL, id, SEL, id, id))objc_msgSend)(
        notificationCenter, ObjectiveCSEL::addObserverSel,
        self->viewController, ObjectiveCSEL::deviceOrientationDidChangeSel,
        notificationName, nil);
    
    // Enable orientation notifications
    Class deviceClass = objc_getClass(kUIDeviceClass);
    
    id currentDevice = ((id(*)(Class, SEL))objc_msgSend)(deviceClass, ObjectiveCSEL::currentDeviceSel);
    ((void(*)(id, SEL))objc_msgSend)(currentDevice, ObjectiveCSEL::beginGenDeviceOrientationNotifSel);
}

int device_getMaxFramesPerSecond(void) {
    
    Class screenClass = objc_getClass(kUIScreenClass);
    
    // Get main screen
    id mainScreen = ((id(*)(Class, SEL))objc_msgSend)(screenClass, ObjectiveCSEL::mainScreenSel);
    
    // Get maximum frames per second from screen
    NSInteger maxFPS = ((NSInteger(*)(id, SEL))objc_msgSend)(mainScreen, ObjectiveCSEL::maximumFramesPerSecond);
    
    return (int)maxFPS;
}

void viewcontroller_setAutoresizingMask(struct ViewController* self, unsigned int mask) {
    if (!self || !self->glkview) return;
    
    ((void(*)(id, SEL, unsigned int))objc_msgSend)(self->glkview, ObjectiveCSEL::setAutoresizingMaskSel, mask);
}

void viewcontroller_enableFlexibleSizing(struct ViewController* self) {
    if (!self) return;
    
    // Enable flexible width and height
    unsigned int flexibleSizing = (1 << 1) | (1 << 4);  // UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight
    viewcontroller_setAutoresizingMask(self, flexibleSizing);
}


void viewcontroller_present(struct ViewController* self) {
    if (!self || !self->viewController) {
        printf("Error: Invalid view controller for presentation\n");
        return;
    }

    setupOrientationNotifications(self);
    
    // Set preferred frames per second based on device capabilities
    if (self->viewController) {
        ((void(*)(id, SEL, int))objc_msgSend)(self->viewController, ObjectiveCSEL::setPreferredFramesPerSecondSel, device_getMaxFramesPerSecond());
    }
    
    // Enable the rendering loop
    ((void(*)(id, SEL, BOOL))objc_msgSend)(self->viewController, ObjectiveCSEL::setPausedSel, YES);
    
    // Set delegate for GLKViewController updates
    ((void(*)(id, SEL, id))objc_msgSend)(self->viewController, ObjectiveCSEL::setDelegateSel, self->viewController);
    
    // Get the main window from your application and set as root view controller
    if (gptrApplicationDelegate && gptrApplicationDelegate->uiWindow) {
        ((void(*)(id, SEL, id))objc_msgSend)(gptrApplicationDelegate->uiWindow,
                                             ObjectiveCSEL::setRootViewControllerSel,
                                             self->viewController);

    } else {
        printf("Warning: No main window found for presentation\n");
    }
    
    
}

void viewcontroller_destroy(struct ViewController* self) {
    if (!self) return;
    
    if (self->title) {
        free(self->title);
    }
    
    if (self->glkViewStruct) {
        glkview_destroy(self->glkViewStruct);
        self->glkViewStruct = nullptr;
    }
    
    if (self->glkview) {
        safe_objc_msgSend_void(self->glkview, ObjectiveCSEL::releaseSel);
    }
    
    if (self->viewController) {
        safe_objc_msgSend_void(self->viewController, ObjectiveCSEL::releaseSel);
    }
    
    if (self->delegate) {
        safe_objc_msgSend_void(self->delegate, ObjectiveCSEL::releaseSel);
    }
    
    free(self);
}

void viewcontroller_setPause(struct ViewController* self, BOOL paused) {
    if (!self || !self->viewController) return;
    
    ((void(*)(id, SEL, BOOL))objc_msgSend)(self->viewController, ObjectiveCSEL::setPausedSel, paused ? YES : NO);

}

void viewcontroller_setTitle(struct ViewController* self, const char* title) {
    if (!self || !self->viewController || !title) return;
    
    // Free existing title
    if (self->title) {
        free(self->title);
    }
    
    // Copy new title
    size_t len  = strlen(title);
    self->title = (char*)malloc(len + 1);
    if (!self->title) {
        return;
    }
    strcpy(self->title, title);
    
    // Create NSString and set title
    Class stringClass = safe_objc_getClass(kNSStringClass);
    
    id titleString = ((id(*)(id, SEL, const char*))objc_msgSend)((id)stringClass, ObjectiveCSEL::stringWithUTF8StringSel, title);
    safe_objc_msgSend_id(self->viewController, ObjectiveCSEL::setTitleSel, titleString);
}
 

const char* viewcontroller_getTitle(const struct ViewController* self) {
    if (!self) return NULL;
    return self->title;
}

void viewcontroller_updateViewFromiOS(struct ViewController* self) {
    if (!self || !self->glkview) return;
    
    // Update frame from the actual view bounds
    CGRect bounds = ((CGRect(*)(id, SEL))objc_msgSend)(self->glkview, ObjectiveCSEL::boundsSel);
    self->frame   = bounds;
}

void viewcontroller_getViewFrame(const struct ViewController* self, double* x, double* y, double* width, double* height) {
    if (!self) return;
    
    if (x) *x = self->frame.origin.x;
    if (y) *y = self->frame.origin.y;
    if (width) *width = self->frame.size.width;
    if (height) *height = self->frame.size.height;
}

void viewcontroller_getViewSize(const struct ViewController* self, double* width, double* height) {
    if (!self) return;
    
    if (width)  *width = self->frame.size.width;
    if (height) *height = self->frame.size.height;
}

void viewcontroller_setViewFrame(struct ViewController* self, double x, double y, double width, double height) {
    if (!self) return;
    
    self->frame.origin.x = x;
    self->frame.origin.y = y;
    self->frame.size.width = width;
    self->frame.size.height = height;
    
    if (self->glkview) {
        typedef void (*SetFrameFunc)(id, SEL, CGRect);
        ((SetFrameFunc)objc_msgSend)(self->glkview, ObjectiveCSEL::setFrameSel, self->frame);
    }
}

void viewcontroller_setViewSize(struct ViewController* self, double width, double height) {
    viewcontroller_setViewFrame(self, self->frame.origin.x, self->frame.origin.y, width, height);
}

// OpenGL ES Renderer API Implementation
struct OpenGLRenderer* opengl_init(void) {
    struct OpenGLRenderer* renderer = (struct OpenGLRenderer*)malloc(sizeof(struct OpenGLRenderer));
    if (!renderer) return NULL;
    
    memset(renderer, 0, sizeof(struct OpenGLRenderer));
    return renderer;
}

void opengl_initialize(struct OpenGLRenderer* self, struct ViewController* viewController) {
    if (!self || !viewController || !viewController->glkview) return;
    
    // Create EAGL context for OpenGL ES 3.0
    Class eaglClass = safe_objc_getClass(kEAGLContextClass);

    id allocatedContext = safe_objc_msgSend((id)eaglClass, ObjectiveCSEL::allocSel);
    if (!allocatedContext) return;
    
    self->eaglContext = ((id(*)(id, SEL, int))objc_msgSend)(allocatedContext,ObjectiveCSEL::initWithAPISel, kEAGLRenderingAPIOpenGLES3);
    if (!self->eaglContext) return;
    
    // Set context on GLKView
    self->glkView = viewController->glkview;
    safe_objc_msgSend_id(self->glkView, ObjectiveCSEL::setContextSel, self->eaglContext);
}


void opengl_setupContext(struct OpenGLRenderer* self) {
    if (!self || !self->eaglContext) return;
    
    // Set current context
    Class eaglClass = safe_objc_getClass(kEAGLContextClass);
    safe_objc_msgSend_id((id)eaglClass, ObjectiveCSEL::setCurrentContextSel, self->eaglContext);
    
    GLint maxSamples = OLC_MSAA_SAMPLES;
    if(maxSamples > 0)
        glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    
    // Configure GLKView properties
    if (self->glkView) {
                      
        // Set color format to RGBA8
        ((void(*)(id, SEL, int))objc_msgSend)(self->glkView, ObjectiveCSEL::setDrawableColorFormatSel, kEAGLColorFormatRGBA8);
        
        // Set depth format to 24-bit
        ((void(*)(id, SEL, int))objc_msgSend)(self->glkView, ObjectiveCSEL::setDrawableDepthFormatSel, kEAGLDepthFormat24);
        
        // Enable multisampling
        ((void(*)(id, SEL, int))objc_msgSend)(self->glkView, ObjectiveCSEL::setDrawableMultisampleSel, maxSamples);
               
        // Enable automatic display updates (recommended for games)
        ((void(*)(id, SEL, BOOL))objc_msgSend)(self->glkView, ObjectiveCSEL::setEnableSetNeedsDisplaySel, NO);
        
    }
}

void* opengl_getEAGLContext(const struct OpenGLRenderer* self) {
    if (!self) return NULL;
    return self->eaglContext;
}

void* opengl_getEAGLContextPtr(struct OpenGLRenderer* self) {
    if (!self) return NULL;
    return &(self->eaglContext);
}

void opengl_makeCurrentContext(struct OpenGLRenderer* self) {
    if (!self || !self->eaglContext) return;
    
    Class eaglClass = safe_objc_getClass(kEAGLContextClass);
    safe_objc_msgSend_id((id)eaglClass, ObjectiveCSEL::setCurrentContextSel, self->eaglContext);
}

void opengl_setVsync(struct OpenGLRenderer* self, BOOL enabled) {
    if (!self) return;
    self->vsyncEnabled = enabled;
}


void opengl_swapBuffers(struct OpenGLRenderer* self) {
    if (!self || !self->glkView) return;
    
    // In GLKit, buffer swapping is handled automatically by GLKView
    // We can trigger a display update instead
    safe_objc_msgSend_void(self->glkView, ObjectiveCSEL::displaySel);
}

void opengl_destroy(struct OpenGLRenderer* self) {
    if (!self) return;
    
    if (self->eaglContext) {
        Class eaglClass = safe_objc_getClass(kEAGLContextClass);
        safe_objc_msgSend_id((id)eaglClass,ObjectiveCSEL::setCurrentContextSel, nil);
        
        safe_objc_msgSend_void(self->eaglContext, ObjectiveCSEL::releaseSel);
    }
    
    free(self);
}

char* imageloader_getApplicationPath(struct ImageLoader* self)
{
    return get_application_path();
}

// Load image from file path using NSImage and NSBitmapImageRep
BOOL imageloader_loadFromFile(struct ImageLoader* self, const char* filePath) {
    // Clear any existing data
    if (self->pixelData) {
        free(self->pixelData);
        self->pixelData = NULL;
    }

    self->width             = kZeroWidth;
    self->height            = kZeroHeight;
    self->bytesPerPixel     = kZeroBytes;
    self->bytesPerRow       = kZeroRows;
    self->hasAlpha          = NO;

    // Get required classes and selectors
    Class NSStringClass     = objc_getClass(kNSStringClass);
    Class UIImageClass      = objc_getClass(kUIImageClass);

    // Create NSString from file path
    id pathString = ((id(*)(Class, SEL, const char*))objc_msgSend)(
        NSStringClass, ObjectiveCSEL::stringWithUTF8StringSel, filePath);
    
    if (!pathString) {
        return NO;
    }
    
    // Create UIImage from file
    id image = ((id(*)(id, SEL, id))objc_msgSend)(
                ((id(*)(Class, SEL))objc_msgSend)(UIImageClass, ObjectiveCSEL::allocSel),
               ObjectiveCSEL::initWithContentsOfFileSel, pathString);
    
    if (!image) {
        return NO;
    }
    
    CGImageRef cgImage = ((CGImageRef(*)(id, SEL))objc_msgSend)(image, ObjectiveCSEL::cgImageSel);

    if (!cgImage) {
        return NO;
    }

    self->width         = (int)CGImageGetWidth(cgImage);
    self->height        = (int)CGImageGetHeight(cgImage);
    self->bytesPerPixel = (int)CGImageGetBitsPerPixel(cgImage) / kBitsPerByte; // 4 for RGBA, 3 for RGB
    self->bytesPerRow   = (int)CGImageGetBytesPerRow(cgImage); //  self->width * self->bytesPerPixel;
    self->hasAlpha      = CGImageGetAlphaInfo(cgImage) != kCGImageAlphaNone;
 
    // Get pixel data directly from CGImage without drawing
    CGDataProviderRef dataProvider  = CGImageGetDataProvider(cgImage);
    CFDataRef data                  = CGDataProviderCopyData(dataProvider);

    if (data) {
        const unsigned char* bytes  = CFDataGetBytePtr(data);
        size_t dataLength           = CFDataGetLength(data);
        self->pixelData             = (unsigned char*)malloc(dataLength);
        
        memcpy(self->pixelData, bytes, dataLength);
        CFRelease(data);
        return YES;
    }
    
    return NO;

}

// Get raw pixel data pointer
unsigned char* imageloader_getPixelData(const struct ImageLoader* self) {
    return self->pixelData;
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
    unsigned char* pixel = self->pixelData + (flippedY * self->bytesPerRow) + (x * self->bytesPerPixel);
    
    // Extract color components (assuming RGBA or RGB format)
    if (red)   *red     = pixel[0];
    if (green) *green = pixel[1];
    if (blue)  *blue   = pixel[2];
    if (alpha  && self->bytesPerPixel >= kRGBABytesPerPixel)  *alpha = pixel[3];
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

// Create OpenGL texture from ImageLoader: TODO: Remove not needed
unsigned int imageloader_createOpenGLTexture(const struct ImageLoader* loader) {
    if (!loader->isLoaded(const_cast<struct ImageLoader*>(loader))) {
        return 0;
    }
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Determine format
    GLenum format = (loader->bytesPerPixel == kRGBABytesPerPixel) ? GL_RGBA : GL_RGB;

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, loader->width, loader->height,
                0, format, GL_UNSIGNED_BYTE, loader->pixelData);
    
    // Generate mipmaps for better quality at different scales
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return textureID;
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
    loader->destroy             = imageloader_destroy;
    loader->getPixelData        = imageloader_getPixelData;
    loader->getImageInfo        = imageloader_getImageInfo;
    loader->getDetailedInfo     = imageloader_getDetailedInfo;
    loader->isLoaded            = imageloader_isLoaded;
    loader->getPixel            = imageloader_getPixel;
    loader->createOpenGLTexture = imageloader_createOpenGLTexture;
    
    return loader;
}

// GLKView Event Handler API Implementation
void glkview_setTouchBeganCallback(struct GLKView* self, TouchEventCallback callback, void* userData) {
    if (!self) return;
    self->touchBeganCallback = callback;
    self->touchBeganUserData = userData;
}

void glkview_setTouchMovedCallback(struct GLKView* self, TouchEventCallback callback, void* userData) {
    if (!self) return;
    self->touchMovedCallback = callback;
    self->touchMovedUserData = userData;
}

void glkview_setTouchEndedCallback(struct GLKView* self, TouchEventCallback callback, void* userData) {
    if (!self) return;
    self->touchEndedCallback = callback;
    self->touchEndedUserData = userData;
}

void glkview_setTouchCancelledCallback(struct GLKView* self, TouchEventCallback callback, void* userData) {
    if (!self) return;
    self->touchCancelledCallback = callback;
    self->touchCancelledUserData = userData;
}

void glkview_setKeyDownCallback(struct GLKView* self, KeyEventCallback callback, void* userData) {
    if (!self) return;
    self->keyDownCallback = callback;
    self->keyDownUserData = userData;
}

void glkview_setKeyUpCallback(struct GLKView* self, KeyEventCallback callback, void* userData) {
    if (!self) return;
    self->keyUpCallback = callback;
    self->keyUpUserData = userData;
}



void glkview_setDrawCallback(struct GLKView* self, DrawCallback callback, void* userData) {
    if (!self) return;
    self->drawCallback = callback;
    self->drawUserData = userData;
}

// Application Delegate API Implementation
void application_setDidFinishLaunchingCallback(struct Application* self, ApplicationDelegateCallback callback, void* userData) {
    if (!self) return;
    self->didFinishLaunchingCallback = callback;
    self->didFinishLaunchingUserData = userData;
}

void application_setWillTerminateCallback(struct Application* self, ApplicationDelegateCallback callback, void* userData) {
    if (!self) return;
    self->willTerminateCallback = callback;
    self->willTerminateUserData = userData;
}

void application_setDidBecomeActiveCallback(struct Application* self, ApplicationDelegateCallback callback, void* userData) {
    if (!self) return;
    self->didBecomeActiveCallback = callback;
    self->didBecomeActiveUserData = userData;
}

void application_setWillResignActiveCallback(struct Application* self, ApplicationDelegateCallback callback, void* userData) {
    if (!self) return;
    self->willResignActiveCallback = callback;
    self->willResignActiveUserData = userData;
}

void application_setDidEnterBackgroundCallback(struct Application* self, ApplicationDelegateCallback callback, void* userData) {
    if (!self) return;
    self->didEnterBackgroundCallback = callback;
    self->didEnterBackgroundUserData = userData;
}

void application_setWillEnterForegroundCallback(struct Application* self, ApplicationDelegateCallback callback, void* userData) {
    if (!self) return;
    self->willEnterForegroundCallback = callback;
    self->willEnterForegroundUserData = userData;
}

// View Controller Delegate API Implementation
void viewcontroller_setViewDidLoadCallback(struct ViewController* self, ViewControllerDelegateCallback callback, void* userData) {
    if (!self) return;
    self->viewDidLoadCallback = callback;
    self->viewDidLoadUserData = userData;
}

void viewcontroller_setViewWillAppearCallback(struct ViewController* self, ViewControllerDelegateCallback callback, void* userData) {
    if (!self) return;
    self->viewWillAppearCallback = callback;
    self->viewWillAppearUserData = userData;
}

void viewcontroller_setViewDidAppearCallback(struct ViewController* self, ViewControllerDelegateCallback callback, void* userData) {
    if (!self) return;
    self->viewDidAppearCallback = callback;
    self->viewDidAppearUserData = userData;
}

void viewcontroller_setViewWillDisappearCallback(struct ViewController* self, ViewControllerDelegateCallback callback, void* userData) {
    if (!self) return;
    self->viewWillDisappearCallback = callback;
    self->viewWillDisappearUserData = userData;
}

void viewcontroller_setViewDidDisappearCallback(struct ViewController* self, ViewControllerDelegateCallback callback, void* userData) {
    if (!self) return;
    self->viewDidDisappearCallback = callback;
    self->viewDidDisappearUserData = userData;
}

void viewcontroller_setViewDidLayoutSubviewsCallback(struct ViewController* self, ViewControllerDelegateCallback callback, void* userData) {
    if (!self) return;
    self->viewDidLayoutSubviewsCallback = callback;
    self->viewDidLayoutSubviewsUserData = userData;
}

void viewcontroller_setUpdateCallback(struct ViewController* self, UpdateCallback callback, void* userData) {
    if (!self) return;
    self->updateCallback = callback;
    self->updateUserData = userData;
}

void viewcontroller_setOrientationChangedCallback(struct ViewController* self, OrientationChangedCallback callback, void* userData) {
    if (!self) return;
    self->orientationChangedCallback = callback;
    self->orientationChangedUserData = userData;
}

BOOL device_isMultitouchCapable(void) {
    // Most iOS devices support multitouch
    return YES;
}

BOOL device_hasHardwareKeyboard(void) {
    // This would require checking for connected external keyboards
    // For now, return NO (most  devices use on-screen keyboard)
    return NO;
}

void device_getScreenBounds(double* width, double* height) {
    if (!width || !height) return;
    
    // Get main screen bounds
    Class screenClass = safe_objc_getClass(kUIScreenClass);
    id mainScreen = safe_objc_msgSend((id)screenClass, ObjectiveCSEL::mainScreenSel);
    if (!mainScreen) {
        *width = *height = 0.0;
        return;
    }
    
    CGRect bounds = ((CGRect(*)(id, SEL))objc_msgSend)(mainScreen, ObjectiveCSEL::boundsSel);
    *width = bounds.size.width;
    *height = bounds.size.height;
}


//! END IMPLEMENTATION

