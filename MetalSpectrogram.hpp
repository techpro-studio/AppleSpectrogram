//
//  gpu_spectrogram.h
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#ifndef gpu_spectrogram_h
#define gpu_spectrogram_h

#include "BaseSpectrogram.hpp"
#include "_MetalSpectrogram.hpp"
#include "NSObjectCPPProxy.hpp"
#include <complex>


template <typename Input, unsigned int size, unsigned int nfft, unsigned int noverlap>
class MetalSpectrogram: public BaseSpectrogram<Input, size, nfft, noverlap>, protected NSObjectCPPProxy{
    typedef BaseSpectrogram<Input, size, nfft, noverlap> Base;
    using typename Base::Output;
public:
    MetalSpectrogram(const void* gpu): BaseSpectrogram<Input, size, nfft, noverlap>(), NSObjectCPPProxy((__bridge void *)[[_MetalSpectrogram alloc] initWithDevice:(__bridge id<MTLDevice>)gpu inputType: Base::inputType andSCP: Base::scp]){}

    Output calculate(const Tensor<Input, size>& input) override{
        Output output;
        auto spectrogram = (__bridge _MetalSpectrogram*)ptr;
        [spectrogram compute: input.value output:output.value];
        return output;
    }
};

#endif /* gpu_spectrogram_h */
