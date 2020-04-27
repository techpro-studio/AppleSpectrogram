//
//  vDSPSpectrogram.h
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#ifndef vDSPSpectrogram_h
#define vDSPSpectrogram_h

#include <Accelerate/Accelerate.h>
#include "Base.h"

#if defined __cplusplus
extern "C" {
#endif

// this should be calculated one time somewhere; its const parameters;
typedef struct {
    SpectrogramComputationParameters parameters;
    vDSP_DFT_Setup setup;
} vDSP_Spectrogram_Setup;


vDSP_Spectrogram_Setup vDSP_Spectrogram_Setup_Init(SpectrogramComputationParameters parameters);

void vDSP_Spectrogram_Setup_Destroy(vDSP_Spectrogram_Setup setup);

void CalculateMagnitude(DSPSplitComplex *split, float * columnPtr, const int vectorSize);

void vDSP_Spectrogram_Real(vDSP_Spectrogram_Setup setup, float* input, float* output);

void vDSP_Spectrogram_Real_P(vDSP_Spectrogram_Setup setup, float* input, float* output);

void vDSP_Spectrogram_Complex(vDSP_Spectrogram_Setup setup, DSPComplex* input, float* output);

void vDSP_Spectrogram_Complex_P(vDSP_Spectrogram_Setup setup, DSPComplex* input, float* output);


#if defined __cplusplus
}
#endif

#endif /* vDSPSpectrogram_h */
