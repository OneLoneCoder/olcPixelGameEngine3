// C API macOS declarations for C++ wrapper

#pragma once
#include "config.h"
//! START DECLARATION
#if OLC_HOST == OLC_HOST_MACOS
extern "C" {
    
    #ifndef BOOL
    #define BOOL int
    #endif

    // NSRect (OSX rectangle structure same as GCRect C structure)
    typedef struct NSRect {
        double x, y;
        double width, height;
    } NSRect;
    
    // Forward declarations
    struct Application;
    struct Window;
    struct OpenGLRenderer;
    struct ImageLoader;
    
    // Application API - as implemented in api_macos.c
    struct Application* application_init (void);
    void application_initialize          (struct Application* self);
    void application_activate            (struct Application* self);
    void application_run                 (struct Application* self);
    void application_destroy             (struct Application* self);
    
    // Window API - as implemented in api_macos.c
    struct Window* window_init           (double x, double y, double width, double height);
    void window_create                   (struct Window* self);
    void window_show                     (struct Window* self);
    void window_destroy                  (struct Window* self);
    void window_setTitle                 (struct Window* self, const char* title);
    const char* window_getTitle          (const struct Window* self);
    void window_updateFrameFromOSX       (struct Window* self);
    void window_getWindowFrame           (const struct Window* self, double* x, double* y, double* width, double* height);
    void window_getWindowPosition        (const struct Window* self, double* x, double* y);
    void window_getWindowSize            (const struct Window* self, double* width, double* height);
    void window_setWindowFrame           (struct Window* self, double x, double y, double width, double height);
    void window_setWindowPosition        (struct Window* self, double x, double y);
    void window_setWindowSize            (struct Window* self, double width, double height);
    

    // OpenGL Renderer API - as implemented in api_macos.c
    struct OpenGLRenderer* opengl_init    (void);
    void opengl_initialize                (struct OpenGLRenderer* self, struct Window* window);
    void opengl_setupContext              (struct OpenGLRenderer* self);
    void* opengl_getOpenGLContext         (const struct OpenGLRenderer* self);  // Returns id, cast to void*
    void* opengl_getCGLContextObj         (struct OpenGLRenderer* self);        // Returns CGLContextObj, cast to void*
    void* opengl_getCGLContextObjPtr      (struct OpenGLRenderer* self);        // Returns CGLContextObj as void*
    void opengl_makeCurrentContext        (struct OpenGLRenderer* self);        // Make OpenGL context current
    void opengl_setVsync                  (struct OpenGLRenderer* self, BOOL enabled); // Enable/disable vsync
    void opengl_destroy                   (struct OpenGLRenderer* self);

    // Image Loader API - as implemented in api_macos.c
    struct ImageLoader* imageloader_init        (void);
    BOOL imageloader_loadFromFile               (struct ImageLoader* self, const char* filePath);
    void imageloader_destroy                    (struct ImageLoader* self);
    unsigned char* imageloader_getPixelData     (const struct ImageLoader* self);
    void imageloader_getImageInfo               (const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel);
    void imageloader_getDetailedInfo            (const struct ImageLoader* self, int* width, int* height, int* bytesPerPixel, int* bytesPerRow, BOOL* hasAlpha);
    BOOL imageloader_isLoaded                   (const struct ImageLoader* self);
    BOOL imageloader_getPixel                   (const struct ImageLoader* self, int x, int y, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);
    unsigned int imageloader_createOpenGLTexture(const struct ImageLoader* self);
    
    // Autorelease pool management
    void* objc_autoreleasePoolPush  (void);
    void objc_autoreleasePoolPop    (void* pool);
    
    // OpenGL functions we might need
    void glDeleteTextures(int n, const unsigned int* textures);
    
    // ===========================================================================
    // EVENT HANDLING API
    // ===========================================================================
    
    // Event callback function types
    typedef void (*KeyEventCallback)        (unsigned short keyCode, const char* characters, void* userData);
    typedef void (*MouseEventCallback)      (double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData);
    //typedef void (*MouseMoveEventCallback)  (double x, double y, unsigned int modifierFlags, void* userData);
    
    // Event handler setup
    void window_setKeyDownCallback          (struct Window* self, KeyEventCallback callback, void* userData);
    void window_setKeyUpCallback            (struct Window* self, KeyEventCallback callback, void* userData);
    void window_setMouseDownCallback        (struct Window* self, MouseEventCallback callback, void* userData);
    void window_setMouseUpCallback          (struct Window* self, MouseEventCallback callback, void* userData);
    void window_setMouseMovedCallback       (struct Window* self, MouseEventCallback callback, void* userData);
    void window_setMouseDraggedCallback     (struct Window* self, MouseEventCallback callback, void* userData);
    void window_setRightMouseDownCallback   (struct Window* self, MouseEventCallback callback, void* userData);
    void window_setRightMouseUpCallback     (struct Window* self, MouseEventCallback callback, void* userData);
    void window_setOtherMouseDownCallback   (struct Window* self, MouseEventCallback callback, void* userData);
    void window_setOtherMouseUpCallback     (struct Window* self, MouseEventCallback callback, void* userData);
    void window_setScrollWheelCallback      (struct Window* self, void (*callback)(double, double, double, double, unsigned int, void*), void* userData);

    // Event system management
    void window_enableEventHandling     (struct Window* self);
    void window_disableEventHandling    (struct Window* self);
    
    // ===========================================================================
    // APPLICATION DELEGATE EVENT HANDLING API
    // ===========================================================================
    
    // Application delegate callback function type
    typedef void (*ApplicationDelegateCallback)(void* userData);
    
    // Application delegate event handler setup
    void application_setWillFinishLaunchingCallback (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setDidFinishLaunchingCallback  (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setWillTerminateCallback       (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setDidBecomeActiveCallback     (struct Application* self, ApplicationDelegateCallback callback, void* userData);
    void application_setWillResignActiveCallback    (struct Application* self, ApplicationDelegateCallback callback, void* userData);

    // ===========================================================================
    // WINDOW DELEGATE EVENT HANDLING API
    // ===========================================================================
    
    // Window delegate callback function type
    typedef void (*WindowDelegateCallback)(void* userData);
    
    // Window delegate event handler setup
    void window_setWindowDidResizeCallback          (struct Window* self, WindowDelegateCallback callback, void* userData);
    void window_setWindowWillCloseCallback          (struct Window* self, WindowDelegateCallback callback, void* userData);
    void window_setWindowDidBecomeKeyCallback       (struct Window* self, WindowDelegateCallback callback, void* userData);
    void window_setWindowDidResignKeyCallback       (struct Window* self, WindowDelegateCallback callback, void* userData);
    void window_setWindowDidMiniaturizeCallback     (struct Window* self, WindowDelegateCallback callback, void* userData);
    void window_setWindowDidDeminiaturizeCallback   (struct Window* self, WindowDelegateCallback callback, void* userData);
}

// C++ only function that returns C++ NSRect (not C-linkage compatible)
NSRect window_getSize(struct Window* self) noexcept;

#endif // OLC_HOST == OLC_HOST_MACOS
//! END DECLARATION
