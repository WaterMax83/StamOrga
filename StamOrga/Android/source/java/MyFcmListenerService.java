package org.qtproject.example;
import org.qtproject.example.R;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import java.util.Map;
import java.lang.System;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

public class MyFcmListenerService extends FirebaseMessagingService
{
    private static final String TAG = "MyFcmListenerService";

    /**
     * Called when message is received.
     */
    @Override
    public void onMessageReceived(RemoteMessage message){
        String from = message.getFrom();
        Map<String, String> data = message.getData();
        String title = "";
        String body = "";

//        for (Map.Entry<String, String> entry : data.entrySet())
//        {
//            Log.d(TAG, entry.getKey() + "/" + entry.getValue());
//        }
        if (data.containsKey("title"))
            title = data.get("title").toString();
        if (data.containsKey("body"))
            body = data.get("body").toString();

        Log.d(TAG, "From: " + from);
        Log.d(TAG, "Size: " + data.size());

        if (from.startsWith("/topics/")) {
            // message received from some topic.
//            long time = System.currentTimeMillis();

            String userIndex = "unknown";
            if (data.containsKey("u_id"))
                userIndex = data.get("u_id").toString();

            SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
            String savedUserIndex = sharedPreferences.getString(QuickstartPreferences.FCM_TOPIC_USER_INDEX, "");

            /* Check if this topic was started from this user */
            if (!userIndex.equals("-1") && userIndex.equals(savedUserIndex)) {
                Log.d(TAG, "Did not send notification because userIndex is from this instance: " + userIndex);
                return;
            }
        }

        sendNotification(title, body);
    }

    private void sendNotification(String title, String message)
    {
        Intent intent = new Intent(this, MainActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0 /* Request code */, intent, PendingIntent.FLAG_ONE_SHOT);

        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this)
                .setSmallIcon(R.drawable.icon)
                .setContentTitle(title)
                .setContentText(message)
                .setAutoCancel(true)
                .setContentIntent(pendingIntent);

        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

        notificationManager.notify(0 /* ID of notification */, notificationBuilder.build());
    }
}
