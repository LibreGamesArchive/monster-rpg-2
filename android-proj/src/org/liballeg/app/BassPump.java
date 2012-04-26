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
	private static ArrayList<Music> mps = new ArrayList<Music>();
	private static int music_ids = 1;

	public static void initSound(AllegroActivity activity)
	{
		BassPump.activity = activity;

		sp = new SoundPool(4, AudioManager.STREAM_MUSIC, 0);
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
		}
		catch (IOException e) {
			Log.e("BassPump", "ERROR LOADING MUSIC");
			return 0;
		}

		Music m = new Music();
		m.id = music_ids++;
		m.mp = mp;
		mps.add(m);

		return m.id;
	}

	public static void playMusic(int music)
	{
		int idx = findMusic(music);
		MediaPlayer mp = mps.get(idx).mp;
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
		int idx = findMusic(music);
		mps.get(idx).mp.stop();
	}

	public static void destroyMusic(int music)
	{
		int idx = findMusic(music);
		mps.get(idx).mp.stop();
		mps.get(idx).mp.release();
		mps.get(idx).mp = null;
		mps.remove(idx);
	}

	public static void setMusicVolume(int music, float vol)
	{
		int idx = findMusic(music);
		mps.get(idx).mp.setVolume(vol, vol);
	}

	public static void shutdownBASS()
	{
		sp.release();
		sp = null;
	}

	private static int findMusic(int id)
	{
		for (int i = 0; i < mps.size(); i++) {
			if (mps.get(i).id == id) {
				return i;
			}
		}

		Log.d("BassPump", "Couldn't find music " + id);
		return -1;
	}
}

class Sample
{
	int id;
	boolean loop;
	int streamID;
}

class Music
{
	int id;
	MediaPlayer mp;
}
