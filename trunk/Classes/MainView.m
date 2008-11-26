//
//  MainView.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "MainView.h"

@implementation MainView

- (id)initWithFrame:(CGRect)frame {
    NSLog(@"MainView::initWithFrame");
    if (self = [super initWithFrame:frame]) {
        // Initialization code
    }
    return self;
}

- (void)awakeFromNib
{
    NSLog(@"MainView::awakeFromNib");
    
    _audioEngine = AudioEngine::getInstance();
    _synth = _audioEngine->getSynth();
    
    CGRect bounds = [self bounds];
    NSLog(@"bounds: w %f h %f", bounds.size.width, bounds.size.height);
    _synth->setDisplayBounds(bounds); // needed to map position to parameter ranges    
    _audioEngine->start();
}

- (void)drawRect:(CGRect)rect {
    //NSLog(@"MainView::drawRect");
    
    CGRect bounds = [self bounds];
    CGContextRef contextRef = UIGraphicsGetCurrentContext();
    _synth->drawVoices(contextRef, bounds);
}

- (void)dealloc {
    [super dealloc];
}


#pragma mark Touch Handlers
- (void)touchesBegan: (NSSet *)touches withEvent:(UIEvent *)event {
    //NSLog(@"touchesBegan. %@", touches);
    
    // add touch voices to the synth
    CGRect bounds = [self bounds];
    for (UITouch *touch in touches)
    {
        CGPoint p = [touch locationInView:self];
        bool success = _synth->addTouchVoice(p.x, p.y);
        if (!success)
        {
            NSLog(@"MainView::touchesBegan no free voices!");
            // should we do anything here?        
        }
    }
    
    [self setNeedsDisplay];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesMoved. %@\n%@", touches, _touchPoints);
    
    // find the matching point (by looking at previous locations) and modify it
    for (UITouch *touch in touches)
    {
        bool success = _synth->updateTouchVoice([touch locationInView:self].x,
                                                [touch locationInView:self].y,
                                                [touch previousLocationInView:self].x,
                                                [touch previousLocationInView:self].y);
        if (!success)
        {
            NSLog(@"MainView::touchesMoved could not match a voice!");
            // should we do anything here?        
        }

    }
    
    // So we can assume that we will never get a new touch with "touchedMoved"?

    [self setNeedsDisplay];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesEnded. %@", touches);
    
    // so we always get a touchesMoved event before a touchesEnded event if the touch has moved
    // from its original location?
            
    for (UITouch *touch in touches)
    {
        bool success = _synth->removeTouchVoice([touch locationInView:self].x, [touch locationInView:self].y);
        if (!success)
        {
            NSLog(@"MainView::touchesEnded could not match a voice!");
            // should we do anything here?        
        }   
    }

    [self setNeedsDisplay];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesCancelled");

    _synth->removeAllVoices();
    
    [self setNeedsDisplay];
}

@end