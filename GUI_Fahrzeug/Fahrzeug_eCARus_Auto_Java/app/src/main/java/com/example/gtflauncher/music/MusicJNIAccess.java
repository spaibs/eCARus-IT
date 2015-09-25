package com.example.gtflauncher.music;

import android.util.Log;

public class MusicJNIAccess {
    private static MusicJNIAccess instance = null;
    private static Music music;

    public static MusicJNIAccess getInstance() {
        if (instance == null) {
            instance = new MusicJNIAccess();
        }
        return instance;
    }

    // plays title
    public static void playTitle(String title) {
        music.playTitle(title);
    }

    // pauses current title
    public static void pauseTitle() {
        music.pauseTitle();
    }

    // JNI Methods: Java --> C++
    private static native void addTitle(boolean b1, boolean b2, boolean b3);
}