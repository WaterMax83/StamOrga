package org.qtproject.example;
import org.qtproject.example.R;

import android.content.Context;
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

import com.google.firebase.messaging.FirebaseMessaging;

public class MainActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static final String TAG = "MainActivity";

    private BroadcastReceiver mRegistrationBroadcastReceiver;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        String token1 = sharedPreferences.getString(QuickstartPreferences.FCM_TOKEN, "");
        Log.i(TAG, "Token beim Start " + token1);

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

    public static void SubscribeToTopic(String topic) {
        FirebaseMessaging.getInstance().subscribeToTopic(topic);
    }

    public static void UnRegisterFromTopic(String topic) {
        FirebaseMessaging.getInstance().unsubscribeFromTopic(topic);
    }
}
