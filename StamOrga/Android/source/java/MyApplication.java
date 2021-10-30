package com.watermax.stamorga;

import android.util.Log;
import android.content.Context;
import android.app.Application;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.os.Build;

public class MyApplication extends org.qtproject.qt5.android.bindings.QtApplication
{
    public static final String CHANNEL_GENERAL_ID = "General";

    private static Context context;

    public void onCreate()
    {
        super.onCreate();
        Log.d("MyApplication", "onCreate********************************************************");
        MyApplication.context = getApplicationContext();

        createNotificationChannels();
    }

    public static Context getAppContext()
    {
        return MyApplication.context;
    }

    private void createNotificationChannels() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel1 = new NotificationChannel(
                    CHANNEL_GENERAL_ID,
                    "General",
                    NotificationManager.IMPORTANCE_HIGH
            );
            channel1.setDescription("This is general channel");
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(channel1);
        }
    }
}
