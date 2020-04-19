//
//  AppDelegate.m
//  MarlinViewer
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#import "AppDelegate.h"

#include <marlin.hpp>

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    marlin::print("Hello World");
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
