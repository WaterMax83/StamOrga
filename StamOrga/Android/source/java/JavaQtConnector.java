package com.watermax.stamorga;

import org.qtproject.qt5.android.QtNative;

import java.lang.String;
import java.io.File;
import android.content.Intent;
import android.util.Log;
import android.net.Uri;
import android.content.ContentValues;
import android.content.Context;
//import android.support.v4.content.FileProvider;
import androidx.core.content.FileProvider;

public class JavaQtConnector
{
    protected JavaQtConnector()
    {
    }

        /* This function was to test installing a app instead of QDesktopServices::openUrl()
         * openURL() did not work with Qt5.9 and SDK >= 24: android.os.FileUriExposedException
         * I tried it with this function, but I got the same error (probably same code in QT)
         * So I updated AndroidManifest and xml/provider_paths.xml and this function with FileProvider
         * but the result was that parsing apk got a problem, so I switched back to SDK = 23 and try
         * again after updating Qt Version */

//    public static int installApp(String appPackageName) {
//        if (QtNative.activity() == null)
//            return -1;
//        try {
//            Intent intent = new Intent(Intent.ACTION_VIEW);
//            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

//            Uri apkURI = FileProvider.getUriForFile(
//                            QtNative.activity(),
//                            QtNative.activity().getApplicationContext().getPackageName() + ".provider",
//                            new File(appPackageName));

//            intent.setDataAndType(apkURI, "application/vnd.android.package-archive");

//            /* this is the handling before SDK >= 24 */
////            intent.setDataAndType(Uri.fromFile(new File(appPackageName)),
////                                               "application/vnd.android.package-archive");

//            QtNative.activity().startActivity(intent);
//            //W System.err: android.os.FileUriExposedException: file:///storage/emulated/0/Download/StamOrga.Android.v1.1.4.apk exposed beyond app through Intent.getData()
//            return 0;
//        } catch (android.content.ActivityNotFoundException anfe) {
//            return -3;
//        }
//    }

}
