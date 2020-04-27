//
//  vdsp_spectrogram.h
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#ifndef vdsp_spectrogram_h
#define vdsp_spectrogram_h

#include "BaseSpectrogram.hpp"
#include "vDSPSpectrogram.h"

template <typename Input, unsigned int size, unsigned int nfft, unsigned int noverlap, bool concurrent>
class BasevDSPSpectrogram: public BaseSpectrogram<Input, size, nfft, noverlap>{
    static_assert(powerof2(nfft) || powerof2(nfft / 3) || powerof2(nfft / 5) || powerof2(nfft / 15), "nfft should be power of 2 or 2 ^ n * 3 * 5 where n >= 3");
protected:
    vDSP_Spectrogram_Setup setup;
public:
    BasevDSPSpectrogram(): setup(vDSP_Spectrogram_Setup_Init(BaseSpectrogram<Input, size, nfft, noverlap>::scp)) {}
    virtual ~BasevDSPSpectrogram() {
        vDSP_Spectrogram_Setup_Destroy(setup);
    }
};

template <unsigned int size, unsigned int nfft, unsigned int noverlap, bool concurrent>
class RealvDSPSpectrogram: BasevDSPSpectrogram<float, size, nfft, noverlap, concurrent> {
    static constexpr auto calc = concurrent ? vDSP_Spectrogram_Real_P : vDSP_Spectrogram_Real;
    using typename BaseSpectrogram<float, size, nfft, noverlap>::OutputMatrix;
public:
    OutputMatrix calculate(const Tensor<float, size>& input) override {
        OutputMatrix output;
        calc(BasevDSPSpectrogram<float, size, nfft, noverlap, concurrent>::setup, input.value, output.value);
        return output;
    }
};


template <unsigned int size, unsigned int nfft, unsigned int noverlap, bool concurrent>
class ComplexvDSPSpectrogram: BasevDSPSpectrogram<std::complex<float>, size, nfft, noverlap, concurrent> {
    static constexpr auto calc = concurrent ? vDSP_Spectrogram_Complex_P : vDSP_Spectrogram_Complex;
    using typename BaseSpectrogram<std::complex<float>, size, nfft, noverlap>::OutputMatrix;
public:
    OutputMatrix calculate(const Tensor<std::complex<float>, size>& input) override {
        OutputMatrix output;
        calc(BasevDSPSpectrogram<std::complex<float>, size, nfft, noverlap, concurrent>::setup, (DSPComplex *)input.value, output.value);
        return output;
    }
};


#endif /* vdsp_spectrogram_h */
