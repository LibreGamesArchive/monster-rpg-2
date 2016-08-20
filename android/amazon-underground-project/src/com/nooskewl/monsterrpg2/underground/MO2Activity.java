package com.nooskewl.monsterrpg2.underground;

import org.liballeg.android.AllegroActivity;

import android.net.Uri;
import android.content.Intent;
import android.text.ClipboardManager;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import java.io.File;
import android.view.KeyEvent;
import android.util.Log;
import android.app.ActivityManager;
import android.os.Bundle;
import java.io.*;
import android.util.*;
import java.util.*;
import java.security.spec.*;
import android.app.Activity;
import android.view.View;
import android.content.IntentFilter;
import com.amazon.ags.api.*;
import com.amazon.ags.api.achievements.*;
import com.amazon.ags.api.overlay.PopUpLocation;

public class MO2Activity extends AllegroActivity {
	/* load libs */
	static {
		System.loadLibrary("allegro_monolith");
		System.loadLibrary("bass");
		System.loadLibrary("monsterrpg2");
	}

	//reference to the agsClient
	AmazonGamesClient agsClient;

	AmazonGamesCallback callback = new AmazonGamesCallback() {
		@Override
		public void onServiceNotReady(AmazonGamesStatus status) {
			//unable to use service
			Log.d("MoRPG2", "GameCircle not initialized: " + status.toString());
		}
		@Override
		public void onServiceReady(AmazonGamesClient amazonGamesClient) {
			Log.d("MoRPG2", "GameCircle initialized!");
			agsClient = amazonGamesClient;
			//ready to use GameCircle
			agsClient.setPopUpLocation(PopUpLocation.TOP_CENTER);
		}
	};

	//list of features your game uses (in this example, achievements and leaderboards)
	EnumSet<AmazonGamesFeature> myGameFeatures = EnumSet.of(AmazonGamesFeature.Achievements);

	public MO2Activity()
	{
		super("libmonsterrpg2.so");
	}

	public void logString(String s)
	{
		Log.d("MoRPG2", s);
	}

	MyBroadcastReceiver bcr;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		bcr = new MyBroadcastReceiver();
	}
	
	public void onPause() {
		super.onPause();

		unregisterReceiver(bcr);
	}
	
	public void onResume() {
		super.onResume();

		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STARTED"));
		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STOPPED"));

		AmazonGamesClient.initialize(this, callback, myGameFeatures);
	}
	
	public boolean gamepadAlwaysConnected()
	{
		return getPackageManager().hasSystemFeature("android.hardware.touchscreen") == false;
	}

	public void unlock_achievement(String id) {
		if (agsClient != null) {
			AchievementsClient acClient = agsClient.getAchievementsClient();
			if (acClient != null) {
				AGResponseHandle<UpdateProgressResponse> handle = acClient.updateProgress(id, 100.0f);
			}
		}
	}

	public void show_achievements()
	{
		if (agsClient != null) {
			AchievementsClient acClient = agsClient.getAchievementsClient();
			if (acClient != null) {
				acClient.showAchievementsOverlay();
			}
		}
	}
}

