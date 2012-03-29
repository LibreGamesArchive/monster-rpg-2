package org.liballeg.app;

import com.un4seen.bass.BASS;
import com.un4seen.bass.BASSmix;
import android.media.AudioTrack;
import android.media.AudioManager;
import android.media.AudioFormat;
import java.nio.ByteBuffer;
import android.util.Log;
import android.os.Handler;
import java.util.ArrayList;
import java.io.FileInputStream;
import java.io.File;

public class BassPump
{
	private static int mainChannel;
	private static AudioTrack track;
	private static ByteBuffer buffer;
	private static ByteBuffer silence;
	private static ArrayList<Sample> sample_data = new ArrayList<Sample>();
	private static ArrayList<PoolItem> sample_pool = new ArrayList<PoolItem>();
	private static AllegroActivity activity;

	public static void initSound(AllegroActivity activity)
	{
		BassPump.activity = activity;

		BASS.BASS_Init(0, 44100, 0);

		mainChannel = BASSmix.BASS_Mixer_StreamCreate(
			44100,
			2,
			BASS.BASS_STREAM_DECODE
		);

		BASS.BASS_ChannelPlay(mainChannel, false);

		track = new AudioTrack(
			AudioManager.STREAM_MUSIC,
			44100,
			AudioFormat.CHANNEL_CONFIGURATION_STEREO,
			AudioFormat.ENCODING_PCM_16BIT,
			AudioTrack.getMinBufferSize(44100, AudioFormat.CHANNEL_CONFIGURATION_STEREO, AudioFormat.ENCODING_PCM_16BIT),
			AudioTrack.MODE_STREAM);
		
		track.play();

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

	private static int loadSampleWorker(String name, boolean loop)
	{
		int idx = sample_data.size()+1;

		Sample s = new Sample();
		s.loop = loop;

		try {
			AllegroAPKStream in = new AllegroAPKStream(activity, name);
			in.open();
			int size = (int)in.size();
			s.data = ByteBuffer.allocate(size);
			s.data.mark();
			byte[] bytes = new byte[size];
			int read = 0;
			while (read < size) {
				int r = in.read(bytes);
				if (r < 0) break;
				read += r;
			}
			s.data.put(bytes);
			in.close();
		}
		catch (Exception e) {
			Log.e("BassPump", "loadSampleWorker failed for '" + name + "'");
			return 0;
		}

		sample_data.add(s);

		return idx;
	}

	public static int loadSample(String name)
	{
		int idx = loadSampleWorker(name, false);
		return idx;
	}

	public static int loadSampleLoop(String name)
	{
		int idx = loadSampleWorker(name, true);
		return idx;
	}

	private static void playSampleWorker(int s, float vol, float pan, boolean loop, PoolItem pi)
	{
		BASS.BASS_ChannelSetAttribute(s, BASS.BASS_ATTRIB_VOL, vol);
		BASS.BASS_ChannelSetAttribute(s, BASS.BASS_ATTRIB_PAN, pan);
		BASS.BASS_ChannelSetPosition(s, 0, BASS.BASS_POS_BYTE);
		if (loop) {
			BASS.BASS_ChannelSetSync(s, BASS.BASS_SYNC_END, 0, new Sync(), new Object());
		}
		else {
			BASS.BASS_ChannelSetSync(s, BASS.BASS_SYNC_END, 0, new Sync_Remove(), pi);
		}

		BASSmix.BASS_Mixer_StreamAddChannel(mainChannel, s, 0);
	}

	private static boolean playFinished(int idx, float vol, float pan)
	{
		for (int i = 0; i < sample_pool.size(); i++) {
			PoolItem pi = sample_pool.get(i);
			if (pi.done && pi.idx == idx) {
				pi.done = false;
				playSampleWorker(pi.handle, vol, pan, sample_data.get(idx-1).loop, pi);
				return true;
			}
		}

		return false;
	}

	public static void playSampleVolumePan(int idx, float vol, float pan)
	{
		if (playFinished(idx, vol, pan)) {
			return;
		}

		ByteBuffer b = sample_data.get(idx-1).data;
		int s = BASS.BASS_StreamCreateFile(b, 0, b.capacity(), BASS.BASS_STREAM_DECODE);

		PoolItem pi = new PoolItem();
		pi.idx = idx;
		pi.handle = s;
		pi.done = false;
		sample_pool.add(pi);

		playSampleWorker(s, vol, pan, sample_data.get(idx-1).loop, pi);
	}

	public static void playSampleVolume(int idx, float vol)
	{
		playSampleVolumePan(idx, vol, 0.0f);
	}

	public static void playSample(int idx)
	{
		playSampleVolumePan(idx, 1.0f, 0.0f);
	}

	public static void stopSample(int s)
	{
		BASS.BASS_ChannelStop(s);
	}

	public static int loadMusic(String name)
	{
		AllegroAPKStream apk_stream = new AllegroAPKStream(activity, name);
		apk_stream.open();
		int m = BASS.BASS_StreamCreateFileUser(
			BASS.STREAMFILE_NOBUFFER,
			BASS.BASS_STREAM_DECODE,
			new UserFileProcs(),
			apk_stream
		);
		BASS.BASS_ChannelSetSync(m, BASS.BASS_SYNC_END, 0, new Sync(), new Object());
		return m;
	}

	public static void playMusic(int music)
	{
		BASS.BASS_ChannelSetPosition(music, 0, BASS.BASS_POS_BYTE);
		BASSmix.BASS_Mixer_StreamAddChannel(mainChannel, music, 0);
	}

	public static void stopMusic(int music)
	{
		BASSmix.BASS_Mixer_ChannelRemove(music);
	}

	public static void destroyMusic(int music)
	{
		BASS.BASS_StreamFree(music);
	}

	public static void shutdownBASS()
	{
		BASS.BASS_Free();
	}

	public static void destroySample(int s)
	{
		BASS.BASS_SampleFree(s);
	}
}

class Sync implements BASS.SYNCPROC
{
	public void SYNCPROC(int handle, int channel, int data, Object user)
	{
		BASS.BASS_ChannelSetPosition(channel, 0, BASS.BASS_POS_BYTE);
	}
}

class Sync_Remove implements BASS.SYNCPROC
{
	public void SYNCPROC(int handle, int channel, int data, Object user)
	{
		PoolItem pi = (PoolItem)user;
		BASSmix.BASS_Mixer_ChannelRemove(channel);
		pi.done = true;
	}
}

class Sample
{
	public ByteBuffer data;
	public boolean loop;
}

class PoolItem
{
	int idx;
	public int handle;
	boolean done;
}

class UserFileProcs implements BASS.BASS_FILEPROCS
{
	public void FILECLOSEPROC(Object user)
	{
		AllegroAPKStream a = (AllegroAPKStream)user;
		a.close();
	}

	public long FILELENPROC(Object user)
	{
		AllegroAPKStream a = (AllegroAPKStream)user;
		return a.size();
	}

	public int FILEREADPROC(ByteBuffer buffer, int length, Object user)
	{
		AllegroAPKStream a = (AllegroAPKStream)user;
		byte[] b = new byte[length];
		long res = a.read(b);
		buffer.put(b);
		return (int)res;
	}

	public boolean FILESEEKPROC(long offset, Object user)
	{
		AllegroAPKStream a = (AllegroAPKStream)user;
		return a.seek(offset);
	}
}
