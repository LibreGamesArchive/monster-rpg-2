#include <allegro.h>
#include <Cocoa/Cocoa.h>

/*
#define ALLEGRO_SRC 1 // HACK!
#include <allegro/platform/aintmac.h>
#include <allegro/platform/alosx.h>
#include <allegro/platform/aintosx.h>
#import <CoreFoundation/CoreFoundation.h>
*/

/* NSImageFromAllegroBitmap:
 * Create an NSImage from an Allegro bitmap
 * This could definitely be speeded up if necessary.
 */
NSImage* NSImageFromAllegroBitmap(BITMAP* bmp)
{
   int w = bmp->w;
   int h = bmp->h;
   NSImage* img = [[NSImage alloc] initWithSize: NSMakeSize((float) w, (float) h)];
   NSBitmapImageRep* rep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes: NULL // Allocate memory yourself
      pixelsWide:w 
      pixelsHigh:h 
      bitsPerSample: 8 
      samplesPerPixel: 4 
      hasAlpha:YES 
      isPlanar:NO 
      colorSpaceName:NSDeviceRGBColorSpace 
      bytesPerRow: 0 // Calculate yourself
      bitsPerPixel:0 ];// Calculate yourself
   int x, y;
   for (y = 0; y<h; ++y) {
      for (x = 0; x<w; ++x) {
      	 int c = _getpixel32(bmp, x, y);
         unsigned char* ptr = [rep bitmapData] + y * [rep bytesPerRow] + x * ([rep bitsPerPixel]/8);
	 *(ptr+0) = (int)(getr32(c) * (geta32(c)/255.0));
	 *(ptr+1) = (int)(getg32(c) * (geta32(c)/255.0));
	 *(ptr+2) = (int)(getb32(c) * (geta32(c)/255.0));
	 *(ptr+3) = geta32(c);
      }
   }
   [img addRepresentation:rep];
   [rep release];
   return [img autorelease];
}


/* set_icon:
 * Set the icon - OS X doesn't have per-window icons so 
 * ignore the display parameter
 */
void my_osx_set_icon(BITMAP* bitmap)
{
   [NSApp setApplicationIconImage: NSImageFromAllegroBitmap(bitmap)];
}

