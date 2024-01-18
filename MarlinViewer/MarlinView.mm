//
//  MarlinView.m
//  MarlinViewer
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#import "MarlinView.h"

#include <marlin/marlin.hpp>
#include <marlin/scene/mesh.hpp>
#include <marlin/scene/scene.hpp>

#import <QuartzCore/CAMetalLayer.h>

#include <vector>

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
        
    marlin::ScenePtr scene = marlin::Scene::create();
    static bool first = true;
    if ( first )
    {

        {
            const std::vector<marlin::Vec3f> vertices = {
                {-0.5f, -0.5f, 0.0},
                { 0.5f, -0.5f, 0.0f},
                { 0.5f,  0.5f, 0.0f},
                {-0.5f,  0.5f, 0.0f}
            };

            const std::vector<marlin::Vec3f> colors = {
                {1.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 1.0f},
                {1.0f, 1.0f, 1.0f}
            };

            const std::vector< uint32_t > indices = {
                0, 1, 2, 2, 3, 0
            };

            marlin::Mesh mesh;
            mesh.setVertices( vertices );
            mesh.setColors( colors );
            mesh.setIndices( indices );
            
            marlin::GeometryPtr geometry = marlin::Geometry::create( scene );
            geometry->setLOD( mesh, 0 );
            
            scene->addObject( geometry );
        }
        
        {
            const std::vector<marlin::Vec3f> vertices = {
                {-1.5f, -1.5f, 0.0},
                { 1.5f, -1.5f, 0.0f},
                { 1.5f,  1.5f, 0.0f},
                {-1.5f,  1.5f, 0.0f}
            };

            const std::vector<marlin::Vec3f> colors = {
                {1.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 1.0f},
                {1.0f, 1.0f, 1.0f}
            };

            const std::vector< uint32_t > indices = {
                0, 1, 2, 2, 3, 0
            };

            marlin::Mesh mesh;
            mesh.setVertices( vertices );
            mesh.setColors( colors );
            mesh.setIndices( indices );
            
            marlin::GeometryPtr geometry = marlin::Geometry::create( scene );
            geometry->setLOD( mesh, 0 );
            
            scene->addObject( geometry );
        }
        
        first = false;
    }
    
    marlin::render( scene );
    
    return kCVReturnSuccess;
}

- (void)viewDidLoad {
    
    [super viewDidLoad];

    // Back the view with a layer created by the makeBackingLayer method.
    self.view.wantsLayer = YES;

    marlin::init((__bridge void*)(self.view.layer ));
    
    CVDisplayLinkCreateWithActiveCGDisplays(&m_displayLink);
    CVDisplayLinkSetOutputCallback(m_displayLink, &displayLinkCallback,  (__bridge void*)(self));
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
