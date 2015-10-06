package com.example.gtflauncher;

import android.content.Context;
import android.util.Log;

public class JNILibraryPreloader
{
    private final String dataDir;
    private final String LOG_TAG = "JNILibraryPreloader";

    public JNILibraryPreloader(final Context appContext)
    {
        dataDir = appContext.getApplicationInfo().dataDir;
        Log.d("JNILibraryPreloader", "dataDir: " + dataDir);
    }

    public void preloadLibraries(final String[] libNames)
    {
        if(libNames == null)
        {
            return;
        }

        for (String lib : libNames)
        {
            System.load(lib);
        }
    }
}
