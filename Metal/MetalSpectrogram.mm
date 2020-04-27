//
//  BaseMetalSpectrogram.m
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#import "_MetalSpectrogram.h"

struct GPUConfig {
    uint height;
    uint nfft;
    uint delta;
    uint ouputLength;
};


@implementation _MetalSpectrogram
{
    id<MTLDevice> device;
    id<MTLComputePipelineState> spectrogramFunctionPipState;
    id<MTLCommandQueue> commanQueue;
    id<MTLBuffer> configBuffer;
    unsigned int outputLength;
    MTLSize countOfThreadGroups;
    MTLSize threadPerGroup;
}

- (instancetype) initWithDevice: (id<MTLDevice>) device inputType: (MetalSpectrogramInputType) inputType andConfig: (SpectrogramConfig) config;
{
    self = [super init];
    if (self)
    {
        self->device = device;
        NSError* error = nil;
        auto defaultLibrary = [device newDefaultLibrary];
        if (defaultLibrary == nil)
         {
             NSLog(@"Failed to find the default library.");
             return nil;
         }
        NSString* functionName = inputType == real ?  @"spectrogram_real" : @"spectrogram_complex";
        unsigned int inputSize = inputType == real ? sizeof(float) : 2 * sizeof(float);

        auto spectrogram = [defaultLibrary newFunctionWithName:functionName];
        if (spectrogram == nil)
        {
            NSLog(@"Failed to find the adder function.");
            return nil;
        }
        spectrogramFunctionPipState = [device newComputePipelineStateWithFunction: spectrogram error:&error];
        if (spectrogramFunctionPipState == nil)
        {
            NSLog(@"Failed to created pipeline state object, error %@.", error);
            return nil;
        }
        commanQueue = [device newCommandQueue];
        if (commanQueue == nil)
        {
            NSLog(@"Failed to find the command queue.");
            return nil;
        }
        if (config.nfft % 2 != 0) {
            NSLog(@"nfft should be divided by 2");
            return nil;
        }
        auto delta = config.nfft - config.noverlap;
        if (delta == 0) {
            NSLog(@"Failed to noverlap the command queue.");
            return nil;
        }
        auto height = config.nfft / 2 + 1;
        if ((config.size - config.noverlap) % delta != 0) {
            NSLog(@"Failed size or noverlap");
            return nil;
        }
        auto width = (config.size - config.noverlap) / delta;
        outputLength = width * height;
        GPUConfig gpuConfig { .height = height , .nfft=config.nfft, .delta=delta, .ouputLength=outputLength };
        configBuffer = [device newBufferWithBytes:&gpuConfig length:sizeof(struct GPUConfig) options:MTLResourceStorageModeShared];
        _inputBufferSize = config.size * inputSize;
        _outputBufferSize = outputLength * sizeof(float);
        _inputBuffer = [device newBufferWithLength:_inputBufferSize options:MTLResourceStorageModeShared];
        _outputBuffer = [device newBufferWithLength:_outputBufferSize options:MTLResourceStorageModeShared];
        NSUInteger threadGroupSize = spectrogramFunctionPipState.maxTotalThreadsPerThreadgroup;
        NSUInteger countOfThreadGroups = outputLength / threadGroupSize + (outputLength % threadGroupSize == 0 ? 0 : 1);
        self->countOfThreadGroups = MTLSizeMake(countOfThreadGroups, 1, 1);
        self->threadPerGroup = MTLSizeMake(threadGroupSize, 1, 1);
    }

    return self;
}

- (void) compute
{
    auto commandBuffer = [commanQueue commandBuffer];
    assert(commandBuffer != nil);
    auto computeEncoder = [commandBuffer computeCommandEncoder];
    assert(computeEncoder != nil);
    [self encodeSpectrogramCommand:computeEncoder];
    [computeEncoder endEncoding];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
}

- (void)encodeSpectrogramCommand:(id<MTLComputeCommandEncoder>)computeEncoder {
    [computeEncoder setComputePipelineState:spectrogramFunctionPipState];
    [computeEncoder setBuffer:_inputBuffer offset:0 atIndex:0];
    [computeEncoder setBuffer:configBuffer offset:0 atIndex:1];
    [computeEncoder setBuffer:_outputBuffer offset:0 atIndex:2];
    [computeEncoder dispatchThreadgroups:countOfThreadGroups threadsPerThreadgroup:threadPerGroup];
}

@end
