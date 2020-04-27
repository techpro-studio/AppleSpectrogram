//
//  MetalPerformer.m
//  audio_test
//
//  Created by Alex on 27.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#import "MetalPerformer.hpp"

@implementation MetalPerformer

- (NSError *) createError: (NSString *) description code: (NSInteger) code
{
    return [NSError errorWithDomain:@"com.metal.performer" code:code userInfo:@{NSLocalizedDescriptionKey: description}];
}

- (NSError *)setupMetalFunction: (NSString*) functionName {

    auto defaultLibrary = [device newDefaultLibrary];
    if (defaultLibrary == nil)
    {
        return [self createError:@"Failed to find the default library." code: 0];
    }
    auto spectrogram = [defaultLibrary newFunctionWithName:functionName];
    if (spectrogram == nil)
    {
        return [self createError:@"Failed to find the function." code: 1];
    }
    NSError* error = nil;
    function = [device newComputePipelineStateWithFunction: spectrogram error:&error];
    if (function == nil)
    {
        return [self createError:[NSString stringWithFormat:@"Failed to created pipeline state object, error %@.", error.localizedDescription] code:2];
    }
    commandQueue = [device newCommandQueue];
    if (commandQueue == nil)
    {
        return [self createError:@"Failed to find the command queue." code:3];
    }
    return nil;
}

- (void) calculateThreadParameters: (unsigned int) length
{
    NSUInteger threadGroupSize = function.maxTotalThreadsPerThreadgroup;
    NSUInteger countOfThreadGroups = length / threadGroupSize + (length % threadGroupSize == 0 ? 0 : 1);
    self->countOfThreadGroups = MTLSizeMake(countOfThreadGroups, 1, 1);
    self->threadPerGroup = MTLSizeMake(threadGroupSize, 1, 1);
}

- (void) computeWithEncoding: (CommandEncoder) encoder
{
    auto commandBuffer = [commandQueue commandBuffer];
    assert(commandBuffer != nil);
    auto computeEncoder = [commandBuffer computeCommandEncoder];
    assert(computeEncoder != nil);
    encoder(computeEncoder);
    [computeEncoder endEncoding];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
}

@end
