//
//  spectro_impl.h
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#ifndef spectro_impl_h
#define spectro_impl_h

#include <Accelerate/Accelerate.h>
#include "Tensor.hpp"
#include <complex>
#include <memory>
#include "Base.h"
#include <type_traits>



template<typename Input, uint size, uint nfft, uint noverlap>
class BaseSpectrogram {
public:

    static constexpr bool isFloat = std::is_same<Input, float>::value;
    static_assert(isFloat || std::is_same<Input, std::complex<float>>::value, "Input should be float or complex");
    
    static constexpr SpectrogramInputType inputType = []{
        if constexpr(isFloat){
            return SpectrogramInputType::real;
        }
        return SpectrogramInputType::complex;
    }();

    static constexpr unsigned int column = inputType == real ?  nfft / 2 + 1 : nfft;
    static constexpr auto step = nfft - noverlap;

    static_assert(nfft != noverlap, "nfft should not be equal to noverlap");
    static_assert(nfft > 0, "nfft should not be more than 0");
    static_assert(nfft % step == 0, "nfft should divide on delta");
    static_assert(size % step == 0, "size should divide on delta");
    static_assert(size >= nfft, "size should be greater or equal than nftt");

    static constexpr unsigned int row = (size - noverlap) /  step;
    static constexpr SpectrogramComputationParameters scp = { .nfft = nfft, .step = step , .outputRow = row, .outputColumn = column, .inputSize = size };

    typedef Tensor<float, row, column> OutputMatrix;

    virtual OutputMatrix calculate(const Tensor<Input, size>& input) = 0;

};






#endif /* spectro_impl_h */
