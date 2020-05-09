//
//  spectrogram.metal
//  audio_test
//
//  Created by Alex on 20.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

struct config {
    uint height;
    uint nfft;
    uint delta;
    uint ouputLength;
};

// this was moved to function, because it was an idea to put differeent implementations of Fourier transform;
float2 real_dft_step(device const float *input, uint k, uint nfft){
    float2 value {0.f, 0.f};
    for (uint n = 0; n < nfft; ++n) {
        float angle = 2 * M_PI_F * n * k / nfft;
        value += { *(input + n) * cos(angle), -1 * *(input + n) * sin(angle) };
    }
    return value;
}

float2 complex_dft_step(device const float2 *input, uint k, uint nfft){
    float2 value {0.f, 0.f};
    for (uint n = 0; n < nfft; ++n) {
        float angle = 2 * M_PI_F * n * k / nfft;
        float2 inputN = *(input + n);
        value += { inputN[0] * cos(angle) + inputN[1] * sin(angle) , -1 * inputN[0] * sin(angle) + inputN[1] * cos(angle) };
    }
    return value;
}

float calculate_magnitude(float2 complex) {
    float magnitude = sqrt(complex[0] * complex[0] + complex[1] * complex[1]);
    magnitude += 1.5849e-13;
    float result = 10.0f * log10(magnitude);
    return result;
}


kernel void spectrogram_real(device const float *input, device const config *config, device float *output, uint index[[thread_position_in_grid]])
{
    if (index >= config->ouputLength) { return; }
    uint index_inside = index % config->height;
    uint number_of_slice = (index - index_inside) / config->height;
    device const float *input_begin = input + number_of_slice * config->delta;
    float2 dft = real_dft_step(input_begin, index_inside, config->nfft);
    output[index] = calculate_magnitude(dft);
}

kernel void spectrogram_complex(device const float2 *input, device const config *config, device float *output, uint index[[thread_position_in_grid]])
{
    if (index >= config->ouputLength) { return; }
    uint index_inside = index % config->height;
    uint number_of_slice = (index - index_inside) / config->height;
    device const float2 *input_begin = input + number_of_slice * config->delta;
    float2 dft = complex_dft_step(input_begin, index_inside, config->nfft);
    output[index] = calculate_magnitude(dft);
}




