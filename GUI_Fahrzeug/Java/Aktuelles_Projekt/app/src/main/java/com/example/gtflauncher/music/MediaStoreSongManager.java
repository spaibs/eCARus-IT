////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

package com.example.gtflauncher.music;

import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.media.MediaMetadataRetriever;
import android.net.Uri;
import android.provider.MediaStore;
import android.provider.MediaStore.Audio.AudioColumns;
import android.util.Log;

import java.util.ArrayList;

public class MediaStoreSongManager
{
	private final static String LOG_TAG = "ExampleAudioPlayer";

    // See http://stackoverflow.com/questions/5298858/list-all-of-one-file-type-on-android-device
    private final Uri mediaPath = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
    private final String selection = AudioColumns.IS_MUSIC + " != 0";

    // Specify here what columns are needed
    private final String[] TRACK_COLUMNS = new String[] {
            MediaStore.Audio.AudioColumns.ALBUM,		// Album name
            MediaStore.Audio.AudioColumns.ARTIST,		// Artist name
            MediaStore.Audio.AudioColumns.DURATION,	// Song duration
            MediaStore.MediaColumns.TITLE,				// The track title
            MediaStore.MediaColumns.DATA,				// The file location
    };

	private ContentResolver contentResolver;
	private Cursor cursor;
	
	private int artistColumn;
	private int albumColumn;
	private int titleColumn;
	private int durationColumn;
	private int fileColumn;

	public boolean init(Context ctx)
	{
		contentResolver = ctx.getContentResolver();
        cursor = contentResolver.query(mediaPath, TRACK_COLUMNS, selection, null, null);

        // If content resolved and first item is pointed at
        // move to first will let the cursor point to first row "0"
        if (cursor != null && cursor.moveToFirst()) 
        {
        	artistColumn = cursor.getColumnIndex(MediaStore.Audio.AudioColumns.ARTIST);
        	albumColumn = cursor.getColumnIndex(MediaStore.Audio.AudioColumns.ALBUM);
        	titleColumn = cursor.getColumnIndex(MediaStore.MediaColumns.TITLE);
        	durationColumn = cursor.getColumnIndex(MediaStore.Audio.AudioColumns.DURATION);
        	fileColumn = cursor.getColumnIndex(MediaStore.MediaColumns.DATA);
        	
        	Log.d(LOG_TAG, "init() success, found " + cursor.getCount() + " items");

        	return true;
    	}
		else if(cursor.getCount() == 0)
		{
			Log.d(LOG_TAG, "init() success, but didn't find any songs.");
			return true;
		}

        return false;
	}

	public void close()
	{
		cursor.close();
	}

	public int getNrOfFiles()
	{
		return cursor.getCount();
	}

	public String getSongFile()
	{
		Log.d(LOG_TAG, "getSongFile(): " + cursor.getString(fileColumn));
		return cursor.getString(fileColumn);
	}

	public boolean nextSong()
	{	
		// Check for out of bounds
		if(cursor.getPosition() < (cursor.getCount()-1))
		{
			return cursor.moveToNext();
		}
		return false;
	}

	public boolean previousSong()
	{
		// Check for out of bounds
		if(cursor.getPosition() > 0)
		{
			return cursor.moveToPrevious();
		}
		return false;
	}

	public boolean firstSong()
	{
		// cursor.moveToFirst returns false if cursor is empty or move was unsuccessful
		return cursor.moveToFirst();
	}

	public boolean lastSong()
	{
		// cursor.moveToLast returns false if cursor is empty or move was unsuccessful
		return cursor.moveToLast();
	}

    public boolean goToSong(final int songIndex)
    {
        return cursor.moveToPosition(songIndex);
    }

	public int getSongIndex()
	{
		Log.d(LOG_TAG, "getSongIndex(): cursor index=" + cursor.getPosition());
		return cursor.getPosition();
	}

	public String getSongInfoArtist()
	{
		return cursor.getString(artistColumn);
	}

	public String getSongInfoAlbum()
	{
		return cursor.getString(albumColumn);
	}

	public String getSongInfoTitle()
	{
		return cursor.getString(titleColumn);
	}

	public long getSongInfoDuration()
	{
		return cursor.getLong(durationColumn);
	}

	public byte[] getCoverArt()
	{
		final MediaMetadataRetriever mediaRetriever = new MediaMetadataRetriever();
		mediaRetriever.setDataSource(getSongFile());

		return mediaRetriever.getEmbeddedPicture();
	}

	public String[] getTrackList()
	{
		ArrayList<String> trackList = new ArrayList<String>();

		Cursor tempCursor = contentResolver.query(mediaPath, TRACK_COLUMNS, selection, null, null);

        if(tempCursor != null && tempCursor.getCount() > 0 && tempCursor.moveToFirst())
        {
            do
            {
                final long durationSeconds = tempCursor.getLong(durationColumn) / 1000;
                final int minutes = (int) durationSeconds / 60;
                final int seconds = (int) durationSeconds % 60;
                String secondsString;
                if(seconds < 10)
                {
                    secondsString = "0" + seconds;
                }
                else
                {
                    secondsString = "" + seconds;
                }

                String item = tempCursor.getString(artistColumn) + " - " + tempCursor.getString(titleColumn);
                item += " [" + minutes + ":" + secondsString + "]";
                trackList.add(item);
            }
            while(tempCursor.moveToNext());
        }

        return trackList.toArray(new String[0]);
	}
}
