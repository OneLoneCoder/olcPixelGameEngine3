#pragma once

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <atomic>
#include <array>
#include <string>
//! END STDHEADER

//! START DECLARATION
#include <android_native_app_glue.h>
#include <android/log.h>
#include <jni.h>

// We allow users to create a normal main function for android apps
extern int main(int argc, char** argv);

namespace olc::host
{
    using AndroidApp = struct android_app;
    class Host_Android : public olc::host::Host
    {
    public:
        Host_Android();
        bool StartSystemEventLoop(bool bBlockIfPossible) override;
        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;

        bool ConnectHostResourceToRenderer() override;

        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;

        olc::KeyboardLayout GetKeyboardLayout() const override;

        void OnAppCmd(AndroidApp* app, int32_t cmd);
        int32_t OnInputEvent(AndroidApp* app, AInputEvent* event);

        bool IsInitialized() const { return initialized.load(); }

        void ShowKeyboard(bool bShow);

        // TODO: Temporary file loading support
        std::vector<uint8_t> OpenFile(const std::string& sFileName);
        std::string OpenTextFile(const std::string& sFileName);

        static AndroidApp* androidApp;
    protected:
        olc::Window* pgeWindow = nullptr;
        std::atomic<bool> initialized{false};
        bool shiftOn = false;
    };

    class JNI
    {
    public:
        static void Init(AndroidApp* app);
        static void Detach();

        static JNIEnv* GetEnv();

    private:
        static JavaVM* jvm;
    };

    class JNIObject
    {
    public:
        JNIObject() = default;
        JNIObject(jobject obj);

        ~JNIObject();

        JNIObject(const JNIObject&) = delete;
        JNIObject& operator=(const JNIObject&) = delete;

        JNIObject(JNIObject&& other) noexcept;

        JNIObject& operator=(JNIObject&& other) noexcept;

        jobject Get() const { return obj; }
        jobject operator *() const { return obj; }
        operator bool() const { return obj != nullptr; }

        template <typename R, typename... Args>
        R Call(
            const std::string& methodName,
            const std::string& methodSig,
            Args&&... args
        )
        {
            JNIEnv* env = JNI::GetEnv();
            jclass objClass = env->GetObjectClass(obj);
            jmethodID methodID = env->GetMethodID(
                objClass,
                methodName.c_str(),
                methodSig.c_str()
            );
            env->DeleteLocalRef(objClass);

            R result{};
            
            if constexpr (std::is_same_v<R, void>)
            {
                env->CallVoidMethod(obj, methodID, std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jint>)
            {
                result = env->CallIntMethod(obj, methodID,  std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jboolean>)
            {
                result = env->CallBooleanMethod(obj, methodID,  std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jbyte>)
            {
                result = env->CallByteMethod(obj, methodID,  std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jchar>)
            {
                result = env->CallCharMethod(obj, methodID,  std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jshort>)
            {
                result = env->CallShortMethod(obj, methodID,  std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jlong>)
            {
                result = env->CallLongMethod(obj, methodID,  std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jfloat>)
            {
                result = env->CallFloatMethod(obj, methodID,  std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jdouble>)
            {
                result = env->CallDoubleMethod(obj, methodID,  std::forward<Args>(args)...);
            }
            else if constexpr (std::is_same_v<R, jobject> || std::is_same_v<R, JNIObject>)
            {
                jobject callResult = env->CallObjectMethod(obj, methodID, std::forward<Args>(args)...);
                if (env->ExceptionCheck())
                {
                    env->ExceptionDescribe();
                    env->ExceptionClear();
                    return {};
                }
                if constexpr (std::is_same_v<R, jobject>)
                {
                    result = callResult;
                }
                else // JNIObject
                {
                    result = JNIObject(callResult);
                }
            }
            else
            {
                static_assert(sizeof(R) == 0, "Unsupported return type in JNIObject::Call");
            }
            
            if (env->ExceptionCheck())
            {
                env->ExceptionDescribe();
                env->ExceptionClear();
                return {};
            }
            return result;
        }

    private:
        jobject obj = nullptr;
    };

    class JNIString
    {
    public:
        JNIString(const std::string& str)
        {
            JNIEnv* env = JNI::GetEnv();
            jstring local = env->NewStringUTF(str.c_str());
            jstr = static_cast<jstring>(env->NewGlobalRef(local));
            env->DeleteLocalRef(local);
        }

        JNIString(jstring jstr)
        {
            JNIEnv* env = JNI::GetEnv();
            const char* chars = env->GetStringUTFChars(jstr, nullptr);
            std::string str(chars);
            env->ReleaseStringUTFChars(jstr, chars);
            jstring local = env->NewStringUTF(str.c_str());
            this->jstr = static_cast<jstring>(env->NewGlobalRef(local));
            env->DeleteLocalRef(local);
        }

        ~JNIString()
        {
            if (jstr)
            {
                JNI::GetEnv()->DeleteGlobalRef(jstr);
            }
            jstr = nullptr;
        }

        operator std::string() const
        {
            JNIEnv* env = JNI::GetEnv();
            const char* chars = env->GetStringUTFChars(jstr, nullptr);
            std::string result(chars);
            env->ReleaseStringUTFChars(jstr, chars);
            return result;
        }

        jstring operator*() const
        {
            return jstr;
        }

    private:
        jstring jstr = nullptr;
    };

}
//! END DECLARATION
