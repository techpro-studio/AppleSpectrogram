//
//  vDSPSpectrogram.c
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#include "vDSPSpectrogram.h"

void CalculateMagnitude(DSPSplitComplex *split, float * columnPtr, const int vectorSize)
{
    vDSP_zvmags(split, 1, columnPtr, 1, vectorSize);
    vvsqrtf(columnPtr, columnPtr, &vectorSize);
    const Float32 kAdjust0DB = 1.5849e-13;
    vDSP_vsadd(columnPtr, 1, &kAdjust0DB, columnPtr, 1, vectorSize);
    Float32 one = 1;
    vDSP_vdbcon(columnPtr, 1, &one, columnPtr, 1, vectorSize, 0);
}

vDSP_Spectrogram_Setup vDSP_Spectrogram_Setup_Init(SpectrogramComputationParameters parameters) {
    vDSP_DFT_Setup dftSetup = vDSP_DFT_zop_CreateSetup(NULL, parameters.nfft, vDSP_DFT_FORWARD);
    return (vDSP_Spectrogram_Setup){ .parameters = parameters, .setup = dftSetup };
}

void vDSP_Spectrogram_Setup_Destroy(vDSP_Spectrogram_Setup setup) {
    vDSP_DFT_DestroySetup(setup.setup);
}

void vDSP_Spectrogram_Real(vDSP_Spectrogram_Setup setup, float* input, float* output) {
    float outputMemory[setup.parameters.nfft * 2];
    float inputImag [setup.parameters.nfft];
    memset(inputImag, 0.0f, setup.parameters.nfft*sizeof(float));
    DSPSplitComplex outputSplit = {outputMemory, outputMemory + setup.parameters.nfft};
    for (int r = 0; r < setup.parameters.outputRow; ++r) {
        vDSP_DFT_Execute(setup.setup,
                         input + r * setup.parameters.step, inputImag,
                         outputSplit.realp, outputSplit.imagp);
        CalculateMagnitude(&outputSplit, output + r * setup.parameters.outputColumn, setup.parameters.outputColumn);
    }
}

void vDSP_Spectrogram_Real_P(vDSP_Spectrogram_Setup setup, float* input, float* output){
    __block float* inputImag = calloc(setup.parameters.nfft, sizeof(float));
    memset(inputImag, 0.0f, setup.parameters.nfft*sizeof(float));
    dispatch_apply(setup.parameters.outputRow, DISPATCH_APPLY_AUTO, ^(size_t r) {
        float outputMemory[setup.parameters.nfft * 2];
        DSPSplitComplex outputSplit = {outputMemory, outputMemory + setup.parameters.nfft};
        vDSP_DFT_Execute(setup.setup,
                         input + r * setup.parameters.step, inputImag,
                         outputSplit.realp, outputSplit.imagp);
        CalculateMagnitude(&outputSplit, output + r * setup.parameters.outputColumn, setup.parameters.outputColumn);;
    });
    free(inputImag);
}

void vDSP_Spectrogram_Complex(vDSP_Spectrogram_Setup setup, DSPComplex* input, float* output) {
    float outputMemory[setup.parameters.nfft * 2];
    float inputMemory[setup.parameters.nfft * 2];
    DSPSplitComplex inputSplit = {inputMemory, inputMemory + setup.parameters.nfft};
    DSPSplitComplex outputSplit = {outputMemory, outputMemory + setup.parameters.nfft};
    for (int r = 0; r < setup.parameters.outputRow; ++r) {
        vDSP_ctoz(input + r * setup.parameters.step, 2, &inputSplit, 1, setup.parameters.nfft);
        vDSP_DFT_Execute(setup.setup,
                         inputSplit.realp, inputSplit.imagp,
                         outputSplit.realp, outputSplit.imagp);
        CalculateMagnitude(&outputSplit, output + r * setup.parameters.outputColumn, setup.parameters.outputColumn);
    }
}

void vDSP_Spectrogram_Complex_P(vDSP_Spectrogram_Setup setup, DSPComplex* input, float* output) {
    dispatch_apply(setup.parameters.outputRow, DISPATCH_APPLY_AUTO, ^(size_t r) {
        float outputMemory[setup.parameters.nfft * 2];
        float inputMemory[setup.parameters.nfft * 2];
        DSPSplitComplex inputSplit = {inputMemory, inputMemory + setup.parameters.nfft};
        DSPSplitComplex outputSplit = {outputMemory, outputMemory + setup.parameters.nfft};
        vDSP_ctoz(input + r * setup.parameters.step, 2, &inputSplit, 1, setup.parameters.nfft);
        vDSP_DFT_Execute(setup.setup,
                         inputSplit.realp, inputSplit.imagp,
                         outputSplit.realp, outputSplit.imagp);
        CalculateMagnitude(&outputSplit, output + r * setup.parameters.outputColumn, setup.parameters.outputColumn);
    });
}
