package com.example.gtflauncher.music;

import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;

import com.example.gtflauncher.LauncherActivity;

public class Music {
    static String lastTitle;
    static MediaPlayer mediaPlayer;
    static LauncherActivity launcherActivity;

    public Music(MusicJNIAccess musicJNIAccess, LauncherActivity launcherActivity) {
        // musicJNIAccess used for Java --> C++
        this.launcherActivity = launcherActivity;
        mediaPlayer = MediaPlayer.create(launcherActivity, Uri.parse(Environment.getExternalStorageDirectory().getPath() + "/Music/Gin Tonic.mp3"));
        lastTitle = "";
    }

    public static void playTitle(String title) {
        Log.d("MusicJNIAccess", "Playing title " + title);

        // TODO make in AsyncTask (?)
        // TODO fix "Should have subtitle controller already set"
        // TODO use mediaPlayer.setDataSource instead.

        if (lastTitle.equals(title)) {
            mediaPlayer.start();
            Log.d("MusicJNIAccess", "continue..");
        }
        else {
            mediaPlayer.stop();
            mediaPlayer = MediaPlayer.create(launcherActivity, Uri.parse(Environment.getExternalStorageDirectory().getPath() + "/Music/" + title + ".mp3"));
            mediaPlayer.start();
        }

        lastTitle = title;
    }

    public static void pauseTitle() {
        Log.d("MusicJNIAccess", "paused.");
        mediaPlayer.pause();
    }
}
