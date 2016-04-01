package com.example.gtflauncher.music;

public class MusicJNIAccess
{
    // We do not want this class to be instantiated
    private MusicJNIAccess() { }

    //
    // Methods called from native c++ code (Native -> Java)
    //

    // plays title
    public static void playTitle(int trackIndex)
    {
        final Music music = Music.getInstance();
        music.playTitle(trackIndex);
    }

    // pauses current title
    public static void pauseTitle()
    {
        final Music music = Music.getInstance();
        music.pauseTitle();
    }

    public static void updateVolume(float volumeValue)
    {
        final Music music = Music.getInstance();
        music.setVolume(volumeValue);
    }

    public static void getTrackList()
    {
        final Music music = Music.getInstance();
        updateTrackList(music.getTrackList());
    }


    //
    // Methods calling native c++ code (Java -> Native)
    //

    public static native void updateTrackList(String[] trackList);
}