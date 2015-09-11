package com.example.gtflauncher;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GtfHmiSurface extends SurfaceView implements SurfaceHolder.Callback {

    private final static String LOG_TAG = "GtfHmiSurface";

    public GtfHmiSurface(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        getHolder().addCallback(this);

        // Make transparent
        getHolder().setFormat(PixelFormat.TRANSPARENT);
        Log.i(LOG_TAG, "GtfHmiSurface()");
    }

    public void printSurfaceInfo(Surface s)
    {
        if (!s.isValid())
            Log.w(LOG_TAG, "printSurfaceInfo(): invalid!");
        else
            Log.i(LOG_TAG, "printSurfaceInfo(): " + s.toString());
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder)
    {
        Log.i(LOG_TAG, "surfaceCreated()");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h)
    {
        Log.i(LOG_TAG, "surfaceChanged(" + w + "," + h + ")");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder)
    {
        Log.i(LOG_TAG, "surfaceDestroyed()");
    }
}
