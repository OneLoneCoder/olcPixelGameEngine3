#include "host_android.h"

//! START IMPLEMENTATION
#include <android/input.h>
#include <android/asset_manager.h>

namespace olc::host
{
    constexpr size_t ANDROID_KEY_MAX = 163;


    struct KeyEntry
    {
        olc::Key key;
        bool shiftOn;
    };

    static const std::array<KeyEntry, ANDROID_KEY_MAX> AndroidKeyMap = [] {
        std::array<KeyEntry, ANDROID_KEY_MAX> map{};
        map.fill({Key::NONE, false});

        // Letters
        map[AKEYCODE_A] = {Key::A, false};
        map[AKEYCODE_B] = {Key::B, false};
        map[AKEYCODE_C] = {Key::C, false};
        map[AKEYCODE_D] = {Key::D, false};
        map[AKEYCODE_E] = {Key::E, false};
        map[AKEYCODE_F] = {Key::F, false};
        map[AKEYCODE_G] = {Key::G, false};
        map[AKEYCODE_H] = {Key::H, false};
        map[AKEYCODE_I] = {Key::I, false};
        map[AKEYCODE_J] = {Key::J, false};
        map[AKEYCODE_K] = {Key::K, false};
        map[AKEYCODE_L] = {Key::L, false};
        map[AKEYCODE_M] = {Key::M, false};
        map[AKEYCODE_N] = {Key::N, false};
        map[AKEYCODE_O] = {Key::O, false};
        map[AKEYCODE_P] = {Key::P, false};
        map[AKEYCODE_Q] = {Key::Q, false};
        map[AKEYCODE_R] = {Key::R, false};
        map[AKEYCODE_S] = {Key::S, false};
        map[AKEYCODE_T] = {Key::T, false};
        map[AKEYCODE_U] = {Key::U, false};
        map[AKEYCODE_V] = {Key::V, false};
        map[AKEYCODE_W] = {Key::W, false};
        map[AKEYCODE_X] = {Key::X, false};
        map[AKEYCODE_Y] = {Key::Y, false};
        map[AKEYCODE_Z] = {Key::Z, false};

        // Numbers
        map[AKEYCODE_0] = {Key::K0, false};
        map[AKEYCODE_1] = {Key::K1, false};
        map[AKEYCODE_2] = {Key::K2, false};
        map[AKEYCODE_3] = {Key::K3, false};
        map[AKEYCODE_4] = {Key::K4, false};
        map[AKEYCODE_5] = {Key::K5, false};
        map[AKEYCODE_6] = {Key::K6, false};
        map[AKEYCODE_7] = {Key::K7, false};
        map[AKEYCODE_8] = {Key::K8, false};
        map[AKEYCODE_9] = {Key::K9, false};

        // Numpad
        map[AKEYCODE_NUMPAD_0] = {Key::NP0, false};
        map[AKEYCODE_NUMPAD_1] = {Key::NP1, false};
        map[AKEYCODE_NUMPAD_2] = {Key::NP2, false};
        map[AKEYCODE_NUMPAD_3] = {Key::NP3, false};
        map[AKEYCODE_NUMPAD_4] = {Key::NP4, false};
        map[AKEYCODE_NUMPAD_5] = {Key::NP5, false};
        map[AKEYCODE_NUMPAD_6] = {Key::NP6, false};
        map[AKEYCODE_NUMPAD_7] = {Key::NP7, false};
        map[AKEYCODE_NUMPAD_8] = {Key::NP8, false};
        map[AKEYCODE_NUMPAD_9] = {Key::NP9, false};
        map[AKEYCODE_NUMPAD_ADD] = {Key::NP_ADD, false};
        map[AKEYCODE_NUMPAD_SUBTRACT] = {Key::NP_SUB, false};
        map[AKEYCODE_NUMPAD_MULTIPLY] = {Key::NP_MUL, false};
        map[AKEYCODE_NUMPAD_DIVIDE] = {Key::NP_DIV, false};
        map[AKEYCODE_NUMPAD_DOT] = {Key::NP_DECIMAL, false};

        // Function keys
        map[AKEYCODE_F1] = {Key::F1, false};
        map[AKEYCODE_F2] = {Key::F2, false};
        map[AKEYCODE_F3] = {Key::F3, false};
        map[AKEYCODE_F4] = {Key::F4, false};
        map[AKEYCODE_F5] = {Key::F5, false};
        map[AKEYCODE_F6] = {Key::F6, false};
        map[AKEYCODE_F7] = {Key::F7, false};
        map[AKEYCODE_F8] = {Key::F8, false};
        map[AKEYCODE_F9] = {Key::F9, false};
        map[AKEYCODE_F10] = {Key::F10, false};
        map[AKEYCODE_F11] = {Key::F11, false};
        map[AKEYCODE_F12] = {Key::F12, false};

        // Arrows
        map[AKEYCODE_DPAD_UP] = {Key::UP, false};
        map[AKEYCODE_DPAD_DOWN] = {Key::DOWN, false};
        map[AKEYCODE_DPAD_LEFT] = {Key::LEFT, false};
        map[AKEYCODE_DPAD_RIGHT] = {Key::RIGHT, false};

        // Common keys
        map[AKEYCODE_SPACE] = {Key::SPACE, false};
        map[AKEYCODE_TAB] = {Key::TAB, false};
        map[AKEYCODE_ENTER] = {Key::ENTER, false};
        map[AKEYCODE_ESCAPE] = {Key::ESCAPE, false};
        map[AKEYCODE_DEL] = {Key::BACK, false};
        map[AKEYCODE_FORWARD_DEL] = {Key::DEL, false};

        // Modifiers
        map[AKEYCODE_SHIFT_LEFT] = {Key::SHIFT, false};
        map[AKEYCODE_SHIFT_RIGHT] = {Key::SHIFT, false};
        map[AKEYCODE_CTRL_LEFT] = {Key::CTRL, false};
        map[AKEYCODE_CTRL_RIGHT] = {Key::CTRL, false};
        map[AKEYCODE_CAPS_LOCK] = {Key::CAPS_LOCK, false};

        // OEM-style punctuation
        map[AKEYCODE_SEMICOLON] = {Key::OEM_1, false};
        map[AKEYCODE_SLASH] = {Key::OEM_2, false};
        map[AKEYCODE_GRAVE] = {Key::OEM_7, false};
        map[AKEYCODE_LEFT_BRACKET] = {Key::OEM_4, false};
        map[AKEYCODE_BACKSLASH] = {Key::OEM_5, false};
        map[AKEYCODE_RIGHT_BRACKET] = {Key::OEM_6, false};
        map[AKEYCODE_APOSTROPHE] = {Key::OEM_3, false};
        map[AKEYCODE_EQUALS] = {Key::EQUALS, false};
        map[AKEYCODE_COMMA] = {Key::COMMA, false};
        map[AKEYCODE_MINUS] = {Key::MINUS, false};
        map[AKEYCODE_PERIOD] = {Key::PERIOD, false};
        map[AKEYCODE_PLUS] = {Key::EQUALS, true};
        map[AKEYCODE_AT] = {Key::K2, true};
        map[AKEYCODE_POUND] = {Key::K3, true};
        map[AKEYCODE_STAR] = {Key::NP_MUL, false};

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
            int32_t meta = AKeyEvent_getMetaState(event);

            auto entry = ((keyCode > 0) && keyCode < ANDROID_KEY_MAX) ? AndroidKeyMap[keyCode] : KeyEntry{Key::NONE, false};

            shiftOn =
                (meta & AMETA_SHIFT_ON) != 0 ||
                (meta & AMETA_SHIFT_LEFT_ON) != 0 ||
                (meta & AMETA_SHIFT_RIGHT_ON) != 0 ||
                (meta & AMETA_CAPS_LOCK_ON) != 0 ||
                entry.shiftOn;

            if (entry.key != Key::NONE)
            {
                pgeWindow->olc_OnKeyPress(Key::SHIFT, shiftOn);
                if (action == AKEY_EVENT_ACTION_DOWN)
                {
                    pgeWindow->olc_OnKeyPress(entry.key, true);
                }
                else if (action == AKEY_EVENT_ACTION_UP)
                {
                    pgeWindow->olc_OnKeyPress(entry.key, false);
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

    std::vector<uint8_t> Host_Android::OpenFile(const std::string &sFileName)
    {
        AAsset* asset = AAssetManager_open(
            androidApp->activity->assetManager,
            sFileName.c_str(),
            AASSET_MODE_STREAMING
        );
        if (!asset) {
            return {};
        }

        off_t size = AAsset_getLength(asset);
        std::vector<uint8_t> buffer(size);
        AAsset_read(asset, buffer.data(), size);
        AAsset_close(asset);

        return buffer;
    }

    std::string Host_Android::OpenTextFile(const std::string &sFileName)
    {
        AAsset* asset = AAssetManager_open(
            androidApp->activity->assetManager,
            sFileName.c_str(),
            AASSET_MODE_STREAMING
        );
        if (!asset) {
            return {};
        }

        off_t size = AAsset_getLength(asset);
        std::string content(size, '\0');
        AAsset_read(asset, content.data(), size);
        AAsset_close(asset);

        return content;
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
