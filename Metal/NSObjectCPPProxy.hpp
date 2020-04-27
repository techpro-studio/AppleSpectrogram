//
//  RealGPUSpectrogramBridge.hpp
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#ifndef object_hpp
#define object_hpp

#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>

class NSObjectCPPProxy
{
public:
    const void* GetPtr() const { return ptr; }
    inline operator bool() const { return ptr != nullptr; }

protected:
    NSObjectCPPProxy(): ptr(nullptr){}

    NSObjectCPPProxy(const void* ptr): ptr(ptr) {
        if (ptr)
            CFRetain(ptr);
    }

    NSObjectCPPProxy(const NSObjectCPPProxy& rhs): ptr(rhs.ptr) {
        if (ptr)
            CFRetain(ptr);
    }
    
    NSObjectCPPProxy(NSObjectCPPProxy&& rhs):  ptr(rhs.ptr){
        rhs.ptr = nullptr;
    }

    virtual ~NSObjectCPPProxy() {
        if (ptr)
            CFRelease(ptr);
    }

    NSObjectCPPProxy& operator=(const NSObjectCPPProxy& rhs) {
        if (rhs.ptr == ptr)
            return *this;
        if (rhs.ptr)
            CFRetain(rhs.ptr);
        if (ptr)
            CFRelease(ptr);
        ptr = rhs.ptr;
        return *this;
    }

    NSObjectCPPProxy& operator=(NSObjectCPPProxy&& rhs) {
        if (rhs.ptr == ptr)
            return *this;
        if (ptr)
            CFRelease(ptr);
        ptr = rhs.ptr;
        rhs.ptr = nullptr;
        return *this;
    }


    const void* ptr = nullptr;
};

#endif
