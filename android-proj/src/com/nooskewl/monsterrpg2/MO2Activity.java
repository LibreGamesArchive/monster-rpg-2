package com.nooskewl.monsterrpg2;

import org.liballeg.app.AllegroActivity;
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

public class MO2Activity extends AllegroActivity {

   /* load libs */
   static {
      System.loadLibrary("allegro");
      System.loadLibrary("allegro_memfile");
      System.loadLibrary("allegro_primitives");
      System.loadLibrary("allegro_image");
      System.loadLibrary("allegro_font");
      System.loadLibrary("allegro_ttf");
      System.loadLibrary("allegro_color");
      System.loadLibrary("bass");
      System.loadLibrary("bassflac");
   }

	public void logString(String s)
	{
		Log.d("MRPG2", s);
	}

	public String getSDCardPrivateDir()
	{
		File f = getExternalFilesDir(null);
		if (f != null) {
			return f.getAbsolutePath();
		}
		else {
			return getFilesDir().getAbsolutePath();
		}
	}

	public boolean wifiConnected()
	{
		ConnectivityManager connManager = (ConnectivityManager)getSystemService(CONNECTIVITY_SERVICE);
		NetworkInfo mWifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

		return mWifi.isConnected();
	}

	public void openURL(String url)
	{
		Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.monster-rpg.com"));
		startActivity(intent);
	}

	private boolean clip_thread_done = false;

	public void setClipData(String saveState)
	{
		final String ss = saveState;

		Runnable runnable = new Runnable() {
			public void run() {
				ClipboardManager m = 
					(ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);

				m.setText(ss);

				clip_thread_done = true;
				}
			};
		runOnUiThread(runnable);

		while (!clip_thread_done)
			;
		clip_thread_done = false;
	}

	private String clipdata;

	public String getClipData()
	{
		Runnable runnable = new Runnable() {
			public void run() {
				ClipboardManager m = 
					(ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);

				clipdata = m.getText().toString();

				clip_thread_done = true;
			}
		};
		runOnUiThread(runnable);

		while (!clip_thread_done)
			;
		clip_thread_done = false;

		return clipdata;
	}

	static String keyS = "base64 encoded key goes here";

	static HashMap<String, Product> mOutstandingPurchaseRequests = new HashMap<String, Product>();
	static PublicKey mPublicKey;

	static int purchased = -1;

	public void requestPurchase(final Product product)
		throws GeneralSecurityException, UnsupportedEncodingException, JSONException {
		SecureRandom sr = SecureRandom.getInstance("SHA1PRNG");

		// This is an ID that allows you to associate a successful purchase with
		// it's original request. The server does nothing with this string except
		// pass it back to you, so it only needs to be unique within this instance
		// of your app to allow you to pair responses with requests.
		String uniqueId = Long.toHexString(sr.nextLong());

		JSONObject purchaseRequest = new JSONObject();
		purchaseRequest.put("uuid", uniqueId);
		purchaseRequest.put("identifier", product.getIdentifier());
		// This value is only needed for testing, not setting it results in a live purchase
		purchaseRequest.put("testing", "true"); 
		String purchaseRequestJson = purchaseRequest.toString();

		byte[] keyBytes = new byte[16];
		sr.nextBytes(keyBytes);
		SecretKey key = new SecretKeySpec(keyBytes, "AES");

		byte[] ivBytes = new byte[16];
		sr.nextBytes(ivBytes);
		IvParameterSpec iv = new IvParameterSpec(ivBytes);

		Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding", "BC");
		cipher.init(Cipher.ENCRYPT_MODE, key, iv);
		byte[] payload = cipher.doFinal(purchaseRequestJson.getBytes("UTF-8"));

		cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding", "BC");
		cipher.init(Cipher.ENCRYPT_MODE, mPublicKey);
		byte[] encryptedKey = cipher.doFinal(keyBytes);

		Purchasable purchasable =
			new Purchasable(
				product.getIdentifier(),
				Base64.encodeToString(encryptedKey, Base64.NO_WRAP),
				Base64.encodeToString(ivBytes, Base64.NO_WRAP),
				Base64.encodeToString(payload, Base64.NO_WRAP) );
	
		synchronized (mOutstandingPurchaseRequests) {
		    mOutstandingPurchaseRequests.put(uniqueId, product);
		}
		OuyaResponseListener<String> purchaseListener =
		new OuyaResponseListener<String>() {
		    @Override
		    public void onCancel() {
		    	purchased = 0;
		    }
		    @Override
		    public void onSuccess(String result) {
		    	purchased = 0;
			try {
			    OuyaEncryptionHelper helper = new OuyaEncryptionHelper();

			    JSONObject response = new JSONObject(result);

			    String id = helper.decryptPurchaseResponse(response, mPublicKey);
			    Product storedProduct;
			    synchronized (mOutstandingPurchaseRequests) {
				storedProduct = mOutstandingPurchaseRequests.remove(id);
			    }
			    if(storedProduct == null) {
				onFailure(
				    OuyaErrorCodes.THROW_DURING_ON_SUCCESS, 
				    "No purchase outstanding for the given purchase request",
				    Bundle.EMPTY);
				return;
			    }

			    Log.d("Purchase", "Congrats you bought: " + storedProduct.getName());

			    if (storedProduct.getName().equals("Monster RPG 2")) {
			    	purchased = 1;
			    }
			} catch (Exception e) {
			    Log.e("Purchase", "Your purchase failed.", e);
			}
		    }

		    @Override
		    public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
		    	purchased = 0;
			Log.d("Error", errorMessage);
		    }
		};
		OuyaFacade.getInstance().requestPurchase(purchasable, purchaseListener);
	}

	public void doIAP()
	{
		purchased = -1;
		if (OuyaFacade.getInstance().isRunningOnOUYAHardware()) {
			List<Purchasable> PRODUCT_ID_LIST =
				Arrays.asList(new Purchasable("MONSTER_RPG_2"));
			OuyaResponseListener<ArrayList<Product>> productListListener =
				new OuyaResponseListener<ArrayList<Product>>() {
					@Override
					public void onCancel() {
						purchased = 0;
					}
					@Override
					public void onSuccess(ArrayList<Product> products) {
						if (products.size() == 0) {
							purchased = 0;
						}
						else {
							for (Product p : products) {
								Log.d("Product", p.getName() + " costs " + p.getPriceInCents());
								try {
									requestPurchase(p);
								} catch (Exception e) {
									Log.e("ERROR", "requestPurcase failure", e);
								}
							}
						}
					}

					@Override
					public void onFailure(int errorCode, String errorMessage, Bundle errorBundle) {
						purchased = 0;
						Log.d("Error", errorMessage);
					}
			};
			OuyaFacade.getInstance().requestProductList(PRODUCT_ID_LIST, productListListener);
		}
		else {
			purchased = 0;
		}

	}

	public int isPurchased()
	{
		//Log.d("Purchased?", "" + purchased);
		return purchased;
	}

	public void queryPurchased()
	{
		purchased = -1;
		OuyaResponseListener<String> receiptListListener =
		new OuyaResponseListener<String>() {
		    @Override
		    public void onCancel() {
		    	purchased = 0;
		    }
		    @Override
		    public void onSuccess(String receiptResponse) {
			OuyaEncryptionHelper helper = new OuyaEncryptionHelper();
			List<Receipt> receipts = null;
			try {
			    JSONObject response = new JSONObject(receiptResponse);
			    receipts = helper.decryptReceiptResponse(response, mPublicKey);
			} catch (Exception e) {
		    	    purchased = 0;
			    throw new RuntimeException(e);
			}
			Log.d("Receipt", "Listing purchases:");
			for (Receipt r : receipts) {
			    Log.d("Receipt", "You have purchased: " + r.getIdentifier());
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
		    	purchased = 0;
			Log.d("Error", errorMessage);
		    }
		};
		OuyaFacade.getInstance().requestReceipts(receiptListListener);
	}

	public static final String DEVELOPER_ID = "developer id goes here";

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		OuyaFacade.getInstance().init(this, DEVELOPER_ID);
		try {
			X509EncodedKeySpec keySpec = new X509EncodedKeySpec(Base64.decode(keyS, Base64.DEFAULT));
			KeyFactory keyFactory = KeyFactory.getInstance("RSA");
			mPublicKey = keyFactory.generatePublic(keySpec);
		} catch (Exception e) {
			Log.e("ERROR", "Unable to create encryption key", e);
		}
		super.onCreate(savedInstanceState);
	}

	@Override
	public void onDestroy() {
		OuyaFacade.getInstance().shutdown();
		super.onDestroy();
	}
}

