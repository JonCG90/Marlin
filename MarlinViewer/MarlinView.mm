//
//  MarlinView.m
//  MarlinViewer
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#import "MarlinView.h"

#include <marlin.hpp>

#import <QuartzCore/CAMetalLayer.h>

@interface MarlinViewController ()

@end

@implementation MarlinViewController {
    CVDisplayLinkRef m_displayLink;
}

// Rendering loop callback function for use with a CVDisplayLink.
static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp* now,
                                    const CVTimeStamp* outputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags* flagsOut,
                                    void* target) {
    
    marlin::render();
    
    return kCVReturnSuccess;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    // Back the view with a layer created by the makeBackingLayer method.
    self.view.wantsLayer = YES;

    marlin::init();
    
    marlin::MlnContext context;
    context.createInstance();

    CVDisplayLinkCreateWithActiveCGDisplays(&m_displayLink);
    CVDisplayLinkSetOutputCallback(m_displayLink, &displayLinkCallback,  (__bridge void *)(self));
    CVDisplayLinkStart(m_displayLink);
}

@end

@implementation MarlinView

// Indicates that the view wants to draw using the backing layer instead of using drawRect.
- (BOOL)wantsUpdateLayer {
    return YES;
}

// Returns a Metal-compatible layer.
+ (Class)layerClass {
    return [CAMetalLayer class];
}

// If the wantsLayer property is set to YES, this method will be invoked to return a layer instance.
-(CALayer*)makeBackingLayer {
    
    CALayer* layer = [self.class.layerClass layer];
    CGSize viewScale = [self convertSizeToBacking: CGSizeMake(1.0, 1.0)];
    layer.contentsScale = MIN(viewScale.width, viewScale.height);
    return layer;
}

@end
