#import <Foundation/Foundation.h>
#import <CoreAudio/CoreAudioTypes.h>
#import <AudioToolbox/AudioQueue.h>
#import <AudioToolbox/AudioFile.h>
#import <AudioToolbox/ExtendedAudioFile.h>
#import <AudioToolbox/AudioToolbox.h>
#ifdef IPHONE
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#endif
#define ASSERT ALLEGRO_ASSERT
#include <allegro5/platform/aintiphone.h>

#include "monster2.hpp"

// Make configurable
#define NUM_BUFFERS (4)

typedef struct DATA {
	char *filename;
	bool volume_off;
	AudioFileID audioFileID;
	UInt32 numPacketsToRead;
	UInt32 currentPacket;
	UInt64 totalPackets;
	AudioStreamPacketDescription *packetDescs;
} DATA;

static AudioQueueRef queue;
static DATA data;
static AudioQueueBufferRef buffers[NUM_BUFFERS];
static ALLEGRO_THREAD *thread;

/* Audio queue callback */
static void handle_buffer(
   void *in_data,
   AudioQueueRef inAQ,
   AudioQueueBufferRef inBuffer)
{
	UInt32 numBytesReadFromFile;
	UInt32 numPackets = data.numPacketsToRead;
	//UInt32 tmp = numPackets;

	AudioFileReadPackets (
		data.audioFileID,
		false,
		&numBytesReadFromFile,
		data.packetDescs, 
		data.currentPacket,
		&numPackets,
		inBuffer->mAudioData 
	);

	if (numPackets > 0) {
		inBuffer->mAudioDataByteSize = numBytesReadFromFile;
		AudioQueueEnqueueBuffer(
			inAQ,
			inBuffer,
			data.packetDescs ? numPackets : 0,
			data.packetDescs
		);
		data.currentPacket += numPackets;
	}
	else {
		// Stop queue?
	}
	
	if (data.currentPacket >= data.totalPackets) {// || numPackets < tmp) {
		data.currentPacket = 0;
	}
}

static void DeriveBufferSize (
    AudioStreamBasicDescription *ASBDesc,                            // 1
    UInt32                      maxPacketSize,                       // 2
    Float64                     seconds,                             // 3
    UInt32                      *outBufferSize,                      // 4
    UInt32                      *outNumPacketsToRead                 // 5
) {
    static const int maxBufferSize = 0x50000;                        // 6
    static const int minBufferSize = 0x4000;                         // 7
 
    if (ASBDesc->mFramesPerPacket != 0) {                             // 8
        Float64 numPacketsForTime =
            ASBDesc->mSampleRate / ASBDesc->mFramesPerPacket * seconds;
        *outBufferSize = numPacketsForTime * maxPacketSize;
    } else {                                                         // 9
        *outBufferSize =
            maxBufferSize > maxPacketSize ?
                maxBufferSize : maxPacketSize;
    }
 
    if (                                                             // 10
        *outBufferSize > maxBufferSize &&
        *outBufferSize > maxPacketSize
    )
        *outBufferSize = maxBufferSize;
    else {                                                           // 11
        if (*outBufferSize < minBufferSize)
            *outBufferSize = minBufferSize;
    }
 
    *outNumPacketsToRead = *outBufferSize / maxPacketSize;           // 12
}

static void *stream_proc(ALLEGRO_THREAD *thread, void *unused)
{
#if defined IPHONE || defined ALLEGRO_MACOSX
	POOL_BEGIN
#endif

   (void)unused;


   AudioStreamBasicDescription desc;

    CFURLRef audioFileURL =
    CFURLCreateFromFileSystemRepresentation (           // 1
        NULL,                                           // 2
        (const UInt8 *) data.filename,                       // 3
        strlen (data.filename),                              // 4
        false                                           // 5
    );

    AudioFileOpenURL (                                  // 2
        audioFileURL,                                   // 3
        kAudioFileReadPermission,                                       // 4
        0,                                              // 5
        &data.audioFileID                              // 6
    );
 
   CFRelease (audioFileURL);                               // 7

   UInt32 dataFormatSize = sizeof (desc);    // 1
 
   AudioFileGetProperty (                                  // 2
    data.audioFileID,                                  // 3
    kAudioFilePropertyDataFormat,                       // 4
    &dataFormatSize,                                    // 5
    &desc                                 // 6
   );

	UInt32 size = sizeof(data.totalPackets);
	AudioFileGetProperty(
		data.audioFileID,
		kAudioFileStreamProperty_AudioDataPacketCount,
		&size, &data.totalPackets);


   int ret = AudioQueueNewOutput(
      &desc,
      handle_buffer,
      NULL,
      CFRunLoopGetCurrent(),
      kCFRunLoopCommonModes,
      0,
      &queue);


	UInt32 maxPacketSize;
	UInt32 propertySize = sizeof (maxPacketSize);
	AudioFileGetProperty (                               // 1
		data.audioFileID,                               // 2
		kAudioFilePropertyPacketSizeUpperBound,          // 3
		&propertySize,                                   // 4
		&maxPacketSize                                   // 5
	);

	UInt32 bufferByteSize;

	DeriveBufferSize (                                   // 6
		&desc,                              // 7
		maxPacketSize,                                   // 8
		0.5,                                             // 9
		&bufferByteSize,                          // 10
		&data.numPacketsToRead                        // 11
	);
	
	bool isFormatVBR = (                                       // 1
		desc.mBytesPerPacket == 0 ||
		desc.mFramesPerPacket == 0
	);

	if (isFormatVBR) {                                         // 2
		data.packetDescs =
		(AudioStreamPacketDescription*) malloc (
		data.numPacketsToRead * sizeof (AudioStreamPacketDescription)
		);
	}
	else {                                                   // 3
		data.packetDescs = NULL;
	}	


	UInt32 cookieSize = sizeof (UInt32);                   // 1
	bool couldNotGetProperty =                             // 2
		AudioFileGetPropertyInfo (                         // 3
			data.audioFileID,                             // 4
			kAudioFilePropertyMagicCookieData,             // 5
			&cookieSize,                                   // 6
			NULL                                           // 7
		);
	 
	if (!couldNotGetProperty && cookieSize) {              // 8
		char* magicCookie =
			(char *) malloc (cookieSize);
	 
		AudioFileGetProperty (                             // 9
			data.audioFileID,                             // 10
			kAudioFilePropertyMagicCookieData,             // 11
			&cookieSize,                                   // 12
			magicCookie                                    // 13
		);
	 
		AudioQueueSetProperty (                            // 14
			queue,                                 // 15
			kAudioQueueProperty_MagicCookie,               // 16
			magicCookie,                                   // 17
			cookieSize                                     // 18
		);
	 
		free (magicCookie);                                // 19
	}


   for (int i = 0; i < NUM_BUFFERS; ++i) {
      AudioQueueAllocateBuffer(
         queue,
         bufferByteSize,
         &buffers[i]
      );

    handle_buffer (                                  // 7
        NULL,                                          // 8
        queue,                                    // 9
        buffers[i]                                // 10
    );
   }

   float vol;

   if (data.volume_off) vol = 0;
   else vol = config.getMusicVolume()/255.0;

   AudioQueueSetParameter(
      queue,
      kAudioQueueParam_Volume,
      vol
   );

   ret = AudioQueueStart(
      queue,
      NULL
   );

	do {
#if defined IPHONE || defined ALLEGRO_MACOSX
		POOL_BEGIN
#endif
		CFRunLoopRunInMode(
			kCFRunLoopDefaultMode,
			0.05,
			false
		);
#if defined IPHONE || defined ALLEGRO_MACOSX
		POOL_END
#endif
	} while (!al_get_thread_should_stop(thread));

#if defined IPHONE || defined ALLEGRO_MACOSX
	POOL_END
#endif

   return NULL;
}

void play_aac(const char *filename, bool volume_off)
{
	if (!filename || filename[0] == '\0') {
		if (thread) {
			al_join_thread(thread, NULL);
			AudioQueueDispose(
				queue,
				true
			);
			AudioFileClose(data.audioFileID);
			free(data.filename);
			free(data.packetDescs);
			al_destroy_thread(thread);
		}
		return;
	}
	data.filename = strdup(filename);
	data.volume_off = volume_off;
	data.currentPacket = 0;

	thread = al_create_thread(stream_proc, NULL);
	al_start_thread(thread);
}

void set_aac_volume(float vol)
{
   AudioQueueSetParameter(
      queue,
      kAudioQueueParam_Volume,
      vol
   );
}

static int oldMusicVolume = 255;
static int oldSFXVolume = 255;
static bool muted = false;

static void mute(void)
{
	oldMusicVolume = config.getMusicVolume();
	oldSFXVolume = config.getSFXVolume();

	config.setMusicVolume(0);
	config.setSFXVolume(0);
}

static void unmute(void)
{
	config.setMusicVolume(oldMusicVolume);
	config.setSFXVolume(oldSFXVolume);
}

bool is_muted(void)
{
	return muted;
}

void check_iphone_mute_status(void)
{
#ifdef IPHONE
	CFStringRef route;
	UInt32 propertySize = sizeof(CFStringRef);
	
	AudioSessionGetProperty(
							kAudioSessionProperty_AudioRoute,
							&propertySize,
							&route
							);
	
	bool now_muted = route == NULL;
	
	if (route)
		CFRelease(route);
	
 	if (now_muted != muted) {
		muted = now_muted;
		if (muted) {
			mute();
		}
		else {
			unmute();
		}
	}
#endif
}

/*
static void route_change_callback(
	void *inClientData,
	AudioSessionPropertyID inID,
	UInt32 inDataSize,
	const void *inData
) {
	CFDictionaryRef dict = (CFDictionaryRef)inClientData;

	CFNumberRef n = CFDictionaryGetValue(dict,
		kAudioSession_AudioRouteChangeKey_Reason);
	

}
*/

void init_aac(void)
{
	/* These settings allow ipod music playback simultaneously with
	 * our Allegro music/sfx, and also do not stop the streams when
	 * a phone call comes in. Instead they restart after the call.
	 */

/*
	AudioSessionInitialize(NULL, NULL, NULL, NULL);
	UInt32 sessionCategory = kAudioSessionCategory_AmbientSound;
	//UInt32 sessionCategory = kAudioSessionCategory_MediaPlayback;
	UInt32 mix = TRUE;
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory,
		sizeof(sessionCategory), &sessionCategory);
	AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryMixWithOthers,
		sizeof(mix), &mix);
*/

/*
	AudioSessionAddPropertyListener(
		kAudioSessionProperty_AudioRouteChange,
		route_change_callback,
		NULL
	);
	*/
}

