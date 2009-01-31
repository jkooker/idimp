// Copyright (c) 2009 Michelle Daniels and John Kooker
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

//
//  MainView.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
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
    //NSLog(@"bounds: w %f h %f", bounds.size.width, bounds.size.height);
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
    
    // TODO: it might be cool if double-taps create a persistent voice which stays alive even 
    // after the user removes their finger.  a later tap on that voice would remove it?
    
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
        //NSLog(@"MainView::touchesBegan added x = %f, y = %f\n", p.x, p.y);
    }
    
    [self setNeedsDisplay];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesMoved. %@\n", touches);
    
    // find the matching point (by looking at previous locations) and modify it
    for (UITouch *touch in touches)
    {
        bool success = _synth->updateTouchVoice([touch locationInView:self].x,
                                                [touch locationInView:self].y,
                                                [touch previousLocationInView:self].x,
                                                [touch previousLocationInView:self].y);
        if (!success)
        {
            NSLog(@"MainView::touchesMoved could not match voice: x = %f, y = %f", [touch locationInView:self].x, [touch locationInView:self].y);
            // should we do anything here?        
        }

    }

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
            NSLog(@"MainView::touchesEnded could not match voice: x = %f, y = %f", [touch locationInView:self].x, [touch locationInView:self].y);
            //_synth->printVoices();
            // should we do anything here?        
        }   
    }
    //_synth->printVoices();

    [self setNeedsDisplay];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesCancelled");

    _synth->removeAllVoices();
    
    [self setNeedsDisplay];
}

@end
