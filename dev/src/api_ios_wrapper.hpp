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
#include "api_ios.h"
//! END CUSTOMHEADER

//! START DECLARATION
namespace olc {
    namespace apis {
        namespace ios {

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
            
            // RAII wrapper for OpenGL ES texture management
            class OpenGLTexture {
            private:
                unsigned int textureID_;
                
            public:
                explicit constexpr OpenGLTexture(unsigned int id = 0) noexcept : textureID_(id) {}
                
                ~OpenGLTexture() noexcept {
                    if (textureID_ != 0) {
                        glDeleteTextures(1, &textureID_);
                    }
                }
                
                constexpr unsigned int get() const noexcept { return textureID_; }
                constexpr bool isValid() const noexcept { return textureID_ != 0; }
                
                // Release ownership
                unsigned int release() noexcept {
                    unsigned int id = textureID_;
                    textureID_ = 0;
                    return id;
                }
                
                // Reset with new texture ID
                void reset(unsigned int id = 0) noexcept {
                    if (textureID_ != 0) {
                        glDeleteTextures(1, &textureID_);
                    }
                    textureID_ = id;
                }
                
                // Move semantics
                OpenGLTexture(OpenGLTexture&& other) noexcept : textureID_(other.textureID_) {
                    other.textureID_ = 0;
                }
                
                OpenGLTexture& operator=(OpenGLTexture&& other) noexcept {
                    if (this != &other) {
                        reset();
                        textureID_ = other.textureID_;
                        other.textureID_ = 0;
                    }
                    return *this;
                }
                
                // Non-copyable
                OpenGLTexture(const OpenGLTexture&) = delete;
                OpenGLTexture& operator=(const OpenGLTexture&) = delete;
            };

            // Exception class for framework errors
            class FrameworkException : public std::runtime_error {
            public:
                explicit FrameworkException(const std::string& message)
                    : std::runtime_error("iOS Wrapper Framework Error: " + message) {}
            };
            
            // Touch event structure
            struct TouchEvent {
                uint32_t touchID;
                double x, y;
                double sizeX, sizeY;

                TouchEvent(uint32_t id, double x, double y, double sx, double sy) noexcept
                    : touchID(id), x(x), y(y), sizeX(sx), sizeY(sy) {}

                TouchEvent(TouchEvent&&) noexcept = default;
                TouchEvent& operator=(TouchEvent&&) noexcept = default;
                TouchEvent(const TouchEvent&) = default;
                TouchEvent& operator=(const TouchEvent&) = default;
            };
            
            // Key event structure (for hardware keyboards)
            struct KeyEvent {
                unsigned short keyCode;
                std::string characters;
            };
            
            // Device orientation enumeration
            enum class DeviceOrientation {
                Unknown = 0,
                Portrait = 1,
                PortraitUpsideDown = 2,
                LandscapeLeft = 3,
                LandscapeRight = 4,
                FaceUp = 5,
                FaceDown = 6
            };
            
            // Application wrapper class
            class Application {
            private:
                struct ::Application* app_;
                
                std::string appPath_;
                
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
                        throw FrameworkException("Failed to initialize iOS application");
                    }
                }
                
                ~Application() {
                    if (app_) {
                        application_destroy(app_);
                    }
                }
                
                // Sets the iOS internal path to the application
                void setApplicationPath(const std::string& path) {
                    if (app_) {
                        appPath_ = path;
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
                
                // Set callback for application did enter background event
                void setDidEnterBackgroundCallback(std::function<void()> callback) {
                    setCallback(application_setDidEnterBackgroundCallback, std::move(callback));
                }
                
                // Set callback for application will enter foreground event
                void setWillEnterForegroundCallback(std::function<void()> callback) {
                    setCallback(application_setWillEnterForegroundCallback, std::move(callback));
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
                            application_destroy(app_);
                        }
                        app_ = other.app_;
                        callbacks_ = std::move(other.callbacks_);
                        other.app_ = nullptr;
                    }
                    return *this;
                }
            };
            
            // View Controller wrapper class
            class ViewController {
            private:
                struct ::ViewController* viewController_;
                                
                // Template helper for simple callbacks
                template<typename SetterFunc>
                void setCallback(SetterFunc setter, std::function<void()> callback) {
                    if (!viewController_) return;
                    
                    auto callbackPtr = std::make_unique<std::function<void()>>(std::move(callback));
                    auto* rawPtr = callbackPtr.get();
                    callbacks_.emplace_back(std::move(callbackPtr));
                    
                    setter(viewController_, [](void* userData) {
                        auto* cb = static_cast<std::function<void()>*>(userData);
                        (*cb)();
                    }, rawPtr);
                }
                
                // Template helper for orientation changed callback
                template<typename SetterFunc>
                void setOrientCallback(SetterFunc setter, std::function<void(DeviceOrientation)> callback) {
                    if (!viewController_) return;
                    
                    auto callbackPtr = std::make_unique<std::function<void(DeviceOrientation)>>(std::move(callback));
                    auto* rawPtr = callbackPtr.get();
                    orientationCallbacks_.emplace_back(std::move(callbackPtr));
                    
                    setter(viewController_, [](int orientation, void* userData) {
                        auto* cb = static_cast<std::function<void(DeviceOrientation)>*>(userData);
                        (*cb)(static_cast<DeviceOrientation>(orientation));
                    }, rawPtr);
                    
                }
                
            public:
                std::string title_;
                std::vector<std::unique_ptr<std::function<void()>>> callbacks_;
                std::vector<std::unique_ptr<std::function<void(DeviceOrientation)>>> orientationCallbacks_;
                
            public:
                ViewController(double width, double height, const std::string& title)
                    : viewController_(nullptr), title_(title) {
                    viewController_ = viewcontroller_init(width, height);
                    if (!viewController_) {
                        throw FrameworkException("Failed to create iOS view controller");
                    }
                }
                
                ~ViewController() {
                    if (viewController_) {
                        viewcontroller_destroy(viewController_);
                    }
                }

                // Get underlying C handle
                struct ::ViewController* getCHandle() const noexcept { return viewController_; }
                
                // Present the view controller
                void present() {
                    if (viewController_) {
                        viewcontroller_present(viewController_);
                    }
                }

                // Update the internal frame from the iOS view
                void updateViewFromiOS() {
                    if (viewController_) {
                        viewcontroller_updateViewFromiOS(viewController_);
                    }
                }
                
                const std::string& getTitle() const noexcept { return title_; }

                void setTitle(const std::string& title) {
                    title_ = title;
                    if (viewController_) {
                        viewcontroller_setTitle(viewController_, title.c_str());
                    }
                }
                
                void setPaused(bool paused) noexcept {
                    if (viewController_) {
                        viewcontroller_setPause(viewController_, paused);
                    }
                }
                
                // Get the current view size
                CGRect getSize() const noexcept {
                    if (viewController_) {
                        CGRect frame{};
                        viewcontroller_getViewFrame(viewController_, &frame.origin.x, &frame.origin.y, &frame.size.width, &frame.size.height);
                        return frame;
                    }
                    return CGRect{0, 0, 0, 0};
                }

                void getViewSize(double& width, double& height) const noexcept {
                    if (viewController_) {
                        viewcontroller_getViewSize(viewController_, &width, &height);
                    } else {
                        width = height = 0.0;
                    }
                }
                
                // Set the view frame size
                void setFrameSize(double width, double height) noexcept {
                    if (viewController_) {
                        double x = 0.0, y = 0.0;
                        viewcontroller_getViewFrame(viewController_, &x, &y, nullptr, nullptr);
                        viewcontroller_setViewFrame(viewController_, x, y, width, height);
                    }
                }
                
                void setPosition(double x, double y) noexcept {
                    if (viewController_) {
                        double width = 0.0, height = 0.0;
                        viewcontroller_getViewFrame(viewController_, nullptr, nullptr, &width, &height);
                        viewcontroller_setViewFrame(viewController_, x, y, width, height);
                    }
                }

                // View controller lifecycle callbacks
                void setViewDidLoadCallback(std::function<void()> callback) {
                    setCallback(viewcontroller_setViewDidLoadCallback, std::move(callback));
                }
                
                void setViewWillAppearCallback(std::function<void()> callback) {
                    setCallback(viewcontroller_setViewWillAppearCallback, std::move(callback));
                }
                
                void setViewDidAppearCallback(std::function<void()> callback) {
                    setCallback(viewcontroller_setViewDidAppearCallback, std::move(callback));
                }
                
                void setViewWillDisappearCallback(std::function<void()> callback) {
                    setCallback(viewcontroller_setViewWillDisappearCallback, std::move(callback));
                }
                
                void setViewDidDisappearCallback(std::function<void()> callback) {
                    setCallback(viewcontroller_setViewDidDisappearCallback, std::move(callback));
                }
                
                void setViewDidLayoutSubviewsCallback(std::function<void()> callback) {
                    setCallback(viewcontroller_setViewDidLayoutSubviewsCallback, std::move(callback));
                }
                
                void setUpdateCallback(std::function<void()> callback) {
                    setCallback(viewcontroller_setUpdateCallback, std::move(callback));
                }
                
                void setOrientationChangedCallback(std::function<void(DeviceOrientation)> callback) {
                    setOrientCallback(viewcontroller_setOrientationChangedCallback, std::move(callback));

                }
                
                // Non-copyable but movable
                ViewController(const ViewController&) = delete;
                ViewController& operator=(const ViewController&) = delete;
                ViewController(ViewController&& other) noexcept 
                    : viewController_(other.viewController_), title_(std::move(other.title_)), 
                      callbacks_(std::move(other.callbacks_)),
                      orientationCallbacks_(std::move(other.orientationCallbacks_)) {
                    other.viewController_ = nullptr;
                }
                
                ViewController& operator=(ViewController&& other) noexcept {
                    if (this != &other) {
                        if (viewController_) {
                            viewcontroller_destroy(viewController_);
                        }
                        viewController_ = other.viewController_;
                        title_ = std::move(other.title_);
                        callbacks_ = std::move(other.callbacks_);
                        orientationCallbacks_ = std::move(other.orientationCallbacks_);
                        other.viewController_ = nullptr;
                    }
                    return *this;
                }
            };
            
            // GLKView wrapper class
            class GLKView {
            private:
                struct ::GLKView* glkView_;
                ViewController* parentViewController_;
                
                 // Touch event handlers
                std::function<void(const TouchEvent&)>  touchBeganHandler_;
                std::function<void(const TouchEvent&)>  touchMovedHandler_;
                std::function<void(const TouchEvent&)>  touchEndedHandler_;
                std::function<void(const TouchEvent&)>  touchCancelledHandler_;

                std::function<void(int, const std::string&)> keyDownHandler;
                std::function<void(int, const std::string&)> keyUpHandler;
                std::function<void(double, double, double, double)> drawHandler;
                
                // Store callback pointers for cleanup  
                std::vector<std::unique_ptr<std::function<void(const TouchEvent&)>>> touchCallbacks_;
                std::vector<std::unique_ptr<std::function<void(const std::vector<TouchEvent>&)>>> multiTouchCallbacks_;
                std::vector<std::unique_ptr<std::function<void(const KeyEvent&)>>> keyCallbacks_;
                
                // Re-register all callbacks after GLKView initialization
                void reregisterCallbacks() {
                    if (!glkView_) return;
                    
                    if (touchBeganHandler_) {
                        glkview_setTouchBeganCallback(glkView_, [](uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->touchBeganHandler_) {
                                handler->touchBeganHandler_(TouchEvent(touchID, x, y, sizeX, sizeY));
                            }
                        }, this);
                    }
                    
                    if (touchMovedHandler_) {
                        glkview_setTouchMovedCallback(glkView_, [](uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->touchMovedHandler_) {
                                handler->touchMovedHandler_(TouchEvent(touchID, x, y, sizeX, sizeY));
                            }
                        }, this);
                    }
                    
                    if (touchEndedHandler_) {
                        glkview_setTouchEndedCallback(glkView_, [](uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->touchEndedHandler_) {
                                handler->touchEndedHandler_(TouchEvent(touchID, x, y, sizeX, sizeY));
                            }
                        }, this);
                    }
                    
                    if (touchCancelledHandler_) {
                        glkview_setTouchCancelledCallback(glkView_, [](uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->touchCancelledHandler_) {
                                handler->touchCancelledHandler_(TouchEvent(touchID, x, y, sizeX, sizeY));
                            }
                        }, this);
                    }
                    
                    if (keyDownHandler) {
                        glkview_setKeyDownCallback(glkView_, [](unsigned short keyCode, const char* characters, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->keyDownHandler) {
                                handler->keyDownHandler(keyCode, std::string(characters ? characters : ""));
                            }
                        }, this);
                    }
                    
                    if (keyUpHandler) {
                        glkview_setKeyUpCallback(glkView_, [](unsigned short keyCode, const char* characters, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->keyUpHandler) {
                                handler->keyUpHandler(keyCode, std::string(characters ? characters : ""));
                            }
                        }, this);
                    }
                    
                    
                    if (drawHandler) {
                        glkview_setDrawCallback(glkView_, [](double x, double y, double width, double height, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->drawHandler) {
                                handler->drawHandler(x, y, width, height);
                            }
                        }, this);
                    }
                }
                
            public:
                explicit GLKView(ViewController& parentViewController) 
                    : glkView_(nullptr), parentViewController_(&parentViewController) {
                }
                
                ~GLKView() {
                    // Note: The underlying GLKView is managed by ViewController, so we don't destroy it here
                }
                
                // Initialize GLKView after ViewController presentation
                bool initialize() {
                    if (parentViewController_ && parentViewController_->getCHandle()) {
                        // Get the GLKView struct from the presented ViewController
                        glkView_ = glkview_init(parentViewController_->getCHandle());
                        if (glkView_) {
                            // Re-register all callbacks that were set before initialization
                            reregisterCallbacks();
                            return true;
                        }
                    }
                    return false;
                }
                
                // Get underlying C handle
                struct ::GLKView* getCHandle() const noexcept { return glkView_; }
                
                // Get native view pointer
                void* getNativeView() const {
                    return glkView_ ? glkview_getNativeView(glkView_) : nullptr;
                }
                
                // Frame management
                void setFrame(double x, double y, double width, double height) {
                    if (glkView_) {
                        glkview_setFrame(glkView_, x, y, width, height);
                    }
                }
                
                // Get the current frame of the GLKView
                CGRect getFrame(double x, double y, double width, double height) const {
                    if (glkView_) {
                        CGRect frame{};
                        glkview_getFrame(glkView_, &frame.origin.x, &frame.origin.y, &frame.size.width, &frame.size.height);
                        return frame;
                    }
                    return CGRect{0, 0, 0, 0};
                }
                
                // Touch handling management
                void setMultipleTouchEnabled(bool enabled) {
                    if (glkView_) {
                        glkview_setMultipleTouchEnabled(glkView_, enabled ? YES : NO);
                    }
                }
                
                bool isMultipleTouchEnabled() const {
                    return glkView_ ? glkview_isMultipleTouchEnabled(glkView_) != NO : false;
                }
                
                void enableTouchHandling() {
                    if (glkView_) {
                        glkview_enableTouchHandling(glkView_);
                    }
                }
                
                void disableTouchHandling() {
                    if (glkView_) {
                        glkview_disableTouchHandling(glkView_);
                    }
                }
                
                void debugTouchSetup() {
                    if (glkView_) {
                        glkview_debugTouchSetup(glkView_);
                    }
                }
                
                void updateDisplay()
                {
                    if(glkView_) {
                        glkview_setNeedsDisplay(glkView_);
                    }
                }
                
                // Touch event setters
                void setTouchBeganCallback(std::function<void(const TouchEvent&)> callback) {
                    touchBeganHandler_ = callback;
                    if (glkView_) {
                        glkview_setTouchBeganCallback(glkView_, [](uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->touchBeganHandler_) {
                                handler->touchBeganHandler_(TouchEvent(touchID, x, y, sizeX, sizeY));
                            }
                        }, this);
                    }
                }

                void setTouchMovedCallback(std::function<void(const TouchEvent&)> callback) {
                    touchMovedHandler_ = callback;
                    if (glkView_) {
                        glkview_setTouchMovedCallback(glkView_, [](uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->touchMovedHandler_) {
                                handler->touchMovedHandler_(TouchEvent(touchID, x, y, sizeX, sizeY));
                            }
                        }, this);
                    }
                }

                void setTouchEndedCallback(std::function<void(const TouchEvent&)> callback) {
                    touchEndedHandler_ = callback;
                    if (glkView_) {
                        glkview_setTouchEndedCallback(glkView_, [](uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->touchEndedHandler_) {
                                handler->touchEndedHandler_(TouchEvent(touchID, x, y, sizeX, sizeY));
                            }
                        }, this);
                    }
                }

                void setTouchCancelledCallback(std::function<void(const TouchEvent&)> callback) {
                    touchCancelledHandler_ = callback;
                    if (glkView_) {
                        glkview_setTouchCancelledCallback(glkView_, [](uint32_t touchID, double x, double y, double sizeX, double sizeY, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->touchCancelledHandler_) {
                                handler->touchCancelledHandler_(TouchEvent(touchID, x, y, sizeX, sizeY));
                            }
                        }, this);
                    }
                }

                // Keyboard event setters
                void setKeyDownCallback(std::function<void(int, const std::string&)> callback) {
                    keyDownHandler = callback;
                    if (glkView_) {
                        glkview_setKeyDownCallback(glkView_, [](unsigned short keyCode, const char* characters, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->keyDownHandler) {
                                handler->keyDownHandler(keyCode, std::string(characters ? characters : ""));
                            }
                        }, this);
                    }
                }

                void setKeyUpCallback(std::function<void(int, const std::string&)> callback) {
                    keyUpHandler = callback;
                    if (glkView_) {
                        glkview_setKeyUpCallback(glkView_, [](unsigned short keyCode, const char* characters, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->keyUpHandler) {
                                handler->keyUpHandler(keyCode, std::string(characters ? characters : ""));
                            }
                        }, this);
                    }
                }
                
                
                // Draw callback setter
                void setDrawCallback(std::function<void(double, double, double, double)> callback) {
                    drawHandler = callback;
                    if (glkView_) {
                        glkview_setDrawCallback(glkView_, [](double x, double y, double width, double height, void* userData) {
                            GLKView* handler = static_cast<GLKView*>(userData);
                            if (handler->drawHandler) {
                                handler->drawHandler(x, y, width, height);
                            }
                        }, this);
                    }
                }
            

                
                // Non-copyable but movable
                GLKView(const GLKView&) = delete;
                GLKView& operator=(const GLKView&) = delete;
                GLKView(GLKView&& other) noexcept 
                    : glkView_(other.glkView_), 
                      parentViewController_(other.parentViewController_),
                      touchCallbacks_(std::move(other.touchCallbacks_)),
                      multiTouchCallbacks_(std::move(other.multiTouchCallbacks_)),
                      keyCallbacks_(std::move(other.keyCallbacks_)) {
                    other.glkView_ = nullptr;
                    other.parentViewController_ = nullptr;
                }
                
                GLKView& operator=(GLKView&& other) noexcept {
                    if (this != &other) {
                        glkView_ = other.glkView_;
                        parentViewController_ = other.parentViewController_;
                        touchCallbacks_ = std::move(other.touchCallbacks_);
                        multiTouchCallbacks_ = std::move(other.multiTouchCallbacks_);
                        keyCallbacks_ = std::move(other.keyCallbacks_);
                        other.glkView_ = nullptr;
                        other.parentViewController_ = nullptr;
                    }
                    return *this;
                }
            };

            // OpenGL ES Renderer class
            class OpenGLRenderer {
            private:
                struct ::OpenGLRenderer* renderer_;
                
            public:
                OpenGLRenderer() : renderer_(nullptr) {
                    renderer_ = opengl_init();
                    if (!renderer_) {
                        throw FrameworkException("Failed to create iOS OpenGL renderer");
                    }
                }
                
                ~OpenGLRenderer() {
                    if (renderer_) {
                        opengl_destroy(renderer_);
                    }
                }
                
                void attachToViewController(ViewController& viewController) {
                    if (renderer_) {
                        opengl_initialize(renderer_, viewController.getCHandle());
                    }
                }
                
                void setupContext() {
                    if (renderer_) {
                        opengl_setupContext(renderer_);
                    }
                }
                
                void* getEAGLContext() const {
                    return renderer_ ? opengl_getEAGLContext(renderer_) : nullptr;
                }
                
                void makeCurrentContext() {
                    if (renderer_) {
                        opengl_makeCurrentContext(renderer_);
                    }
                }
                
                void setVsync(bool enabled) {
                    if (renderer_) {
                        opengl_setVsync(renderer_, enabled ? YES : NO);
                    }
                }
                
                void enableVsync() {
                    setVsync(true);
                }
                
                void disableVsync() {
                    setVsync(false);
                }
                
                void swapBuffers() {
                    if (renderer_) {
                        opengl_swapBuffers(renderer_);
                    }
                }
                
                // Non-copyable but movable
                OpenGLRenderer(const OpenGLRenderer&) = delete;
                OpenGLRenderer& operator=(const OpenGLRenderer&) = delete;
                OpenGLRenderer(OpenGLRenderer&& other) noexcept : renderer_(other.renderer_) {
                    other.renderer_ = nullptr;
                }
                OpenGLRenderer& operator=(OpenGLRenderer&& other) noexcept {
                    if (this != &other) {
                        if (renderer_) {
                            opengl_destroy(renderer_);
                        }
                        renderer_ = other.renderer_;
                        other.renderer_ = nullptr;
                    }
                    return *this;
                }
            };
                
            // Image loader wrapper class
            // As of 6 Jan 2025 both MacOS and iOS use the same image loading C API
            // However this may change in the future so we keep separate wrappers for now
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
                
                OpenGLTexture createOpenGLTexture() const noexcept {
                    if (loader_ && loaded_) {
                        return OpenGLTexture(imageloader_createOpenGLTexture(loader_));
                    }
                    return OpenGLTexture{};
                }
                
                // Legacy method for backwards compatibility
                unsigned int createOpenGLTextureID() const noexcept {
                    if (loader_ && loaded_) {
                        return imageloader_createOpenGLTexture(loader_);
                    }
                    return 0;
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
        
        } // namespace ios
    } // namespace apis
} // namespace olc

//! END DECLARATION
