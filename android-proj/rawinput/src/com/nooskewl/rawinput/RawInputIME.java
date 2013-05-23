package com.nooskewl.rawinput;

import android.inputmethodservice.InputMethodService;
import android.view.KeyEvent;
import android.util.Log;

public class RawInputIME extends InputMethodService
{
	static final int MAX_KEYS = 1000;
	static final long KEY_REPEAT_MILLIS_INITIAL = 500;
	static final long KEY_REPEAT_MILLIS = 100;

	static boolean initialPressed[] = new boolean[MAX_KEYS];
	static long pressTimes[] = new long[MAX_KEYS];

	public RawInputIME()
	{
		for (int i = 0; i < MAX_KEYS; i++) {
			initialPressed[i] = false;
			pressTimes[i] = System.currentTimeMillis()-KEY_REPEAT_MILLIS_INITIAL;
		}
	}

	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if (keyCode < 0 || keyCode >= MAX_KEYS) {
			return true;
		}

		long now = System.currentTimeMillis();

		if (initialPressed[keyCode]) {
			if (pressTimes[keyCode]+KEY_REPEAT_MILLIS > now) {
				return true;
			}
		}
		else {
			if (pressTimes[keyCode]+KEY_REPEAT_MILLIS_INITIAL > now) {
				return true;
			}
			initialPressed[keyCode] = true;
		}
			
		pressTimes[keyCode] = now;

		//Log.d("RawInputIME", "Down: " + keyCode);
		getCurrentInputConnection().sendKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN, keyCode));

		return true;
	}

	public boolean onKeyUp(int keyCode, KeyEvent event)
	{
		if (keyCode < 0 || keyCode >= MAX_KEYS) {
			return true;
		}

		initialPressed[keyCode] = false;
		pressTimes[keyCode] = System.currentTimeMillis()-KEY_REPEAT_MILLIS_INITIAL;

		//Log.d("RawInputIME", "Up: " + keyCode);
		getCurrentInputConnection().sendKeyEvent(new KeyEvent(KeyEvent.ACTION_UP, keyCode));

		return true;
	}
}

