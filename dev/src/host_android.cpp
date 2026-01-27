#include "host_android.h"

//! START IMPLEMENTATION
namespace olc::host
{
    AndroidApp* Host_Android::androidApp = nullptr;

    static void Android_onAppCmd(struct android_app* app, int32_t cmd)
    {
        auto host = reinterpret_cast<olc::host::Host_Android*>(app->userData);
        host->OnAppCmd(app, cmd);
    }

    static int32_t Android_onInputEvent(struct android_app* app, AInputEvent* event)
    {
        auto host = reinterpret_cast<olc::host::Host_Android*>(app->userData);
        return host->OnInputEvent(app, event);
    }

    bool Host_Android::StartSystemEventLoop(bool bBlockIfPossible)
    {
        int events;
        struct android_poll_source* source = nullptr;

        if (ALooper_pollOnce(
            bBlockIfPossible || !initialized ? -1 : 0,
            nullptr,
            &events,
            (void**)&source
        ) >= 0) {
            if (source) source->process(androidApp, source);
        }

        if (!androidApp || !initialized) return true;

        if (androidApp->destroyRequested != 0) return false;

        return true;
    }

    void Host_Android::OnAppCmd(struct android_app *app, int32_t cmd)
    {
        auto host = reinterpret_cast<olc::host::Host_Android*>(app->userData);
        switch (cmd) {
            case APP_CMD_WINDOW_RESIZED:
                host->pgeWindow->olc_OnWindowSize({
                  ANativeWindow_getWidth(app->window),
                  ANativeWindow_getHeight(app->window)
                });
                __android_log_print(ANDROID_LOG_DEBUG, "PGE ANDROID",
                                    "APP_CMD_WINDOW_RESIZED received: %dx%d",
                                    ANativeWindow_getWidth(app->window),
                                    ANativeWindow_getHeight(app->window));
                break;
            case APP_CMD_INIT_WINDOW:
                if (app->window) {
                    host->initialized = true;
                    __android_log_print(ANDROID_LOG_DEBUG, "PGE ANDROID",
                                        "APP_CMD_INIT_WINDOW received with Window");
                }
                break;
            case APP_CMD_TERM_WINDOW: {
                host->pgeWindow->olc_OnWindowClose();
            } break;
            case APP_CMD_GAINED_FOCUS: {
                host->pgeWindow->olc_OnMouseFocus(true);
            } break;
            case APP_CMD_LOST_FOCUS: {
                host->pgeWindow->olc_OnMouseFocus(false);
            } break;
            default: break;
        }
    }

    int32_t Host_Android::OnInputEvent(AndroidApp *app, AInputEvent *event)
    {
        auto host = reinterpret_cast<olc::host::Host_Android*>(app->userData);
        auto type = AInputEvent_getType(event);
        auto source = AInputEvent_getSource(event);
        
        if (type == AINPUT_EVENT_TYPE_MOTION) {
            pgeWindow->olc_OnMouseMove({
                static_cast<int32_t>(AMotionEvent_getX(event, 0)),
                static_cast<int32_t>(AMotionEvent_getY(event, 0)),
            });

            auto action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;

            switch (action) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    pgeWindow->olc_OnMouseButton(0, true); // Left button
                    break;
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                    pgeWindow->olc_OnMouseButton(0, false); // Left button
                    break;
                case AMOTION_EVENT_AXIS_WHEEL:
                    // Handle mouse wheel
                    {
                        float vScroll = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_VSCROLL, 0);
                        if (vScroll != 0.0f) {
                            pgeWindow->olc_OnMouseWheel(static_cast<int32_t>(vScroll * 120.0f));
                        }
                    }
                    break;
                default:
                    break;
            }

            return 1;
        }

        return 0;
    }

    bool Host_Android::AddWindowFrame(olc::Window *pWindow, const vi2d &vWindowPos,
                                      const vi2d &vWindowSize, const bool bFullScreen)
    {
        pgeWindow = pWindow;
        return true;
    }

    bool Host_Android::CloseWindowFrame(olc::Window *pWindow)
    {
        return true;
    }

    bool Host_Android::UpdateWindowFrameTitle(olc::Window *pWindow)
    {
        return true;
    }

    std::vector<void*> Host_Android::GetHostWindowDescriptor(olc::Window *pWindow)
    {
        return { reinterpret_cast<void*>(androidApp->window) };
    }

    bool Host_Android::ConnectHostResourceToRenderer()
    {
        return true;
    }

    bool Host_Android::SyncWithDesktopComposite()
    {
        return true;
    }

    Host_Android::Host_Android()
    {
        androidApp->onAppCmd = Android_onAppCmd;
        androidApp->onInputEvent = Android_onInputEvent;
        androidApp->userData = this;
    }
}

void android_main(struct android_app* app)
{
    char arg0[] = "olcPixelGameEngine 3.0";
    char* argv[] = { arg0, nullptr };
    
    olc::host::Host_Android::androidApp = app;

    (void)main(1, argv);

    ANativeActivity_finish(app->activity);

    while (!app->destroyRequested) {
        int events;
        struct android_poll_source* source;

        while (ALooper_pollOnce(0, nullptr, &events, (void**)&source) > ALOOPER_POLL_TIMEOUT) {
            if (source) {
                source->process(app, source);
            }
        }
    }
}
//! END IMPLEMENTATION
