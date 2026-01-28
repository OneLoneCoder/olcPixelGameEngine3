#pragma once
#include "config.h"

//! START STDHEADER GLOBAL
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <vector>
//! END STDHEADER

//! START CUSTOMHEADER
#include "api_macos.h"
//! END CUSTOMHEADER

//! START DECLARATION
namespace olc {
    namespace apis {
        namespace macos {

            // RAII helper for autorelease pools
            class AutoreleasePool {
            private:
                void* pool_;
                
            public:
                AutoreleasePool() noexcept : pool_(objc_autoreleasePoolPush()) {}
                
                ~AutoreleasePool() noexcept {
                    if (pool_) {
                        objc_autoreleasePoolPop(pool_);
                    }
                }
                
                // Non-copyable and non-movable for safety
                AutoreleasePool(const AutoreleasePool&) = delete;
                AutoreleasePool& operator=(const AutoreleasePool&) = delete;
                AutoreleasePool(AutoreleasePool&&) = delete;
                AutoreleasePool& operator=(AutoreleasePool&&) = delete;
            };
            
            // Exception class for framework errors
            class FrameworkException : public std::runtime_error {
            public:
                explicit FrameworkException(const std::string& message)
                    : std::runtime_error("MacOS Wrapper Framework Error: " + message) {}
            };
            
            // Application wrapper class
            class Application {
            private:
                struct ::Application* app_;
                
                // Store callback pointers for cleanup
                std::vector<std::unique_ptr<std::function<void()>>> callbacks_;
                
                // Template helper to reduce code duplication
                template<typename SetterFunc>
                void setCallback(SetterFunc setter, std::function<void()> callback) {
                    if (!app_) return;
                    
                    auto callbackPtr = std::make_unique<std::function<void()>>(std::move(callback));
                    auto* rawPtr = callbackPtr.get();
                    callbacks_.emplace_back(std::move(callbackPtr));
                    
                    setter(app_, [](void* userData) {
                        auto* cb = static_cast<std::function<void()>*>(userData);
                        (*cb)();
                    }, rawPtr);
                }
                
            public:
                explicit Application() : app_(nullptr) {
                    app_ = application_init();
                    if (!app_) {
                        throw FrameworkException("Failed to initialize application");
                    }
                }
                
                ~Application() {
                    if (app_) {
                        application_destroy(app_);  // application_destroy now handles delete internally
                    }
                }
                
                void initialize() noexcept {
                    if (app_) application_initialize(app_);
                }
                
                void activate() noexcept {
                    if (app_) application_activate(app_);
                }
                
                void run() noexcept {
                    if (app_) application_run(app_);
                }
                
                // Get underlying C handle
                struct ::Application* getCHandle() const noexcept { return app_; }
                              
                // Set callback for application will finish launching event
                void setWillFinishLaunchingCallback(std::function<void()> callback) {
                    setCallback(application_setWillFinishLaunchingCallback, std::move(callback));
                }
                
                // Set callback for application did finish launching event
                void setDidFinishLaunchingCallback(std::function<void()> callback) {
                    setCallback(application_setDidFinishLaunchingCallback, std::move(callback));
                }
                
                // Set callback for application will terminate event
                void setWillTerminateCallback(std::function<void()> callback) {
                    setCallback(application_setWillTerminateCallback, std::move(callback));
                }
                
                // Set callback for application did become active event
                void setDidBecomeActiveCallback(std::function<void()> callback) {
                    setCallback(application_setDidBecomeActiveCallback, std::move(callback));
                }
                
                // Set callback for application will resign active event
                void setWillResignActiveCallback(std::function<void()> callback) {
                    setCallback(application_setWillResignActiveCallback, std::move(callback));
                }
                
                // Non-copyable but movable
                Application(const Application&) = delete;
                Application& operator=(const Application&) = delete;
                Application(Application&& other) noexcept : app_(other.app_), callbacks_(std::move(other.callbacks_)) {
                    other.app_ = nullptr;
                }
                Application& operator=(Application&& other) noexcept {
                    if (this != &other) {
                        if (app_) {
                            application_destroy(app_);  // application_destroy now handles delete internally
                        }
                        app_ = other.app_;
                        callbacks_ = std::move(other.callbacks_);
                        other.app_ = nullptr;
                    }
                    return *this;
                }
            };
            
            // Window wrapper class
            class Window {
            private:
                struct ::Window* window_;
                std::string title_;
                std::vector<std::unique_ptr<std::function<void()>>> callbacks_;
                
                // Template helper to reduce code duplication
                template<typename SetterFunc>
                void setCallback(SetterFunc setter, std::function<void()> callback) {
                    if (!window_) return;
                    
                    auto callbackPtr = std::make_unique<std::function<void()>>(std::move(callback));
                    auto* rawPtr = callbackPtr.get();
                    callbacks_.emplace_back(std::move(callbackPtr));
                    
                    setter(window_, [](void* userData) {
                        auto* cb = static_cast<std::function<void()>*>(userData);
                        (*cb)();
                    }, rawPtr);
                }
                
            public:

                // Recommended constructor
                Window(int32_t width, int32_t height, const std::string& title, int32_t x = 100, int32_t y = 100)
                    : window_(nullptr), title_(title) {
                    Window(static_cast<double>(width), static_cast<double>(height), title,
                            static_cast<double>(x), static_cast<double>(y));
                }

                Window(float width, float height, const std::string& title, float x = 100, float y = 100)
                    : window_(nullptr), title_(title) {
                    Window(static_cast<double>(width), static_cast<double>(height), title,
                            static_cast<double>(x), static_cast<double>(y));
                }

                Window(double width, double height, const std::string& title, double x = 100.0, double y = 100.0)
                    : window_(nullptr), title_(title) {
                    window_ = window_init(x, y, width, height);
                    if (!window_) {
                        throw FrameworkException("Failed to create window");
                    }
                }
                
                ~Window() {
                    if (window_) {
                        window_destroy(window_);
                        free(window_);
                    }
                }

                // Get underlying C handle (Are you brave enough to use it?)
                struct ::Window* getCHandle() const noexcept { return window_; }
                
                // Create and show the window
                void show() {
                    if (window_) {
                        setTitle(title_);
                        window_create(window_);
                        window_show(window_);
                    }
                }

                // Update the internal frame from the OS X window
                void updateFrameFromOSX() {
                    if (window_) {
                        window_updateFrameFromOSX(window_);
                    }
                }
                
                const std::string& getTitle() const noexcept { return title_; }

                void setTitle(const std::string& title) {
                    title_ = title;
                    if (window_) {
                        window_setTitle(window_, title.c_str());
                    }
                }
                
                // Get the current window size and position
                NSRect getSize() const noexcept {
                    if (window_) {
                        NSRect frame{};
                        window_getWindowFrame(window_, &frame.x, &frame.y, &frame.width, &frame.height);
                        return frame;
                    }
                    return NSRect{0, 0, 0, 0};
                }

                // Set the window frame size
                void setFrameSize(int32_t width, int32_t height) noexcept {
                    setFrameSize(static_cast<double>(width), static_cast<double>(height));
                }

                void setFrameSize(float width, float height) noexcept {
                    setFrameSize(static_cast<double>(width), static_cast<double>(height));
                }

                void setFrameSize(double width, double height) noexcept {
                    if (window_) {
                        double x = 0.0, y = 0.0;
                        window_getWindowFrame(window_, &x, &y, nullptr, nullptr);
                        window_setWindowFrame(window_, x, y, width, height);
                    }
                }

                void getFrameSize(int32_t& width, int32_t& height) const noexcept {
                    double w = 0.0, h = 0.0;
                    getFrameSize(w, h);
                    width = static_cast<int32_t>(w);
                    height = static_cast<int32_t>(h);
                }

                void getFrameSize(float& width, float& height) const noexcept {
                    double w = 0.0, h = 0.0;
                    getFrameSize(w, h);
                    width = static_cast<float>(w);
                    height = static_cast<float>(h);
                }
                // Get the current window frame size
                void getFrameSize(double& width, double& height) const noexcept {
                    if (window_) {
                        window_getWindowFrame(window_, nullptr, nullptr, &width, &height);
                    } else {
                        width = height = 0.0;
                    }
                }

                // Set the window position
                void setPosition(int32_t x, int32_t y) noexcept {
                    setPosition(static_cast<double>(x), static_cast<double>(y));
                }

                void setPosition(float x, float y) noexcept {
                    setPosition(static_cast<double>(x), static_cast<double>(y));
                }

                void setPosition(double x, double y) noexcept {
                    if (window_) {
                        window_setWindowPosition(window_, x, y);
                    }
                }

                void getPosition(double& x, double& y) const noexcept {
                    if (window_) {
                        window_getWindowFrame(window_, &x, &y, nullptr, nullptr);
                    } else {
                        x = y = 0.0;
                    }
                }

                // Set the window size
                void setWindowSize(int32_t width, int32_t height) noexcept {
                    setWindowSize(static_cast<double>(width), static_cast<double>(height));
                }

                void setWindowSize(float width, float height) noexcept {
                    setWindowSize(static_cast<double>(width), static_cast<double>(height));
                }
                
                void setWindowSize(double width, double height) noexcept {
                    if (window_) {
                        window_setWindowSize(window_, width, height);
                    }
                }

                void getWindowSize(int32_t& width, int32_t& height) const noexcept {
                    double w = 0.0, h = 0.0;
                    getWindowSize(w, h);
                    width = static_cast<int32_t>(w);
                    height = static_cast<int32_t>(h);
                }

                void getWindowSize(float& width, float& height) const noexcept {
                    double w = 0.0, h = 0.0;
                    getWindowSize(w, h);
                    width = static_cast<float>(w);
                    height = static_cast<float>(h);
                }
                                
                // Get the current window dimensions
                void getWindowSize(double& width, double& height) const noexcept {
                    if (window_) {
                        window_getWindowSize(window_, &width, &height);
                    } else {
                        width = height = 0.0;
                    }
                }

                // Context view frame getters and setters
                void setContentViewPosition(int32_t x, int32_t y) noexcept {
                    setContentViewPosition(static_cast<double>(x), static_cast<double>(y));
                }
                void setContentViewPosition(float x, float y) noexcept {
                    setContentViewPosition(static_cast<double>(x), static_cast<double>(y));
                }
                void setContentViewPosition(double x, double y) noexcept {
                    if (window_) {
                        NSRect frame{};
                        window_getContentViewFrame(window_, nullptr, nullptr, &frame.width, &frame.height);
                        window_setContentViewFrame(window_, &x, &y, &frame.width, &frame.height);
                    }
                }

                void setContentViewSize(int32_t width, int32_t height) noexcept {
                    setContentViewSize(static_cast<double>(width), static_cast<double>(height));
                }

                void setContentViewSize(float width, float height) noexcept {
                    setContentViewSize(static_cast<double>(width), static_cast<double>(height));
                }

                void setContentViewSize(double width, double height) noexcept {
                    if(window_) {
                        NSRect frame{};
                        window_getContentViewFrame(window_, &frame.x, &frame.y, nullptr, nullptr);
                        window_setContentViewFrame(window_, &frame.x, &frame.y, &width, &height);
                    }
                }

                void setContentViewFrame(NSRect& frame) noexcept {
                    if (window_) {
                        window_getContentViewFrame(window_, &frame.x, &frame.y, &frame.width, &frame.height);
                    } else {
                        frame = NSRect{0, 0, 0, 0};
                    }
                }

                void getContentViewSize(int32_t& width, int32_t& height) const noexcept {
                    double w = 0.0, h = 0.0;
                    getContentViewSize(w, h);
                    width = static_cast<int32_t>(w);
                    height = static_cast<int32_t>(h);
                }

                void getContentViewSize(float& width, float& height) const noexcept {
                    double w = 0.0, h = 0.0;
                    getContentViewSize(w, h);
                    width = static_cast<float>(w);
                    height = static_cast<float>(h);
                }

                void getContentViewSize(double& width, double& height) const noexcept {
                    NSRect frame{};
                    getContentViewFrame(frame);
                    width = frame.width;
                    height = frame.height;
                }

                void getContentViewPosition(double& x, double& y) const noexcept {
                    NSRect frame{};
                    getContentViewFrame(frame);
                    x = frame.x;
                    y = (frame.y);
                }

                void getContentViewFrame(NSRect& frame) const noexcept {
                    if (window_) {
                        window_getContentViewFrame(window_, &frame.x, &frame.y, &frame.width, &frame.height);
                    } else {
                        frame = NSRect{0, 0, 0, 0};
                    }
                }

                // Set callback for window resize events
                void setWindowDidResizeCallback(std::function<void()> callback) {
                    setCallback(window_setWindowDidResizeCallback, std::move(callback));
                }
                
                // Set callback for window will close events
                void setWindowWillCloseCallback(std::function<void()> callback) {
                    setCallback(window_setWindowWillCloseCallback, std::move(callback));
                }
                
                // Set callback for window did become key events
                void setWindowDidBecomeKeyCallback(std::function<void()> callback) {
                    setCallback(window_setWindowDidBecomeKeyCallback, std::move(callback));
                }
                
                // Set callback for window did resign key events
                void setWindowDidResignKeyCallback(std::function<void()> callback) {
                    setCallback(window_setWindowDidResignKeyCallback, std::move(callback));
                }
                
                // Set callback for window miniaturize events
                void setWindowDidMiniaturizeCallback(std::function<void()> callback) {
                    setCallback(window_setWindowDidMiniaturizeCallback, std::move(callback));
                }
                
                // Set callback for window deminiaturize events
                void setWindowDidDeminiaturizeCallback(std::function<void()> callback) {
                    setCallback(window_setWindowDidDeminiaturizeCallback, std::move(callback));
                }
                
                // Non-copyable but movable
                Window(const Window&) = delete;
                Window& operator=(const Window&) = delete;
                Window(Window&& other) noexcept
                    : window_(other.window_), title_(std::move(other.title_)), callbacks_(std::move(other.callbacks_)) {
                    other.window_ = nullptr;
                }
                Window& operator=(Window&& other) noexcept {
                    if (this != &other) {
                        if (window_) {
                            window_destroy(window_);
                            free(window_);
                        }
                        window_ = other.window_;
                        title_ = std::move(other.title_);
                        callbacks_ = std::move(other.callbacks_);
                        other.window_ = nullptr;
                    }
                    return *this;
                }
            };
            
            // OpenGL renderer wrapper class
            class OpenGLRenderer {
            private:
                struct ::OpenGLRenderer* renderer_;
                
            public:
                explicit OpenGLRenderer() : renderer_(nullptr) {
                    renderer_ = opengl_init();
                    if (!renderer_) {
                        throw FrameworkException("Failed to create OpenGL renderer");
                    }
                }
                
                ~OpenGLRenderer() {
                    if (renderer_) {
                        opengl_destroy(renderer_);
                    }
                }
                
                void attachToWindow(Window& window) noexcept {
                    if (renderer_) {
                        opengl_initialize(renderer_, window.getCHandle());
                    }
                }
                
                void setupContext() noexcept {
                    if (renderer_) {
                        opengl_setupContext(renderer_);
                    }
                }
                
                bool resetContextSize(int32_t width, int32_t height) noexcept {
                    return resetContextSize(static_cast<double>(width), static_cast<double>(height));
                }

                bool resetContextSize(float width, float height) noexcept {
                    return resetContextSize(static_cast<double>(width), static_cast<double>(height));
                }

                bool resetContextSize(double width, double height) noexcept {
                    if (renderer_) {
                        return opengl_resetContextForSize(renderer_, width, height);
                    }
                    return false;
                }
                
                void* getOpenGLContext() const noexcept {
                    return renderer_ ? opengl_getOpenGLContext(renderer_) : nullptr;
                }
                
                void* getCGLContextObj() const noexcept {
                    return renderer_ ? opengl_getCGLContextObj(renderer_) : nullptr;
                }
                
                void* getCGLContextObjPtr() const noexcept {
                    return renderer_ ? opengl_getCGLContextObjPtr(renderer_) : nullptr;
                }

                void makeCurrentContext() const noexcept {
                    if (renderer_) {
                        opengl_makeCurrentContext(renderer_);
                    }
                }

                void setVsync(bool enabled) const noexcept {
                    if (renderer_) {
                        opengl_setVsync(renderer_, enabled ? 1 : 0);
                    }
                }
                
                void enableVsync() const noexcept {
                    setVsync(true);
                }
                
                void disableVsync() const noexcept {
                    setVsync(false);
                }

                // Delete copy constructor and assignment
                OpenGLRenderer(const OpenGLRenderer&) = delete;
                OpenGLRenderer& operator=(const OpenGLRenderer&) = delete;
                OpenGLRenderer(OpenGLRenderer&& other) noexcept : renderer_(other.renderer_) {
                    other.renderer_ = nullptr;
                }
                OpenGLRenderer& operator=(OpenGLRenderer&& other) noexcept {
                    if (this != &other) {
                        if (renderer_) {
                            opengl_destroy(renderer_);
                            // Note: opengl_destroy already calls free() on the renderer
                        }
                        renderer_ = other.renderer_;
                        other.renderer_ = nullptr;
                    }
                    return *this;
                }
            };
            
            // Image loader wrapper class
            class ImageLoader {
            private:
                struct ::ImageLoader* loader_;
                std::string filePath_;
                bool loaded_;
                
            public:
                explicit ImageLoader() : loader_(nullptr), loaded_(false) {
                    loader_ = imageloader_init();
                    if (!loader_) {
                        throw FrameworkException("Failed to create image loader");
                    }
                }
                
                ~ImageLoader() {
                    if (loader_) {
                        imageloader_destroy(loader_);
                        free(loader_);
                    }
                }
                
                bool loadFromFile(const std::string& filePath) {
                    filePath_ = filePath;
                    if (loader_) {
                        BOOL result = imageloader_loadFromFile(loader_, filePath.c_str());
                        loaded_ = (result != 0);
                        return loaded_;
                    }
                    return false;
                }
                
                bool isLoaded() const noexcept {
                    return loaded_ && loader_ && imageloader_isLoaded(loader_);
                }

                void getImageInfo(int& width, int& height, int& bytesPerPixel) const noexcept {
                    if (loader_ && loaded_) {
                        imageloader_getImageInfo(loader_, &width, &height, &bytesPerPixel);
                    } else {
                        width = height = bytesPerPixel = 0;
                    }
                }
                
                const std::string& getFilePath() const noexcept { return filePath_; }
                struct ::ImageLoader* getCHandle() const noexcept { return loader_; }
                
                // Non-copyable but movable
                ImageLoader(const ImageLoader&) = delete;
                ImageLoader& operator=(const ImageLoader&) = delete;
                ImageLoader(ImageLoader&& other) noexcept
                    : loader_(other.loader_), filePath_(std::move(other.filePath_)), loaded_(other.loaded_) {
                    other.loader_ = nullptr;
                    other.loaded_ = false;
                }
                ImageLoader& operator=(ImageLoader&& other) noexcept {
                    if (this != &other) {
                        if (loader_) {
                            imageloader_destroy(loader_);
                            free(loader_);
                        }
                        loader_ = other.loader_;
                        filePath_ = std::move(other.filePath_);
                        loaded_ = other.loaded_;
                        other.loader_ = nullptr;
                        other.loaded_ = false;
                    }
                    return *this;
                }
            };
            
            // Keyboard event data structure
            struct KeyEvent {
                unsigned short keyCode;
                std::string characters;
                
                KeyEvent(unsigned short code, const char* chars) noexcept
                    : keyCode(code), characters(chars ? chars : "") {}
                
                // Move constructor and assignment for better performance
                KeyEvent(KeyEvent&&) noexcept = default;
                KeyEvent& operator=(KeyEvent&&) noexcept = default;
                KeyEvent(const KeyEvent&) = default;
                KeyEvent& operator=(const KeyEvent&) = default;
            };
            
            // Mouse event data structure
            struct MouseEvent {
                double x, y;
                int buttonNumber;
                unsigned int modifierFlags;
                
                constexpr MouseEvent(double x, double y, int buttonNum, unsigned int flags) noexcept
                    : x(x), y(y), buttonNumber(buttonNum), modifierFlags(flags) {}
                
                // Default move and copy semantics
                MouseEvent(MouseEvent&&) noexcept = default;
                MouseEvent& operator=(MouseEvent&&) noexcept = default;
                MouseEvent(const MouseEvent&) = default;
                MouseEvent& operator=(const MouseEvent&) = default;
            };

            // Scroll wheel event data structure
            struct ScrollWheelEvent {
                double x, y;
                double deltaX, deltaY;
                unsigned int modifierFlags;

                ScrollWheelEvent(double x, double y, double deltaX, double deltaY, unsigned int flags) noexcept
                    : x(x), y(y), deltaX(deltaX), deltaY(deltaY), modifierFlags(flags) {}
            };

            // Event handler class for keyboard and mouse events
            class EventHandler {
            private:
                Window& window_;
                std::function<void(const KeyEvent&)>    keyDownHandler_;
                std::function<void(const KeyEvent&)>    keyUpHandler_;
                std::function<void(const MouseEvent&)>  mouseDownHandler_;
                std::function<void(const MouseEvent&)>  mouseUpHandler_;
                std::function<void(const MouseEvent&)>  mouseMovedHandler_;
                std::function<void(const MouseEvent&)>  mouseDraggedHandler_;
                std::function<void(const MouseEvent&)>  rightMouseDownHandler_;
                std::function<void(const MouseEvent&)>  rightMouseUpHandler_;
                std::function<void(const MouseEvent&)>  rightMouseDraggedHandler_;
                std::function<void(const MouseEvent&)>  otherMouseDownHandler_;
                std::function<void(const MouseEvent&)>  otherMouseUpHandler_;
                std::function<void(const MouseEvent&)>  otherMouseDraggedHandler_;
                std::function<void(const ScrollWheelEvent&)> scrollWheelHandler_;
               
                // Template helpers for static callbacks to reduce code duplication
                template<typename EventType, typename HandlerType>
                static void keyCallback(unsigned short keyCode, const char* characters, void* userData, HandlerType EventHandler::*handler) {
                    auto* eventHandler = static_cast<EventHandler*>(userData);
                    if (eventHandler && (eventHandler->*handler)) {
                        (eventHandler->*handler)(KeyEvent(keyCode, characters));
                    }
                }
                
                template<typename EventType, typename HandlerType>
                static void mouseCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData, HandlerType EventHandler::*handler) {
                    auto* eventHandler = static_cast<EventHandler*>(userData);
                    if (eventHandler && (eventHandler->*handler)) {
                        (eventHandler->*handler)(MouseEvent(x, y, buttonNumber, modifierFlags));
                    }
                }
                
                // Static callback functions for C API
                static void keyDownCallback(unsigned short keyCode, const char* characters, void* userData) {
                    keyCallback<KeyEvent>(keyCode, characters, userData, &EventHandler::keyDownHandler_);
                }
                
                static void keyUpCallback(unsigned short keyCode, const char* characters, void* userData) {
                    keyCallback<KeyEvent>(keyCode, characters, userData, &EventHandler::keyUpHandler_);
                }
                
                static void mouseDownCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    mouseCallback<MouseEvent>(x, y, buttonNumber, modifierFlags, userData, &EventHandler::mouseDownHandler_);
                }

                static void mouseUpCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    mouseCallback<MouseEvent>(x, y, buttonNumber, modifierFlags, userData, &EventHandler::mouseUpHandler_);
                }

                static void mouseMovedCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    auto* eventHandler = static_cast<EventHandler*>(userData);
                    if (eventHandler && eventHandler->mouseMovedHandler_) {
                        eventHandler->mouseMovedHandler_(MouseEvent(x, y, buttonNumber, modifierFlags));
                    }
                }

                static void mouseDraggedCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    auto* eventHandler = static_cast<EventHandler*>(userData);
                    if (eventHandler && eventHandler->mouseDraggedHandler_) {
                        eventHandler->mouseDraggedHandler_(MouseEvent(x, y, buttonNumber, modifierFlags));
                    }
                }

                static void rightMouseDownCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    mouseCallback<MouseEvent>(x, y, buttonNumber, modifierFlags, userData, &EventHandler::rightMouseDownHandler_);
                }

                static void rightMouseUpCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    mouseCallback<MouseEvent>(x, y, buttonNumber, modifierFlags, userData, &EventHandler::rightMouseUpHandler_);
                }

                static void rightMouseDraggedCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    auto* eventHandler = static_cast<EventHandler*>(userData);
                    if (eventHandler && eventHandler->rightMouseDraggedHandler_) {
                        eventHandler->rightMouseDraggedHandler_(MouseEvent(x, y, buttonNumber, modifierFlags));
                    }
                }

                static void otherMouseDownCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    mouseCallback<MouseEvent>(x, y, buttonNumber, modifierFlags, userData, &EventHandler::otherMouseDownHandler_);
                }

                static void otherMouseUpCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    mouseCallback<MouseEvent>(x, y, buttonNumber, modifierFlags, userData, &EventHandler::otherMouseUpHandler_);
                }

                static void otherMouseDraggedCallback(double x, double y, int buttonNumber, unsigned int modifierFlags, void* userData) {
                    auto* eventHandler = static_cast<EventHandler*>(userData);
                    if (eventHandler && eventHandler->otherMouseDraggedHandler_) {
                        eventHandler->otherMouseDraggedHandler_(MouseEvent(x, y, buttonNumber, modifierFlags));
                    }
                }

                static void scrollWheelCallback(double x, double y, double deltaX, double deltaY, unsigned int modifierFlags, void* userData) {
                    auto* eventHandler = static_cast<EventHandler*>(userData);
                    if (eventHandler && eventHandler->scrollWheelHandler_) {
                        eventHandler->scrollWheelHandler_(ScrollWheelEvent(x, y, deltaX, deltaY, modifierFlags));
                    }
                }

                // Template helper for setting event handlers to reduce repetition
                template<typename HandlerType, typename SetterFunc, typename CallbackFunc>
                void setEventHandler(HandlerType EventHandler::*member, SetterFunc setter, CallbackFunc callback, std::function<void(const typename HandlerType::element_type&)> handler) {
                    this->*member = std::move(handler);
                    setter(window_.getCHandle(), callback, this);
                }
                
            public:
                explicit EventHandler(Window& window) noexcept : window_(window) {}
                
                ~EventHandler() noexcept {
                    disable();
                }
                
                // Event handler setters - now using template helper
                void onKeyDown(std::function<void(const KeyEvent&)> handler) {
                    keyDownHandler_ = std::move(handler);
                    window_setKeyDownCallback(window_.getCHandle(), keyDownCallback, this);
                }
                
                void onKeyUp(std::function<void(const KeyEvent&)> handler) {
                    keyUpHandler_ = std::move(handler);
                    window_setKeyUpCallback(window_.getCHandle(), keyUpCallback, this);
                }
                
                void onMouseDown(std::function<void(const MouseEvent&)> handler) {
                    mouseDownHandler_ = std::move(handler);
                    window_setMouseDownCallback(window_.getCHandle(), mouseDownCallback, this);
                }
                
                void onMouseUp(std::function<void(const MouseEvent&)> handler) {
                    mouseUpHandler_ = std::move(handler);
                    window_setMouseUpCallback(window_.getCHandle(), mouseUpCallback, this);
                }
                
                void onMouseMoved(std::function<void(const MouseEvent&)> handler) {
                    mouseMovedHandler_ = std::move(handler);
                    window_setMouseMovedCallback(window_.getCHandle(), mouseMovedCallback, this);
                }
                
                void onMouseDragged(std::function<void(const MouseEvent&)> handler) {
                    mouseDraggedHandler_ = std::move(handler);
                    window_setMouseDraggedCallback(window_.getCHandle(), mouseDraggedCallback, this);
                }
                
                void onRightMouseDown(std::function<void(const MouseEvent&)> handler) {
                    rightMouseDownHandler_ = std::move(handler);
                    window_setRightMouseDownCallback(window_.getCHandle(), rightMouseDownCallback, this);
                }

                void onRightMouseDragged(std::function<void(const MouseEvent&)> handler) {
                    rightMouseDraggedHandler_ = std::move(handler);
                    window_setRightMouseDraggedCallback(window_.getCHandle(), rightMouseDraggedCallback, this);
                }
                
                void onRightMouseUp(std::function<void(const MouseEvent&)> handler) {
                    rightMouseUpHandler_ = std::move(handler);
                    window_setRightMouseUpCallback(window_.getCHandle(), rightMouseUpCallback, this);
                }

                void onOtherMouseDown(std::function<void(const MouseEvent&)> handler) {
                    otherMouseDownHandler_ = std::move(handler);
                    window_setOtherMouseDownCallback(window_.getCHandle(), otherMouseDownCallback, this);
                }

                void onOtherMouseUp(std::function<void(const MouseEvent&)> handler) {
                    otherMouseUpHandler_ = std::move(handler);
                    window_setOtherMouseUpCallback(window_.getCHandle(), otherMouseUpCallback, this);
                }

                void onOtherMouseDragged(std::function<void(const MouseEvent&)> handler) {
                    otherMouseDraggedHandler_ = std::move(handler);
                    window_setOtherMouseDraggedCallback(window_.getCHandle(), otherMouseDraggedCallback, this);
                }

                void onScrollWheel(std::function<void(const ScrollWheelEvent&)> handler) {
                    scrollWheelHandler_ = std::move(handler);
                    window_setScrollWheelCallback(window_.getCHandle(), scrollWheelCallback, this);
                }

                // Enable/disable event handling
                void enable() noexcept {
                    window_enableEventHandling(window_.getCHandle());
                }
                
                void disable() noexcept {
                    window_disableEventHandling(window_.getCHandle());
                }
                
                // Non-copyable and non-movable (due to reference member)
                EventHandler(const EventHandler&) = delete;
                EventHandler& operator=(const EventHandler&) = delete;
                EventHandler(EventHandler&&) = delete;
                EventHandler& operator=(EventHandler&&) = delete;
            };
            
        } // namespace macos
    } // namespace apis
} // namespace olc

//! END DECLARATION
