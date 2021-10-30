package org.qtproject.example;
import org.qtproject.example.R;


import android.app.IntentService;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
//import android.support.v4.content.LocalBroadcastManager;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import android.util.Log;

//import com.google.firebase.iid.FirebaseInstanceId;
//import com.google.firebase.iid.FirebaseInstanceIdService;
//import com.google.firebase.messaging.FirebaseMessaging;
import com.google.firebase.messaging.FirebaseMessagingService;

//public class MyInstanceIDListenerService extends FirebaseMessagingService  {

//    private static final String TAG = "MyInstanceIDLS";

//    /**
//    * Called if InstanceID token is updated. This may occur if the security of
//    * the previous token had been compromised. Note that this is also called
//    * when the InstanceID token is initially generated, so this is where
//    * you retrieve the token.
//    */
//    // [START refresh_token]
//    @Override
//    public void onNewToken(String token) {
//      // Get updated InstanceID token.
////      String refreshedToken = FirebaseMessaging.getInstance().getToken();
//      String refreshedToken = token;
//      Log.d(TAG, "Refreshed token: " + refreshedToken);
//      // TODO: Implement this method to send any registration to your app's servers.
//      sendRegistrationToServer(refreshedToken);
//    }
//  // [END refresh_token]

//    public void sendRegistrationToServer(String token) {
//        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);

//        sharedPreferences.edit().putString(QuickstartPreferences.FCM_TOKEN, token).apply();

//        Intent gotToken = new Intent(QuickstartPreferences.FCM_TOKEN);
//        LocalBroadcastManager.getInstance(this).sendBroadcast(gotToken);
//    }


//}
