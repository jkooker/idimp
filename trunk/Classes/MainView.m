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
    
    _touchPoints = [NSMutableArray arrayWithCapacity:5]; // the 6th touch point causes a touchCancelled event
    [_touchPoints retain];
    NSLog(@"_touchPoints = %02X, count = %d", _touchPoints, [_touchPoints count]);
}

- (void)drawRect:(CGRect)rect {
    //NSLog(@"MainView::drawRect");
    
    CGRect bounds = [self bounds];
    //NSLog(@"bounds: w %f h %f", bounds.size.width, bounds.size.height);

    // Drawing code
    CGContextRef contextRef = UIGraphicsGetCurrentContext();
    
    for (NSValue *v in _touchPoints)
    {
        CGPoint p = [v CGPointValue];
        //NSLog(@"point: %f, %f", p.x, p.y);
        
        float xratio = (p.x/bounds.size.width);
        float yratio = 1 - (p.y/bounds.size.height);
        //NSLog(@"x ratio: %f, y ratio: %f", xratio, yratio);
        
        CGContextSetRGBFillColor(contextRef, 0, xratio, 1-xratio, 0.8*yratio);
        CGContextSetRGBStrokeColor(contextRef, 0, xratio, 1-xratio, 0.2+0.8*yratio);
        CGContextSetLineWidth(contextRef, 3);
        
        // Draw a circle (filled)
        CGContextFillEllipseInRect(contextRef, CGRectMake(p.x - circleRadius, p.y - circleRadius, 2*circleRadius, 2*circleRadius));

        // Draw a circle (border only)
        CGContextStrokeEllipseInRect(contextRef, CGRectMake(p.x - circleRadius, p.y - circleRadius, 2*circleRadius, 2*circleRadius));
    }
    NSLog(@"MainView::drawRect finished");
}

- (void)dealloc {
    [_touchPoints release];
    [super dealloc];
}


#pragma mark Touch Handlers
- (void)touchesBegan: (NSSet *)touches withEvent:(UIEvent *)event {
    //NSLog(@"touchesBegan. %@", touches);
    
    // add points to the touchpoints array
    for (UITouch *touch in touches)
    {
        [_touchPoints addObject:[NSValue valueWithCGPoint:[touch locationInView:self]]];
    }
    
    [self setNeedsDisplay];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesMoved. %@\n%@", touches, _touchPoints);
    
    // find the matching point (by looking at previous locations) and modify it
    for (UITouch *touch in touches)
    {
        for (NSUInteger i = 0; i < [_touchPoints count]; i++)
        {
            NSValue *v = [_touchPoints objectAtIndex:i];
            
            if ([v CGPointValue].x == [touch previousLocationInView:self].x &&
                [v CGPointValue].y == [touch previousLocationInView:self].y)
            {
                [_touchPoints replaceObjectAtIndex:i withObject:[NSValue valueWithCGPoint:[touch locationInView:self]]];
                break;
            }
        }
    }

    [self setNeedsDisplay];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesEnded. %@", touches);
    
    for (UITouch *touch in touches)
    {
        for (NSUInteger i = 0; i < [_touchPoints count]; i++)
        {
            NSValue *v = [_touchPoints objectAtIndex:i];
            
            if ([v CGPointValue].x == [touch locationInView:self].x &&
                [v CGPointValue].y == [touch locationInView:self].y)
            {
                //NSLog(@"found it!");
                [_touchPoints removeObjectAtIndex:i];
                break;
            }
        }
    }

    [self setNeedsDisplay];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSLog(@"touchesCancelled");
    
    [_touchPoints removeAllObjects];
    
    [self setNeedsDisplay];
}

@end
