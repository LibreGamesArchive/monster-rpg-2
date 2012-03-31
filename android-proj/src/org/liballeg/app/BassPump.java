package org.liballeg.app;

import android.media.AudioManager;
import android.util.Log;
import java.util.ArrayList;
import android.media.SoundPool;
import android.content.res.AssetFileDescriptor;
import java.io.IOException;
import android.media.MediaPlayer;

public class BassPump
{
	private static AllegroActivity activity;
	private static SoundPool sp;
	private static ArrayList<Sample> samples = new ArrayList<Sample>();
	private static ArrayList<MediaPlayer> mps = new ArrayList<MediaPlayer>();

	public static void initSound(AllegroActivity activity)
	{
		BassPump.activity = activity;

		sp = new SoundPool(32, AudioManager.STREAM_MUSIC, 0);
	}

	public static void update()
	{
	}

	public static int loadSample(String name)
	{
		AssetFileDescriptor fd;
		try {
			fd = activity.getResources().getAssets().openFd(name);
		}
		catch (IOException e) {
			Log.d("BassPump", "IOException in loadSample");
			return 0;
		}
		int id = sp.load(fd, 1);
		Log.d("BassPump", "id=" + id + " in loadSample");
		if (id <= 0) {
			return 0;
		}
		int idx = samples.size()+1;
		Sample s = new Sample();
		s.id = id;
		s.loop = false;
		samples.add(s);
		return idx;
	}

	public static int loadSampleLoop(String name)
	{
		int idx = loadSample(name);
		if (idx > 0) {
			samples.get(idx-1).loop = true;
		}
		return idx;
	}

	public static void playSampleVolumePan(int idx, float vol, float pan)
	{
		int streamID = sp.play(
			samples.get(idx-1).id,
			vol, vol, 1, samples.get(idx-1).loop ? -1 : 0, 1.0f);
		samples.get(idx-1).streamID = streamID;
	}

	public static void playSampleVolume(int idx, float vol)
	{
		playSampleVolumePan(idx, vol, 0.0f);
	}

	public static void playSample(int idx)
	{
		playSampleVolumePan(idx, 1.0f, 0.0f);
	}

	public static void stopSample(int idx)
	{
		sp.stop(samples.get(idx-1).streamID);
	}

	public static void destroySample(int idx)
	{
		sp.unload(samples.get(idx-1).id);
	}

	public static int loadMusic(String name)
	{
		MediaPlayer mp = new MediaPlayer();

		AssetFileDescriptor fd;
		try {
			fd = activity.getResources().getAssets().openFd(name);
			mp.setDataSource(fd.getFileDescriptor(), fd.getStartOffset(), fd.getLength());
			//mp.setDataSource(fd.getFileDescriptor());
		}
		catch (IOException e) {
			Log.e("BassPump", "ERROR LOADING MUSIC");
			return 0;
		}

		mps.add(mp);
		return mps.size();
	}

	public static void playMusic(int music)
	{
		MediaPlayer mp = mps.get(music-1);
		try {
			mp.prepare();
		}
		catch (IOException e) {
			Log.e("BassPump", "PLAY FAILED");
			return;
		}
		mp.start();
		mp.setLooping(true);
	}

	public static void stopMusic(int music)
	{
		MediaPlayer mp = mps.get(music-1);
		mp.stop();
	}

	public static void destroyMusic(int music)
	{
		stopMusic(music);
		mps.set(music-1, null);
	}

	public static void shutdownBASS()
	{
		sp.release();
		sp = null;
	}
}

class Sample
{
	int id;
	boolean loop;
	int streamID;
}
