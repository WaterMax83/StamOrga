package org.qtproject.example;
import org.qtproject.example.R;

import android.content.Context;
import android.os.Vibrator;
import android.app.Activity;
import android.os.Bundle;

import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.firebase.messaging.FirebaseMessaging;

public class Vibrate extends org.qtproject.qt5.android.bindings.QtActivity
{
    // start FCM
//    private static final int PLAY_SERVICES_RESOLUTION_REQUEST = 9000;
    private static final String TAG = "MainActivity";

    private BroadcastReceiver mRegistrationBroadcastReceiver;
    //private KeyStore m_keyStore;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        String token1 = sharedPreferences.getString(QuickstartPreferences.FCM_TOKEN, "");
        Log.i(TAG, "TOken beim Start " + token1);

        FirebaseMessaging.getInstance().subscribeToTopic("TestProject");

        mRegistrationBroadcastReceiver = new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent)
            {
                SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context);
                String token = sharedPreferences.getString(QuickstartPreferences.FCM_TOKEN, "");
                JavaNatives.sendFCMToken(token);
            }
        };
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        LocalBroadcastManager.getInstance(this).registerReceiver(mRegistrationBroadcastReceiver,
                                            new IntentFilter(QuickstartPreferences.FCM_TOKEN));
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    @Override
    protected void onPause()
    {
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mRegistrationBroadcastReceiver);
        super.onPause();
    }

    // start vibrate
    public static Vibrator m_vibrator;
    public static Vibrate m_istance;
    public Vibrate()
    {
        m_istance = this;
    }
    public static void start(int x)
    {
        if (m_vibrator == null)
        {
            if (m_istance != null)
            {
                m_vibrator = (Vibrator) m_istance.getSystemService(Context.VIBRATOR_SERVICE);
                m_vibrator.vibrate(x);
            }
        }
        else m_vibrator.vibrate(x);
    }
    // end vibrate

}
