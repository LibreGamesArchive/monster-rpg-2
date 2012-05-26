package com.nooskewl.monsterrpg2;

import android.media.AudioTrack;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.util.Log;
import java.util.ArrayList;
import android.media.SoundPool;
import android.content.res.AssetFileDescriptor;
import java.io.IOException;
import android.media.MediaPlayer;
import com.un4seen.bass.BASS;
import com.un4seen.bass.BASSFLAC;
import com.un4seen.bass.BASSmix;
import java.nio.ByteBuffer;

public class OldAndroid
{
	private static int mainChannel;
	private static AudioTrack track;
	private static ByteBuffer buffer;
	private static ByteBuffer silence;
	private static AllegroActivity activity;
	private static SoundPool sp;
	private static ArrayList<Sample> samples = new ArrayList<Sample>();
	private static ArrayList<Music> mps = new ArrayList<Music>();
	private static int music_ids = 1;
	private static int numFlacsPlaying = 0;

	public static void initSound(AllegroActivity activity)
	{
		OldAndroid.activity = activity;

		sp = new SoundPool(8, AudioManager.STREAM_MUSIC, 0);
	
		BASS.BASS_Init(0, 44100, 0);

		mainChannel = BASSmix.BASS_Mixer_StreamCreate(
			44100,
			2,
			BASS.BASS_STREAM_DECODE
		);

		BASS.BASS_ChannelPlay(mainChannel, false);

		buffer = ByteBuffer.allocate(4096);

		silence = ByteBuffer.allocate(4096);
		byte[] silence_bytes = new byte[4096];
		for (int i = 0; i < silence.capacity(); i++) {
			silence_bytes[i] = 0;
		}
		silence.put(silence_bytes);
	}

	public static void update()
	{
		if (numFlacsPlaying <= 0)
			return;

		int res = BASS.BASS_ChannelGetData(mainChannel, buffer, buffer.capacity());
		ByteBuffer buf;

		if (res <= 0) {
			buf = silence;
			res = silence.capacity();
		}
		else {
			buf = buffer;
		}

		track.write(buf.array(), 0, res);
	}

	public static int loadSample(String name)
	{
		int id = sp.load(name, 1);
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
		Music m = new Music();
		m.id = music_ids++;

		if (name.substring(name.lastIndexOf(".")).equals(".flac")) {
			int music = BASSFLAC.BASS_FLAC_StreamCreateFile(name, 0, 0, BASS.BASS_SAMPLE_LOOP | BASS.BASS_STREAM_DECODE);
			if (music == 0) {
				Log.e("OldAndroid", "Error loading '" + name + "'");
				music_ids--;
				m = null;
				return 0;
			}
			BASS.BASS_ChannelSetSync(music, BASS.BASS_SYNC_END, 0, new Sync(), new Object());
			m.mp = null;
			m.hmusic = music;
		}
		else {
			MediaPlayer mp = new MediaPlayer();

			AssetFileDescriptor fd;
			try {
				Log.d("OldAndroid", "Loading with MediaPlayer: '" + name + "'");
				fd = activity.getResources().getAssets().openFd(name);
				mp.setDataSource(fd.getFileDescriptor(), fd.getStartOffset(), fd.getLength());
			}
			catch (IOException e) {
				Log.e("OldAndroid", "ERROR LOADING MUSIC");
				return 0;
			}
			m.mp = mp;
		}

		mps.add(m);
		return m.id;
	}

	private static void startAudioTrack()
	{
		track = new AudioTrack(
			AudioManager.STREAM_MUSIC,
			44100,
			AudioFormat.CHANNEL_CONFIGURATION_STEREO,
			AudioFormat.ENCODING_PCM_16BIT,
			AudioTrack.getMinBufferSize(44100, AudioFormat.CHANNEL_CONFIGURATION_STEREO, AudioFormat.ENCODING_PCM_16BIT),
			AudioTrack.MODE_STREAM);
		
		track.play();
	}

	public static void playMusic(int music)
	{
		int idx = findMusic(music);
		MediaPlayer mp = mps.get(idx).mp;
		if (mp == null) {
			if (numFlacsPlaying <= 0) {
				startAudioTrack();
				numFlacsPlaying = 1;
			}
			else {
				numFlacsPlaying++;
			}
			int hmusic = mps.get(idx).hmusic;
			BASS.BASS_ChannelSetPosition(hmusic, 0, BASS.BASS_POS_BYTE);
			BASSmix.BASS_Mixer_StreamAddChannel(mainChannel, hmusic, 0);
			BASS.BASS_ChannelPlay(mainChannel, true);
			BASS.BASS_ChannelPlay(hmusic, true);
			Log.d("OldAndroid", "Add music, numFlacsPlaying=" + numFlacsPlaying);
		}
		else {
			try {
				mp.prepare();
			}
			catch (IOException e) {
				Log.e("OldAndroid", "PLAY FAILED");
				return;
			}
			mp.start();
			mp.setLooping(true);
		}
	}

	public static void stopMusic(int music)
	{
		int idx = findMusic(music);
		MediaPlayer mp = mps.get(idx).mp;
		if (mp == null) {
			BASS.BASS_ChannelPlay(mps.get(idx).hmusic, false);
			BASSmix.BASS_Mixer_ChannelRemove(mps.get(idx).hmusic);
		}
		else {
			mp.stop();
		}
	}

	public static void destroyMusic(int music)
	{
		int idx = findMusic(music);
		Music m = mps.get(idx);

		if (m.mp == null) {
			BASS.BASS_StreamFree(m.hmusic);
			numFlacsPlaying--;
			if (numFlacsPlaying <= 0) {
				BASS.BASS_ChannelPlay(mainChannel, false);
				track.stop();
				track.release();
				track = null;
			}
		}
		else {
			m.mp.stop();
			m.mp.release();
			m.mp = null;
		}

		mps.remove(idx);
	}

	public static void setMusicVolume(int music, float vol)
	{
		int idx = findMusic(music);
		Music m = mps.get(idx);
		if (m.mp == null) {
			BASS.BASS_ChannelSetAttribute(m.hmusic, BASS.BASS_ATTRIB_VOL, vol);
		}
		else {
			m.mp.setVolume(vol, vol);
		}
	}

	public static void shutdownBASS()
	{
		BASS.BASS_Free();
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

		Log.d("OldAndroid", "Couldn't find music " + id);
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
	int hmusic;
}

class Sync implements BASS.SYNCPROC
{
	public void SYNCPROC(int handle, int channel, int data, Object user)
	{
		BASS.BASS_ChannelSetPosition(channel, 0, BASS.BASS_POS_BYTE);
	}
}

