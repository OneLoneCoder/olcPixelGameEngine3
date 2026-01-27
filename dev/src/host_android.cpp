#include "host_android.h"

//! START IMPLEMENTATION
namespace olc::host
{
    static void Android_onAppCmd(struct android_app* app, int32_t cmd)
    {
        auto host = reinterpret_cast<olc::host::Host_Android*>(app->userData);
        host->OnAppCmd(app, cmd);
    }

    static bool Android_motionEventFilter(const GameActivityMotionEvent* event)
    {
        return (event->source & AINPUT_SOURCE_TOUCHSCREEN) || (event->source & AINPUT_SOURCE_MOUSE);
    }

    bool Host_Android::StartSystemEventLoop(bool bBlockIfPossible)
    {
        int events;
        struct android_poll_source* source = nullptr;

        while (ALooper_pollOnce(
            !initialized ? -1 : 0,
            nullptr,
            &events,
            (void**)&source
        ) >= 0) {
            if (source) source->process(olc_App, source);
        }

        if (!initialized) return false;

        android_input_buffer* inputBuffer = android_app_swap_input_buffers(olc_App);
        if (inputBuffer) {
            // Process motion events (touch, mouse, joystick)
            for (int i = 0; i < inputBuffer->motionEventsCount; ++i) {
                GameActivityMotionEvent* motionEvent = &inputBuffer->motionEvents[i];
                switch (motionEvent->action & AMOTION_EVENT_ACTION_MASK)
                {
                    case AMOTION_EVENT_ACTION_DOWN:
                    case AMOTION_EVENT_ACTION_POINTER_DOWN:
                        // Only the first button for now.
                        pgeWindow->olc_OnMouseButton(motionEvent->actionButton, true);
                        break;
                    case AMOTION_EVENT_ACTION_UP:
                    case AMOTION_EVENT_ACTION_POINTER_UP:
                        // Only the first button for now.
                        pgeWindow->olc_OnMouseButton(motionEvent->actionButton, false);
                        break;
                    case AMOTION_EVENT_ACTION_MOVE:
                    case AMOTION_EVENT_ACTION_HOVER_MOVE:
                        if (motionEvent->pointerCount > 0)
                        {
                            pgeWindow->olc_OnMouseMove({
                               static_cast<int32_t>(GameActivityPointerAxes_getX(
                                   &motionEvent->pointers[0])),
                               static_cast<int32_t>(GameActivityPointerAxes_getY(
                                   &motionEvent->pointers[0])),
                           });
                        }
                        break;
                }
            }

            // TODO: Process other input events (keyboard, controller)

            android_app_clear_motion_events(inputBuffer);
            android_app_clear_key_events(inputBuffer);
        }

        if (olc_App->destroyRequested != 0) return false;

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
                break;
            case APP_CMD_INIT_WINDOW:
                host->initialized = app->window != nullptr;
                __android_log_print(ANDROID_LOG_DEBUG, "PGE ANDROID", "APP_CMD_INIT_WINDOW received with Window");
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
        return { reinterpret_cast<void*>(olc_App->window) };
    }

    bool Host_Android::ConnectHostResourceToRenderer()
    {
        return true;
    }

    bool Host_Android::SyncWithDesktopComposite()
    {
        return true;
    }

    void Host_Android::SetAndridApp(struct android_app *app)
    {
        olc_App = app;
        olc_App->userData = this;
        olc_App->onAppCmd = Android_onAppCmd;

        android_app_set_motion_event_filter(olc_App, Android_motionEventFilter);
    }
}
//! END IMPLEMENTATION
