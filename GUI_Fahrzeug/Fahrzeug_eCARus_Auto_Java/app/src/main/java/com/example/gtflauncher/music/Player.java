package com.example.gtflauncher.music;

import android.media.MediaPlayer;
import android.util.Log;

import com.example.gtflauncher.LauncherActivity;


public class Player {
    MediaPlayer player;
    LauncherActivity l;

    public Player(LauncherActivity launcherActivity) {
        this.l = launcherActivity;
        //player = MediaPlayer.create(launcherActivity, Uri.parse(Environment.getExternalStorageDirectory().getPath() + "/Music/gin.mp3"));
        //player.setLooping(true);
    }

    public void play(String title) {
        /*try {
            player.setDataSource(l, Uri.parse(Environment.getExternalStorageDirectory().getPath() + "/Music/gin.mp3"));
        } catch (IOException e) {
            e.printStackTrace();
        }*/
        //player.start();
        Log.d("MusicJNIAccess", "playing title " + title);
    }

    public static void playTitle(String title)
    {
        Log.d("MusicJNIAccess", "playing title " + title);
        // TODO
    }

    public static void pause() {
        Log.d("MusicJNIAccess", "paused.");
    }
}