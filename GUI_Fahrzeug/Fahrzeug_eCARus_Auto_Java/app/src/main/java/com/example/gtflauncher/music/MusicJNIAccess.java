package com.example.gtflauncher.music;

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

    public static void setVolume(int volume) {
        //TODO
    }

    // JNI Methods: Java --> C++
    private static native void addTitle(boolean b1, boolean b2, boolean b3);
}