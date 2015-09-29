package com.example.gtflauncher;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceView;
import android.view.View;

import com.elektrobit.gtf.GtfBridge;
import com.elektrobit.gtf.jni.callback.OnShutdownFromNativeListener;
import com.example.gtflauncher.music.Music;
import com.example.gtflauncher.music.MusicJNIAccess;
import com.example.gtflauncher.navigation.NavigationService;
import com.example.gtflauncher.tower.TowerJNIAccess;
import com.example.gtflauncher.tower.TowerReceiver;

import java.io.File;

public class LauncherActivity extends Activity implements OnShutdownFromNativeListener
{
    private SurfaceView gtfSurface;

    private final String LOG_TAG = "LauncherActivity";

    protected final String GTF_STARTUP_CFG = "gtfStartup.cfg";

    private GtfBridge gtfBridge;
    private NavigationService navigationService;
    private TowerReceiver towerReceiver;


    /* Android lifecycle management */
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {




        super.onCreate(savedInstanceState);



        // start Ethernet App
        //Intent intent = new Intent(Intent.ACTION_MAIN);
        //intent.setComponent(new ComponentName("com.fsl.ethernet","com.package.address.MainActivity"));
        //startActivity(intent);




        String startupConfigFile = getStartupConfigFile();
        setContentView(R.layout.main);

        if(!startupConfigFile.equals(""))
        {
            // Preload model specific libraries in application directory
            //JNILibraryPreloader loader = new JNILibraryPreloader(this);
            //loader.preloadLibraries(new String[]{"libNavigationServiceAccess.so", "libTowerAccess.so"});

            // Initialize GTF
            initGtfSurface();
            gtfBridge = new GtfBridge(this);
            gtfBridge.loadGtfLibs(getModelPath(), getAdditionalNativeLibs(), null);
            gtfBridge.initNativeGtf(getApplicationInfo(), startupConfigFile, this, null);
            gtfBridge.setGtfSurfaceView(gtfSurface);
        }

        // Create Navigation
        navigationService = new NavigationService(this, getModelPath());

        // Initialize Tower communication
        towerReceiver = new TowerReceiver(TowerJNIAccess.getInstance());

        // Create Media
        new Music(MusicJNIAccess.getInstance(), this);



        //Intent launchIntent = getPackageManager().getLaunchIntentForPackage("com.fsl.ethernet");
        //launchIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        //launchIntent.setFlags(Intent.FLAG_ACTIVITY_NO_HISTORY);

        //startActivity(launchIntent);



        /*Intent i = getBaseContext().getPackageManager()
        .getLaunchIntentForPackage("com.fsl.ethernet");

        i.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(i);
*/


    }

    @Override
    protected void onStart()
    {

        super.onStart();
        final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                navigationService.prepareNavigation();
            }
        }, 1000);


    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        navigationService.shutdownNavigation();
        towerReceiver.shutdown();
        android.os.Process.killProcess(android.os.Process.myPid());
    }


    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);

        if(hasFocus)
        {
            // Set fullscreen immersive
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }
    }

    private void initGtfSurface()
    {
        gtfSurface = (SurfaceView) findViewById(R.id.hmiSurface);
        gtfSurface.bringToFront();
        gtfSurface.setZOrderOnTop(true);
        gtfSurface.getHolder().setKeepScreenOn(true);
    }

    /* Touch passthrough */
    @Override
    public boolean dispatchKeyEvent(KeyEvent event)
    {
        if (gtfBridge != null)
        {
            gtfBridge.dispatchKeyEvent(event);
        }

        return super.dispatchKeyEvent(event);
    }


    /**
     * Standard, but generic version of returning the method to return
     * the path to the model files.
     * <p/>
     * Use this if you placed your model files in the correct standard model
     * files directory on the device storage.
     *
     * @return The <b>standard model path</b> which is recommended to use by our guidelines
     * or <b>null</b> if the path could not be found.
     */
    protected String getStandardModelPath()
    {
        File f = getExternalFilesDir(null);
        if (isExistingDirectory(f))
        {
            return f.getAbsolutePath();
        }
        else
        {
            Log.e(LOG_TAG, "The default model path could not be found, please specify another directory!");
            return null;
        }
    }

    /**
     * This method is used to return the path where the Gtf Startup configuration file
     * is located on the device storage.<br>
     * The getModelPath() method has to be overwritten or this method will return a empty string.
     *
     * @return The path to the GTF Startup config file based on the getModelPath() value.     *
     */
    public String getStartupConfigFile()
    {
        String temp = getModelPath();
        if (temp != null)
        {
            File f = new File(temp, GTF_STARTUP_CFG);
            if (f.exists())
            {
                return f.getAbsolutePath();
            }
            else
            {
                Log.e(LOG_TAG, "gtfStartup.cfg file does not exist!");
                return "";
            }
        }
        else
        {
            return "";
        }
    }

    /* Helper methods */
    private boolean isExistingDirectory(File f)
    {
        return f != null && f.exists() && f.isDirectory();
    }

    /* Callback from native code when gtf has shutdown */
    @Override
    public void onNativeShutdown()
    {
        finish();
    }

    private String getModelPath()
    {
        // @TUM: Return alternative model path here if required
        Log.i(LOG_TAG, "Used model path: " + getStandardModelPath());
        return getStandardModelPath();
    }

    private String[] getAdditionalNativeLibs() {
        //return null;
        return new String[]{"/libTowerAccess.so",
                "/libNavigationServiceAccess.so",
                "/libMusicServiceAccess.so"};
    }
}
