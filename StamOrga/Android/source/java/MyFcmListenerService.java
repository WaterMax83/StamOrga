package org.qtproject.example;
import org.qtproject.example.R;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import java.util.Map;

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
        String msg = "";

        for (Map.Entry<String, String> entry : data.entrySet())
        {
            Log.d(TAG, entry.getKey() + "/" + entry.getValue());
        }
        if (data.containsKey("title"))
            msg = data.get("title").toString();

        Log.d(TAG, "From: " + from);
        Log.d(TAG, "Size: " + data.size());

        //adapt that if you want to react to topics
        //individually.
        if (from.startsWith("/topics/")) {
            // message received from some topic.
        } else {
            // normal downstream message.
        }

        sendNotification(msg);
    }

    private void sendNotification(String message)
    {
        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this)
                .setSmallIcon(R.drawable.icon)
                .setContentTitle("StamOrga Nachricht")
                .setContentText(message)
                .setAutoCancel(true);

        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

        notificationManager.notify(0 /* ID of notification */, notificationBuilder.build());
    }
}
