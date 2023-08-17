//
//  InfoTabMatrix.m
//  Transmission OG
//
//  Created by Mitchell Livingston on 12/21/18.
//  Copyright © 2018 The Transmission OG Project. All rights reserved.
//

#import "InfoTabMatrix.h"
#import "InfoTabButtonCell.h"

@implementation InfoTabMatrix

- (void)viewDidChangeEffectiveAppearance;
{
    for (InfoTabButtonCell *cell in self.cells)
    {
        [cell reloadAppearance];
    }
}

@end
