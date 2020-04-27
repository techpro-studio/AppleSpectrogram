//
//  BaseMetalSpectrogram.h
//  audio_test
//
//  Created by Alex on 25.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Base.h"
#import "MetalPerformer.hpp"

NS_ASSUME_NONNULL_BEGIN

@interface _MetalSpectrogram : MetalPerformer

- (instancetype) initWithDevice: (id<MTLDevice>) device inputType: (SpectrogramInputType) inputType andSCP:(SpectrogramComputationParameters) parameters;
- (void) compute: (void*) input output: (float *)output;

@end

NS_ASSUME_NONNULL_END
