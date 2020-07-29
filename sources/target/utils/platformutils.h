/* Copyright (c) 2020 Qmlio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <QDebug>
#include <QGuiApplication>
#include <QWindow>

#if defined(Q_OS_ANDROID)
#include "google_play_services/availability.h"
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

#if defined(Q_OS_IOS)
#include <qpa/qplatformnativeinterface.h>
#endif

#if defined(Q_OS_ANDROID)
namespace qmlio {
namespace googleFirebase {

class GooglePlayServices {
public:
  enum Availability {

    AvailabilityAvailable = ::google_play_services::kAvailabilityAvailable,

    AvailabilityUnavailableDisabled =
        ::google_play_services::kAvailabilityUnavailableDisabled,

    AvailabilityUnavailableInvalid =
        ::google_play_services::kAvailabilityUnavailableInvalid,

    AvailabilityUnavailableMissing =
        ::google_play_services::kAvailabilityUnavailableMissing,

    AvailabilityUnavailablePermissions =
        ::google_play_services::kAvailabilityUnavailablePermissions,

    AvailabilityUnavailableUpdateRequired =
        ::google_play_services::kAvailabilityUnavailableUpdateRequired,

    AvailabilityUnavailableUpdating =
        ::google_play_services::kAvailabilityUnavailableUpdating,

    AvailabilityUnavailableOther =
        ::google_play_services::kAvailabilityUnavailableOther,
  };

  static Availability getAvailability();
  static bool available();
};
#endif

/*
 * PlatformUtils
 */
class PlatformUtils {
public:
#if defined(Q_OS_IOS)
  PlatformUtils();
  ~PlatformUtils();
  static void *getNativeWindow();
#elif defined(Q_OS_ANDROID)
  static jobject getNativeWindow();
#else
  static void getNativeWindow();
#endif

private:
#if defined(Q_OS_ANDROID)
  static jobject nativeWindow;
#endif
};
}
}
#endif // PLATFORM_UTILS_H
