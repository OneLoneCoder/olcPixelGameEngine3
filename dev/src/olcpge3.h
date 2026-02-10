#pragma once

#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "vector4d.h"
#include "matrix3d.h"
#include "matrix4d.h"
#include "transform2d.h"
#include "window.h"
#include "core.h"

#include "image.h"
#include "imload_iface.h"
#if OLC_HOST == OLC_HOST_WINDOWS
#include "imload_wingdi.h"
#endif

#if OLC_HOST == OLC_HOST_MACOS
#include "imload_macos.h"
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_X11 || OLC_HOST == OLC_HOST_LINUX_WAYLAND
#include "imload_lib_png.h"
#endif

#include "gputask.h"
#include "gpu_iface.h"
#include "gpu_opengl33.h"





