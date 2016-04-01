package com.example.gtflauncher.music;

import android.app.Activity;
import android.media.MediaPlayer;
import android.net.Uri;
import android.util.Log;

public class Music
{
    public static final String LOG_TAG = "Music";

    private static Music instance = null;
    public static Music getInstance()
    {
        if(null == instance)
        {
            instance = new Music();
        }
        return instance;
    }

    Activity appActivity;
    MediaPlayer mediaPlayer;
    MediaStoreSongManager songManager;
    int lastTitleIndex = -1;
    float currentVolume = 1.0f;

    // Initialization e. g. from Activity class, e. g. with application context
    public void init(final Activity appActivity)
    {
        Log.d(LOG_TAG, "init()");
        this.appActivity = appActivity;

        songManager = new MediaStoreSongManager();
        songManager.init(appActivity.getApplicationContext());

        final String trackFile = songManager.getSongFile();
        mediaPlayer = MediaPlayer.create(appActivity, Uri.parse(trackFile));
    }

    // De-Initialization e. g. from Activity class
    public void close()
    {
        Log.d(LOG_TAG, "close()");

        mediaPlayer.release();
        mediaPlayer = null;
        songManager = null;
        appActivity = null;
        instance = null;
    }

    public String[] getTrackList()
    {
        return songManager.getTrackList();
    }

    public void playTitle(int trackIndex) {

        if(trackIndex == -1)
        {
            return;
        }

        Log.d(LOG_TAG, "Playing title with id=" + trackIndex);

        if (lastTitleIndex == trackIndex) {
            mediaPlayer.start();
            Log.d(LOG_TAG, "continue..");
        }
        else {
            mediaPlayer.stop();

            boolean trackValid = songManager.goToSong(trackIndex);
            if(trackValid)
            {
                final String trackFile = songManager.getSongFile();
                mediaPlayer = MediaPlayer.create(appActivity, Uri.parse(trackFile));
                mediaPlayer.setVolume(currentVolume, currentVolume);
                mediaPlayer.start();
            }

            //mediaPlayer = MediaPlayer.create(appActivity, Uri.parse(Environment.getExternalStorageDirectory().getPath() + "/Music/" + title + ".mp3"));
        }

        lastTitleIndex = trackIndex;
    }

    public void pauseTitle() {
        Log.d(LOG_TAG, "paused.");
        mediaPlayer.pause();
    }

    public void setVolume(float volumeValue)
    {
        Log.d(LOG_TAG, "setVolume: " + volumeValue);
        currentVolume = volumeValue;
        mediaPlayer.setVolume(volumeValue, volumeValue);
    }
}
