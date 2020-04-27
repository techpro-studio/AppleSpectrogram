
//
//  Common..h
//  audio_test
//
//  Created by Alex on 26.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#ifndef Common__h
#define Common__h

#if defined __cplusplus
extern "C" {
#endif

typedef struct {
    const unsigned int nfft;
    const unsigned int step;
    const unsigned int inputSize;
    const unsigned int outputRow;
    const unsigned int outputColumn;
} SpectrogramComputationParameters;
#if defined __cplusplus
}
#endif

enum SpectrogramInputType { real, complex };

#endif

