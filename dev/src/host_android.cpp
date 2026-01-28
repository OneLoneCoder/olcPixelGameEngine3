#include "host_android.h"

//! START IMPLEMENTATION
#include <android/input.h>

namespace olc::host
{
    constexpr size_t ANDROID_KEY_MAX = 163;

    static const std::array<Key, ANDROID_KEY_MAX> AndroidKeyMap = [] {
        std::array<Key, ANDROID_KEY_MAX> map{};
        map.fill(Key::NONE);

        // Letters
        map[AKEYCODE_A] = Key::A;
        map[AKEYCODE_B] = Key::B;
        map[AKEYCODE_C] = Key::C;
        map[AKEYCODE_D] = Key::D;
        map[AKEYCODE_E] = Key::E;
        map[AKEYCODE_F] = Key::F;
        map[AKEYCODE_G] = Key::G;
        map[AKEYCODE_H] = Key::H;
        map[AKEYCODE_I] = Key::I;
        map[AKEYCODE_J] = Key::J;
        map[AKEYCODE_K] = Key::K;
        map[AKEYCODE_L] = Key::L;
        map[AKEYCODE_M] = Key::M;
        map[AKEYCODE_N] = Key::N;
        map[AKEYCODE_O] = Key::O;
        map[AKEYCODE_P] = Key::P;
        map[AKEYCODE_Q] = Key::Q;
        map[AKEYCODE_R] = Key::R;
        map[AKEYCODE_S] = Key::S;
        map[AKEYCODE_T] = Key::T;
        map[AKEYCODE_U] = Key::U;
        map[AKEYCODE_V] = Key::V;
        map[AKEYCODE_W] = Key::W;
        map[AKEYCODE_X] = Key::X;
        map[AKEYCODE_Y] = Key::Y;
        map[AKEYCODE_Z] = Key::Z;

        // Numbers
        map[AKEYCODE_0] = Key::K0;
        map[AKEYCODE_1] = Key::K1;
        map[AKEYCODE_2] = Key::K2;
        map[AKEYCODE_3] = Key::K3;
        map[AKEYCODE_4] = Key::K4;
        map[AKEYCODE_5] = Key::K5;
        map[AKEYCODE_6] = Key::K6;
        map[AKEYCODE_7] = Key::K7;
        map[AKEYCODE_8] = Key::K8;
        map[AKEYCODE_9] = Key::K9;

        // Numpad
        map[AKEYCODE_NUMPAD_0] = Key::NP0;
        map[AKEYCODE_NUMPAD_1] = Key::NP1;
        map[AKEYCODE_NUMPAD_2] = Key::NP2;
        map[AKEYCODE_NUMPAD_3] = Key::NP3;
        map[AKEYCODE_NUMPAD_4] = Key::NP4;
        map[AKEYCODE_NUMPAD_5] = Key::NP5;
        map[AKEYCODE_NUMPAD_6] = Key::NP6;
        map[AKEYCODE_NUMPAD_7] = Key::NP7;
        map[AKEYCODE_NUMPAD_8] = Key::NP8;
        map[AKEYCODE_NUMPAD_9] = Key::NP9;
        map[AKEYCODE_NUMPAD_ADD] = Key::NP_ADD;
        map[AKEYCODE_NUMPAD_SUBTRACT] = Key::NP_SUB;
        map[AKEYCODE_NUMPAD_MULTIPLY] = Key::NP_MUL;
        map[AKEYCODE_NUMPAD_DIVIDE] = Key::NP_DIV;
        map[AKEYCODE_NUMPAD_DOT] = Key::NP_DECIMAL;

        // Function keys
        map[AKEYCODE_F1] = Key::F1;
        map[AKEYCODE_F2] = Key::F2;
        map[AKEYCODE_F3] = Key::F3;
        map[AKEYCODE_F4] = Key::F4;
        map[AKEYCODE_F5] = Key::F5;
        map[AKEYCODE_F6] = Key::F6;
        map[AKEYCODE_F7] = Key::F7;
        map[AKEYCODE_F8] = Key::F8;
        map[AKEYCODE_F9] = Key::F9;
        map[AKEYCODE_F10] = Key::F10;
        map[AKEYCODE_F11] = Key::F11;
        map[AKEYCODE_F12] = Key::F12;

        // Arrows
        map[AKEYCODE_DPAD_UP] = Key::UP;
        map[AKEYCODE_DPAD_DOWN] = Key::DOWN;
        map[AKEYCODE_DPAD_LEFT] = Key::LEFT;
        map[AKEYCODE_DPAD_RIGHT] = Key::RIGHT;

        // Common keys
        map[AKEYCODE_SPACE] = Key::SPACE;
        map[AKEYCODE_TAB] = Key::TAB;
        map[AKEYCODE_ENTER] = Key::ENTER;
        map[AKEYCODE_ESCAPE] = Key::ESCAPE;
        map[AKEYCODE_DEL] = Key::BACK;
        map[AKEYCODE_FORWARD_DEL] = Key::DEL;

        // Modifiers
        map[AKEYCODE_SHIFT_LEFT] = Key::SHIFT;
        map[AKEYCODE_SHIFT_RIGHT] = Key::SHIFT;
        map[AKEYCODE_CTRL_LEFT] = Key::CTRL;
        map[AKEYCODE_CTRL_RIGHT] = Key::CTRL;
        map[AKEYCODE_CAPS_LOCK] = Key::CAPS_LOCK;

        // OEM-style punctuation
        map[AKEYCODE_SEMICOLON] = Key::OEM_1;
        map[AKEYCODE_SLASH] = Key::OEM_2;
        map[AKEYCODE_GRAVE] = Key::OEM_3;
        map[AKEYCODE_LEFT_BRACKET] = Key::OEM_4;
        map[AKEYCODE_BACKSLASH] = Key::OEM_5;
        map[AKEYCODE_RIGHT_BRACKET] = Key::OEM_6;
        map[AKEYCODE_APOSTROPHE] = Key::OEM_7;

        return map;
    }();

    AndroidApp* Host_Android::androidApp = nullptr;
    JavaVM* JNI::jvm = nullptr;

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
        else if (type == AINPUT_EVENT_TYPE_KEY)
        {
            int32_t keyCode = AKeyEvent_getKeyCode(event);
            int32_t action = AKeyEvent_getAction(event);

            olc::Key key = ((keyCode > 0) && keyCode < ANDROID_KEY_MAX) ? AndroidKeyMap[keyCode] : Key::NONE;

            if (key != Key::NONE)
            {
                if (action == AKEY_EVENT_ACTION_DOWN)
                {
                    pgeWindow->olc_OnKeyPress(key, true);
                }
                else if (action == AKEY_EVENT_ACTION_UP)
                {
                    pgeWindow->olc_OnKeyPress(key, false);
                }
            }

            if (keyCode == AKEYCODE_POWER)
            {
                // Behaviour: CMD_PAUSE -> CMD_SAVE_STATE -> CMD_STOP -> CMD_CONFIG_CHANGED -> CMD_LOST_FOCUS
                // Resuming Behaviour: CMD_START -> CMD_RESUME -> CMD_CONFIG_CHANGED -> CMD_CONFIG_CHANGED -> CMD_GAINED_FOCUS
                return 0;
            }
            else if ((keyCode == AKEYCODE_BACK) || (keyCode == AKEYCODE_MENU))
            {
                return 1;
            }
            else if ((keyCode == AKEYCODE_VOLUME_UP) || (keyCode == AKEYCODE_VOLUME_DOWN))
            {
                return 0;
            }

            return 0;
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

    olc::KeyboardLayout Host_Android::GetKeyboardLayout() const
    {
        JNIObject activity(androidApp->activity->clazz);
        JNIEnv* env = JNI::GetEnv();

        auto param = JNIString("input_method");
        JNIObject imm = activity.Call<JNIObject>(
            "getSystemService",
            "(Ljava/lang/String;)Ljava/lang/Object;",
            *param
        );

        std::string result = "";

        if (*imm) {
            JNIObject subType = imm.Call<JNIObject>(
                "getCurrentInputMethodSubtype",
                "()Landroid/view/inputmethod/InputMethodSubtype;"
            );
            if (*subType) {
                JNIObject localeStr = subType.Call<JNIObject>(
                    "getLocale",
                    "()Ljava/lang/String;"
                );
                
                if (*localeStr) {
                    JNIString localeStrObj(static_cast<jstring>(*localeStr));
                    result = localeStrObj;
                }
            }
        }

        JNI::Detach();

        auto countryCodePos = result.substr(result.find('_') + 1);
        if (countryCodePos.find("US") != std::string::npos)
        {
            return olc::KeyboardLayout::QWERTY_US;
        }
        else if (countryCodePos.find("UK") != std::string::npos ||
                   countryCodePos.find("GB") != std::string::npos)
        {
            return olc::KeyboardLayout::QWERTY_UK;
        }
        else if (countryCodePos.find("DE") != std::string::npos)
        {
            return olc::KeyboardLayout::QWERTZ;
        }
        else if (countryCodePos.find("FR") != std::string::npos)
        {
            return olc::KeyboardLayout::AZERTY;
        }

#ifdef PGE_SPELL_CORRECTLY
        return olc::KeyboardLayout::QWERTY_UK;
#else
        return olc::KeyboardLayout::QWERTY_US;
#endif
    }

    void Host_Android::ShowKeyboard(bool bShow)
    {
        JNIObject activity(androidApp->activity->clazz);

        auto param = JNIString("input_method");
        auto imm = activity.Call<JNIObject>(
            "getSystemService",
            "(Ljava/lang/String;)Ljava/lang/Object;",
            *param
        );
        if (!imm) return;

        auto window = activity.Call<JNIObject>(
            "getWindow",
            "()Landroid/view/Window;"
        );
        if (!window) return;

        auto decor = window.Call<JNIObject>(
            "getDecorView",
            "()Landroid/view/View;"
        );
        if (!decor) return;

        if (bShow)
        {
            decor.Call<jboolean>(
                "requestFocus",
                "()Z"
            );
            imm.Call<jboolean>(
                "showSoftInput",
                "(Landroid/view/View;I)Z",
                *decor,
                0
            );
        }
        else
        {
            auto token = decor.Call<JNIObject>(
                "getWindowToken",
                "()Landroid/os/IBinder;"
            );
            if (!token) return;
            imm.Call<jboolean>(
                "hideSoftInputFromWindow",
                "(Landroid/os/IBinder;I)Z",
                *token,
                0
            );
        }

        JNI::Detach();
    }

    Host_Android::Host_Android()
    {
        androidApp->onAppCmd = Android_onAppCmd;
        androidApp->onInputEvent = Android_onInputEvent;
        androidApp->userData = this;
    }

    void JNI::Init(AndroidApp *app)
    {
        jvm = app->activity->vm;
    }

    void JNI::Detach()
    {
        jvm->DetachCurrentThread();
    }

    JNIEnv* JNI::GetEnv()
    {
        JNIEnv* env = nullptr;
        if (jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        {
            jvm->AttachCurrentThread(&env, nullptr);
        }
        return env;
    }

    JNIObject::JNIObject(jobject obj)
    {
        this->obj = JNI::GetEnv()->NewGlobalRef(obj);
    }

    JNIObject::~JNIObject()
    {
        if (obj)
        {
            JNI::GetEnv()->DeleteGlobalRef(obj);
        }
        obj = nullptr;
    }

    JNIObject::JNIObject(JNIObject&& other) noexcept
    {
        obj = other.obj;
        other.obj = nullptr;
    }

    JNIObject &JNIObject::operator=(JNIObject&& other) noexcept
    {
        if (this != &other) {
            if (obj) JNI::GetEnv()->DeleteGlobalRef(obj);
            obj = other.obj;
            other.obj = nullptr;
        }
        return *this;
    }
    
}

void android_main(struct android_app* app)
{
    char arg0[] = "olcPixelGameEngine 3.0";
    char* argv[] = { arg0, nullptr };
    
    olc::host::JNI::Init(app);
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
