package com.nooskewl.monsterrpg2;

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
import tv.ouya.console.api.*;
import org.json.*;
import java.security.*;
import java.io.*;
import javax.crypto.*;
import javax.crypto.spec.*;
import android.util.*;
import java.util.*;
import java.security.spec.*;
import android.app.Activity;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.content.IntentFilter;

public class MO2Activity extends AllegroActivity {

	/* load libs */
	static {
		System.loadLibrary("allegro_monolith");
		System.loadLibrary("bass");
		System.loadLibrary("monsterrpg2");
	}

	MyBroadcastReceiver bcr;

	public MO2Activity()
	{
		super("libmonsterrpg2.so");
	}

	public void logString(String s)
	{
		Log.d("MoRPG2", s);
	}

	static int purchased = -1;

	public void requestPurchase(final Product product) {
		Purchasable purchasable = new Purchasable(product.getIdentifier());

		OuyaResponseListener<PurchaseResult> purchaseListener =	new OuyaResponseListener<PurchaseResult>() {
			@Override
			public void onCancel() {
				purchased = 0;
			}
			@Override
			public void onSuccess(PurchaseResult result) {
				if (result.getProductIdentifier().equals("MONSTER_RPG_2")) {
					Log.d("MonsterRPG2", "Congrats on your purchase");
					writeReceipt();
					purchased = 1;
				}
				else {
					purchased = 0;
					Log.e("MonsterRPG2", "Your purchase failed.");
				}
			}
			@Override
			public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
				purchased = 0;
				Log.d("MonsterRPG2", errorMessage);
			}
		};

		OuyaFacade.getInstance().requestPurchase(this, purchasable, purchaseListener);
	}

	static void writeReceipt()
	{
		OuyaFacade.getInstance().putGameData("MonsterRPG2", "PURCHASED");
	}

	public void doIAP()
	{
		purchased = -1;

		if (OuyaFacade.getInstance().isRunningOnOUYASupportedHardware()) {
			// This is the set of product IDs which our app knows about
			List<Purchasable> PRODUCT_ID_LIST =
				Arrays.asList(new Purchasable("MONSTER_RPG_2"));

			OuyaResponseListener<List<Product>> productListListener =
				new OuyaResponseListener<List<Product>>() {
					@Override
					public void onCancel() {
						purchased = 0;
					}
					@Override
					public void onSuccess(List<Product> products) {
						if (products.size() == 0) {
							purchased = 0;
						}
						else {
							for (Product p : products) {
								Log.d("MonsterRPG2", p.getName() + " costs " + p.getPriceInCents());
								try {
									if (p.getIdentifier().equals("MONSTER_RPG_2")) {
										requestPurchase(p);
									}
								} catch (Exception e) {
									purchased = 0;
									Log.e("MonsterRPG2", "requestPurcase failure", e);
								}
							}
						}
					}

					@Override
					public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
						purchased = 0;
						Log.d("MonsterRPG2", errorMessage);
					}
			};

			OuyaFacade.getInstance().requestProductList(this, PRODUCT_ID_LIST, productListListener);
		}
		else {
			purchased = 0;
		}
	}

	public int isPurchased()
	{
		return purchased;
	}

	public void queryPurchased()
	{
		purchased = -1;

		// The receipt listener now receives a collection of tv.ouya.console.api.Receipt objects.
		OuyaResponseListener<Collection<Receipt>> receiptListListener =
			new OuyaResponseListener<Collection<Receipt>>() {
				@Override
				public void onSuccess(Collection<Receipt> receipts) {
					for (Receipt r : receipts) {
						Log.d("MonsterRPG2", r.getIdentifier() + " purchased for " + r.getFormattedPrice());
						if (r.getIdentifier().equals("MONSTER_RPG_2")) {
							purchased = 1;
						}
					}
					if (purchased == -1) {
						purchased = 0;
					}
				}

				@Override
				public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
					Log.d("MonsterRPG2", errorMessage);
					purchased = 0;
				}

				@Override
				public void onCancel() {
					Log.d("MonsterRPG2", "Cancelled checking receipts");
					purchased = 0;
				}
			};

		try {
			if (OuyaFacade.getInstance().getGameData("MonsterRPG2").equals("PURCHASED")) {
				purchased = 1;
			}
			else {
				purchased = 0;
			}
		}
		catch (Exception e) {
			if (OuyaFacade.getInstance().isRunningOnOUYASupportedHardware()) {
				OuyaFacade.getInstance().requestReceipts(this, receiptListListener);
			}
			else {
				purchased = 0;
			}
		}
	}

	public static final String DEVELOPER_ID = "FIXME";

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		Bundle developerInfo = new Bundle();

		// Your developer id can be found in the Developer Portal
		developerInfo.putString(OuyaFacade.OUYA_DEVELOPER_ID, DEVELOPER_ID);

		// There are a variety of ways to store and access your application key.
		// Two of them are demoed in the samples 'game-sample' and 'iap-sample-app'
		developerInfo.putByteArray(OuyaFacade.OUYA_DEVELOPER_PUBLIC_KEY, loadApplicationKey());

		OuyaFacade.getInstance().init(this, developerInfo);
		super.onCreate(savedInstanceState);

		bcr = new MyBroadcastReceiver();
	}

	@Override
	public void onDestroy() {
		OuyaFacade.getInstance().shutdown();
		super.onDestroy();
	}

	@Override
	public void onResume() {
		super.onResume();

		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STARTED"));
		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STOPPED"));
	}

	@Override
	public void onPause() {
		super.onPause();

		unregisterReceiver(bcr);
	}

	byte[] loadApplicationKey() {
		// Create a PublicKey object from the key data downloaded from the developer portal.
		try {
			// Read in the key.der file (downloaded from the developer portal)
			InputStream inputStream = getResources().openRawResource(R.raw.key);
			byte[] applicationKey = new byte[inputStream.available()];
			inputStream.read(applicationKey);
			inputStream.close();
			return applicationKey;
		} catch (Exception e) {
			Log.e("MonsterRPG2", "Unable to load application key", e);
		}

		return null;
	}

	@Override
	public void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		if (null != OuyaFacade.getInstance())
		{
			if (OuyaFacade.getInstance().processActivityResult(requestCode, resultCode, data)) {
				// handled activity result
			} else {
				// unhandled activity result
			}
		} else {
		// OuyaFacade not initialized
		}
	}
}

