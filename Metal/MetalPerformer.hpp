//
//  MetalPerformer.h
//  audio_test
//
//  Created by Alex on 27.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^CommandEncoder)(id<MTLComputeCommandEncoder>);

@interface MetalPerformer : NSObject
{
    @protected id<MTLDevice> device;
    @protected id<MTLComputePipelineState> function;
    @protected id<MTLCommandQueue> commandQueue;
    @protected MTLSize countOfThreadGroups;
    @protected MTLSize threadPerGroup;
}

- (NSError *)setupMetalFunction: (NSString*) functionName;
- (void) computeWithEncoding: (CommandEncoder) encoder;
- (void) calculateThreadParameters: (unsigned int) length;

@end

NS_ASSUME_NONNULL_END
