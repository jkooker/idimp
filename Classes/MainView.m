//
//  MainView.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "MainView.h"

static const float circleRadius = 80;

@implementation MainView

@synthesize _touchPoints;

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
    
    //_touchPoints = [NSMutableArray arrayWithCapacity:MAX_TOUCHES]; // the 6th touch point causes a touchCancelled event
    //[_touchPoints retain];
    // NOTE: if I don't use this _touchPoints array, will I still get touchCancelled events?  What will trigger them?
    
    // TODO: these voices need to come from the synth so we can actually get audio
    
    //_audioEngine = NULL;
    //_audioEngine = new AudioEngine();
    //_voices = _audioEngine->m_synth->getVoices();
    //_numVoices = _audioEngine->m_synth->getNumVoices();
    
    _numVoices = NUM_VOICES;
    
    CGRect bounds = [self bounds];
    NSLog(@"bounds: w %f h %f", bounds.size.width, bounds.size.height);
    for (int i = 0; i < _numVoices; i++)
    {
        _voices[i].setMaxX(bounds.size.width);
        _voices[i].setMaxY(bounds.size.height);
    }
    
    //_audioEngine->start();
}

- (void)drawRect:(CGRect)rect {
    //NSLog(@"MainView::drawRect");
    
    CGRect bounds = [self bounds];
    //NSLog(@"bounds: w %f h %f", bounds.size.width, bounds.size.height);

    // Drawing code
    CGContextRef contextRef = UIGraphicsGetCurrentContext();
    
    for (int i = 0; i < _numVoices; i++)
    {
        // only draw voices that are turned on
        if (!_voices[i].isOn()) continue;
        
        float x = _voices[i].m_x;
        float y = _voices[i].m_y;
            
        float xratio = (x / bounds.size.width);
        float yratio = 1 - (y / bounds.size.height);
        
        CGContextSetRGBFillColor(contextRef, 0, xratio, 1 - xratio, 0.8 * yratio);
        CGContextSetRGBStrokeColor(contextRef, 0, xratio, 1 - xratio, 0.2 + 0.8 * yratio);
        CGContextSetLineWidth(contextRef, 3);
        
        // Draw a circle (filled)
        CGContextFillEllipseInRect(contextRef, CGRectMake(x - circleRadius, y - circleRadius, 2 * circleRadius, 2 * circleRadius));

        // Draw a circle (border only)
        CGContextStrokeEllipseInRect(contextRef, CGRectMake(x - circleRadius, y - circleRadius, 2 * circleRadius, 2 * circleRadius));
    }
}

- (void)dealloc {
    [_touchPoints release];
    /*if (_audioEngine != NULL)
    {
        delete _audioEngine;
    }*/
    [super dealloc];
}


#pragma mark Touch Handlers
- (void)touchesBegan: (NSSet *)touches withEvent:(UIEvent *)event {
    //NSLog(@"touchesBegan. %@", touches);
    
    // add points to the touchpoints array
    int nextIndex = 0;
    CGRect bounds = [self bounds];
    for (UITouch *touch in touches)
    {
        CGPoint p = [touch locationInView:self];
        // find unused voice
        for (int i = nextIndex; i < _numVoices; i++)
        {
            if (!_voices[i].isOn())
            {
                // found an unused voice
                _voices[i].setPosition(p.x, p.y);
                _voices[i].turnOn();
                nextIndex = i + 1;
                break;
            }
        }
        // if we got here, there were no unused voices - now what??
    }
    
    [self setNeedsDisplay];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesMoved. %@\n%@", touches, _touchPoints);
    
    // find the matching point (by looking at previous locations) and modify it
    for (UITouch *touch in touches)
    {
        for (int i = 0; i < _numVoices; i++)
        {
            if (!_voices[i].isOn()) continue;
            
            if (_voices[i].m_x == [touch previousLocationInView:self].x &&
                _voices[i].m_y == [touch previousLocationInView:self].y)
            {
                // found a match - update its position
                CGPoint pMoved = [touch locationInView:self];
                _voices[i].setPosition(pMoved.x, pMoved.y);
                break;
            }
        }
    }
    
    // So we can assume that we will never get a new touch with "touchedMoved"?

    [self setNeedsDisplay];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesEnded. %@", touches);
    
    for (UITouch *touch in touches)
    {
        for (int i = 0; i < _numVoices; i++)
        {
            if (!_voices[i].isOn()) continue;
            
            // so we always get a touchesMoved event before a touchesEnded event if the touch has moved
            // from its original location?
            if (_voices[i].m_x == [touch locationInView:self].x &&
                _voices[i].m_y == [touch locationInView:self].y)
            {
                // found a match - turn it off
                _voices[i].turnOff();
                break;
            }
        }
    }

    [self setNeedsDisplay];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesCancelled");

    for (int i = 0; i < _numVoices; i++)
    {
        _voices[i].turnOff();
    }
    
    [self setNeedsDisplay];
}

@end
