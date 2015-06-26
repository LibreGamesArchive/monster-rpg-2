package ca.nooskewl.monsterrpg2;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;

public class MyBroadcastReceiver extends BroadcastReceiver
{
	native void pauseSound();
	native void resumeSound();

	public void onReceive(Context context, Intent intent)
	{
		if (intent.getAction() == "android.intent.action.DREAMING_STARTED") {
			pauseSound();
		}
		else if (intent.getAction() == "android.intent.action.DREAMING_STOPPED") {
			resumeSound();
		}
	}
}


