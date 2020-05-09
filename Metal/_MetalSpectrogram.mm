//
//  BaseMetalSpectrogram.m
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#import "_MetalSpectrogram.hpp"

struct GPUConfig {
    uint height;
    uint nfft;
    uint delta;
    uint ouputLength;
};

@implementation _MetalSpectrogram
{
    id<MTLBuffer> configBuffer;
    id<MTLBuffer> inputBuffer;
    id<MTLBuffer> outputBuffer;

    unsigned int outputLength;
    unsigned int inputBufferSize;
    unsigned int outputBufferSize;
}

- (instancetype) initWithDevice: (id<MTLDevice>) device inputType: (SpectrogramInputType) inputType andSCP:(SpectrogramComputationParameters) parameters;
{
    self = [super init];
    if (self)
    {
        self->device = device;
        NSString* functionName = inputType == real ?  @"spectrogram_real" : @"spectrogram_complex";
        unsigned int inputSize = inputType == real ? sizeof(float) : 2 * sizeof(float);
        auto error = [super setupMetalFunction: functionName];
        if (error != nil){
            NSLog(@"%@", error.localizedDescription);
            return nil;
        }
        outputLength = parameters.outputRow * parameters.outputColumn;
        GPUConfig gpuConfig { .height = parameters.outputColumn , .nfft=parameters.nfft, .delta=parameters.step, .ouputLength=outputLength };
        configBuffer = [device newBufferWithBytes:&gpuConfig length:sizeof(struct GPUConfig) options:MTLResourceStorageModeShared];
        inputBufferSize = parameters.inputSize * inputSize;
        outputBufferSize = outputLength * sizeof(float);
        inputBuffer = [device newBufferWithLength:inputBufferSize options:MTLResourceStorageModeShared];
        outputBuffer = [device newBufferWithLength:outputBufferSize options:MTLResourceStorageModeShared];
        [super calculateThreadParameters:outputLength];
    }

    return self;
}


- (void) compute: (void*) input output: (float *)output
{
    memcpy(inputBuffer.contents, input, inputBufferSize);;
    __weak auto weakSelf = self;
    [super computeWithEncoding:^(id<MTLComputeCommandEncoder> encoder) {
        __strong auto strong = weakSelf;
        [encoder setComputePipelineState: strong->function];
        [encoder setBuffer: strong->inputBuffer offset:0 atIndex:0];
        [encoder setBuffer: strong->configBuffer offset:0 atIndex:1];
        [encoder setBuffer: strong->outputBuffer offset:0 atIndex:2];
        [encoder dispatchThreadgroups: strong->countOfThreadGroups threadsPerThreadgroup:strong->threadPerGroup];
    }];
    memcpy(output, outputBuffer.contents, outputBufferSize);
}

@end
