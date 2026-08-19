// C API  declarations for C++ wrapper

#pragma once
#include "config.h"

//! START STDHEADER GLOBAL
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <functional>
#include <type_traits>
#include <concepts>
#include <mutex>
//! END STDHEADER

//! START DECLARATION
// For C/C++ compilation, we need to include the C-compatible headers first
#include <objc/objc.h>
#include <objc/runtime.h>
#include <objc/message.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include <QuartzCore/QuartzCore.h>
#include <CoreGraphics/CoreGraphics.h>


extern "C" {
    
    // Forward declarations
    struct Application;
    struct ApplicationDelegate;
    struct ViewController;
    struct GLKView;
    struct OpenGLRenderer;
    struct ImageLoader;
    
    // Application API - as implemented in api_.cpp
    struct Application* application_init        (void);
    void application_initialize                 (struct Application* self);
    void application_activate                   (struct Application* self);
    void application_run                        (struct Application* self);
    void application_destroy                    (struct Application* self);
    const char* application_getApplicationPath  (struct Application* self);
    
    
    // View Controller API - as implemented in api_.cpp
    struct ViewController* viewcontroller_init  (double width, double height);
    void viewcontroller_present                 (struct ViewController* self);
    void viewcontroller_destroy                 (struct ViewController* self);
    void viewcontroller_setTitle                (struct ViewController* self, const char* title);
    void viewcontroller_setPause                (struct ViewController* self, BOOL pause);
    const char* viewcontroller_getTitle         (const struct ViewController* self);
    void viewcontroller_updateViewFromiOS       (struct ViewController* self);
    void viewcontroller_getViewFrame            (const struct ViewController* self, double* x, double* y, double* width, double* height);
    void viewcontroller_getViewSize             (const struct ViewController* self, double* width, double* height);
    void viewcontroller_setViewFrame            (struct ViewController* self, double x, double y, double width, double height);
    void viewcontroller_setViewSize             (struct ViewController* self, double width, double height);

     // GLKView API - as implemented in api_.cpp
    struct GLKView* glkview_init                (struct ViewController* viewController);
    void glkview_destroy                        (struct GLKView* self);
    void* glkview_getNativeView                 (const struct GLKView* self);  // Returns UIView, cast to void*
    void glkview_setFrame                       (struct GLKView* self, double x, double y, double width, double height);
    void glkview_getFrame                       (const struct GLKView* self, double* x, double* y, double* width, double* height);
    void glkview_setMultipleTouchEnabled        (struct GLKView* self, BOOL enabled);
    BOOL glkview_isMultipleTouchEnabled         (const struct GLKView* self);
    void glkview_enableTouchHandling            (struct GLKView* self);
    void glkview_disableTouchHandling           (struct GLKView* self);
    
    // OpenGL ES Renderer API - as implemented in api_.cpp
    struct OpenGLRenderer* opengl_init          (void);
    void opengl_initialize                      (struct OpenGLRenderer* self, struct ViewController* viewController);
    void opengl_setupContext                    (struct OpenGLRenderer* self);
    void* opengl_getEAGLContext                 (const struct OpenGLRenderer* self);         // Returns EAGLContext, cast to void*
    void* opengl_getEAGLContextPtr              (struct OpenGLRenderer* self);               // Returns EAGLContext as void*
    void opengl_makeCurrentContext              (struct OpenGLRenderer* self);               // Make OpenGL context current
    void opengl_setVsync                        (struct OpenGLRenderer* self, BOOL enabled); // Enable/disable vsync
    void opengl_swapBuffers                     (struct OpenGLRenderer* self);               // Present framebuffer
    void opengl_destroy                         (struct OpenGLRenderer* self);

    // Image Loader API - as implemented in api_.cpp
    struct ImageLoader* imageloader_init        (void);
    BOOL imageloader_loadFromFile               (struct ImageLoader* self, const char* filePath);
    void imageloader_destroy                    (struct ImageLoader* self);
    unsigned char* imageloader_getPixelData     (const struct ImageLoader* self);
    void imageloader_getImageInfo               (const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel);
    void imageloader_getDetailedInfo            (const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel, int* bytesPerRow, BOOL* hasAlpha);
    BOOL imageloader_isLoaded                   (const struct ImageLoader* self);
    BOOL imageloader_getPixel                   (const struct ImageLoader* self, int x, int y, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);
    unsigned int imageloader_createOpenGLTexture(const struct ImageLoader* self);
    char* imageloader_getApplicationPath        (struct ImageLoader* self);

   
    // Autorelease pool management
    void* objc_autoreleasePoolPush  (void);
    void objc_autoreleasePoolPop    (void* pool);
    
    // ===========================================================================
    // TOUCH EVENT HANDLING API
    // ===========================================================================
    
    // Touch event callback function types
    typedef void (*KeyEventCallback)        (unsigned short keyCode, const char* characters, void* userData);
    typedef void (*TouchEventCallback)      (uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData);
    typedef void (*DrawCallback)            (double x, double y, double width, double height, void* userData);
    typedef void (*UpdateCallback)          (void* userData);
    
    // Touch event handler setup - Now handled by GLKView
    void glkview_setTouchBeganCallback      (struct GLKView* self, TouchEventCallback callback, void* userData);
    void glkview_setTouchMovedCallback      (struct GLKView* self, TouchEventCallback callback, void* userData);
    void glkview_setTouchEndedCallback      (struct GLKView* self, TouchEventCallback callback, void* userData);
    void glkview_setTouchCancelledCallback  (struct GLKView* self, TouchEventCallback callback, void* userData);
    
    // Keyboard event handler setup (for hardware keyboards) - Now handled by GLKView
    void glkview_setKeyDownCallback         (struct GLKView* self, KeyEventCallback callback, void* userData);
    void glkview_setKeyUpCallback           (struct GLKView* self, KeyEventCallback callback, void* userData);
    
    // Draw event handler setup - Now handled by GLKView
    void glkview_setDrawCallback            (struct GLKView* self, DrawCallback callback, void* userData);
    void glkview_setNeedsDisplay            (struct GLKView* self);

    // Touch system management - Now handled by GLKView
    void glkview_debugTouchSetup            (struct GLKView* self);
    
    // Touch system management
    void viewcontroller_enableTouchHandling     (struct ViewController* self);
    void viewcontroller_disableTouchHandling    (struct ViewController* self);
    void viewcontroller_setMultipleTouchEnabled (struct ViewController* self, BOOL enabled);

    // Update event handler setup
    void viewcontroller_setUpdateCallback       (struct ViewController* self, UpdateCallback callback, void* userData);

    // Touch diagnostics TODO: Remove this in production builds
    void viewcontroller_debugTouchSetup         (struct ViewController* self);
    
    // ===========================================================================
    // APPLICATION DELEGATE EVENT HANDLING API
    // ===========================================================================
    
    // Application delegate callback function type
    typedef void (*ApplicationDelegateCallback)(void* userData);
    
    // Application delegate event handler setup
    void application_setDidFinishLaunchingCallback  (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setWillTerminateCallback       (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setDidBecomeActiveCallback     (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setWillResignActiveCallback    (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setDidEnterBackgroundCallback  (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setWillEnterForegroundCallback (struct Application* self, ApplicationDelegateCallback callback, void* userData);

    // ===========================================================================
    // VIEW CONTROLLER DELEGATE EVENT HANDLING API
    // ===========================================================================
    
    // View controller delegate callback function type
    typedef void (*ViewControllerDelegateCallback)          (void* userData);
    typedef void (*OrientationChangedCallback)              (int orientation, void* userData);
    
    // View controller delegate event handler setup
    void viewcontroller_setViewDidLoadCallback              (struct ViewController* self, ViewControllerDelegateCallback callback, void* userData);
    void viewcontroller_setViewWillAppearCallback           (struct ViewController* self, ViewControllerDelegateCallback callback, void* userData);
    void viewcontroller_setViewDidAppearCallback            (struct ViewController* self, ViewControllerDelegateCallback callback, void* userData);
    void viewcontroller_setViewWillDisappearCallback        (struct ViewController* self, ViewControllerDelegateCallback callback, void* userData);
    void viewcontroller_setViewDidDisappearCallback         (struct ViewController* self, ViewControllerDelegateCallback callback, void* userData);
    void viewcontroller_setViewDidLayoutSubviewsCallback    (struct ViewController* self, ViewControllerDelegateCallback callback, void* userData);

    void viewcontroller_setOrientationChangedCallback       (struct ViewController* self, OrientationChangedCallback callback, void* userData);
    
    // ===========================================================================
    // DEVICE ORIENTATION AND CAPABILITIES
    // ===========================================================================
    
    // Device orientation enumeration
    typedef enum {
        DeviceOrientationUnknown            = 0,
        DeviceOrientationPortrait           = 1,
        DeviceOrientationPortraitUpsideDown = 2,
        DeviceOrientationLandscapeLeft      = 3,
        DeviceOrientationLandscapeRight     = 4,
        DeviceOrientationFaceUp             = 5,
        DeviceOrientationFaceDown           = 6
    } DeviceOrientation;
    
    // Device capability queries
    DeviceOrientation device_getCurrentOrientation  (void);
    BOOL device_isMultitouchCapable                 (void);
    BOOL device_hasHardwareKeyboard                 (void);
    void device_getScreenBounds                     (double* width, double* height);
    void device_getSafeAreaInsets                   (double* top, double* left, double* bottom, double* right);
}

//! END DECLARATION
