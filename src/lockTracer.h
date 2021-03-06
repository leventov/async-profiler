/*
 * Copyright 2017 Andrei Pangin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LOCKTRACER_H
#define _LOCKTRACER_H

#include <jvmti.h>
#include "engine.h"


typedef void (JNICALL *UnsafeParkFunc)(JNIEnv*, jobject, jboolean, jlong);

class LockTracer : public Engine {
  private:
    static jlong _start_time;
    static jclass _LockSupport;
    static jmethodID _getBlocker;
    static jmethodID _Unsafe_park;
    static UnsafeParkFunc _original_Unsafe_Park;
    static LockTracer* _the_lockTracer;

    bool _traceOnlyLockContention;

    static void recordContendedLock(jclass lock_class, jlong time);
    static void bindUnsafePark(UnsafeParkFunc entry);

    jclass getParkBlockerClass(jvmtiEnv* jvmti, JNIEnv* env);

  public:
    LockTracer(bool traceOnlyLockContention) {
        _traceOnlyLockContention = traceOnlyLockContention;
    }

    const char* name() {
        if (_traceOnlyLockContention) {
            return "lock";
        } else {
            return "park";
        }
    }

    Error start(const char* event, long interval);
    void stop();

    static void JNICALL MonitorContendedEnter(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object);
    static void JNICALL MonitorContendedEntered(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object);
    static void JNICALL UnsafeParkTrap_static(JNIEnv* env, jobject instance, jboolean isAbsolute, jlong time);

    void JNICALL UnsafeParkTrap(JNIEnv* env, jobject instance, jboolean isAbsolute, jlong time);
};

#endif // _LOCKTRACER_H
