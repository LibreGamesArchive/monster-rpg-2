//
//  Shader.fsh
//  MoRPG2-lite
//
//  Created by Trent Gamblin on 11-04-23.
//  Copyright 2011 Nooskewl. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
