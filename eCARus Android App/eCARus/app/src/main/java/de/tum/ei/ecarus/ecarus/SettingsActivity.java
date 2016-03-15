package de.tum.ei.ecarus.ecarus;

import android.os.Bundle;
import android.preference.PreferenceFragment;
import android.preference.PreferenceActivity;

/**
 * The SettingsActivity is used to enable the user to change the settings of the app
 * such as the language.
 */

//TODO: add toolbar

public class SettingsActivity extends PreferenceActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SettingsFragment sf = new SettingsFragment();

        getFragmentManager().beginTransaction()
                .replace(android.R.id.content, sf)
                .commit();
    }

    public static class SettingsFragment extends PreferenceFragment {
        @Override
        public void onCreate(final Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            // Load the preferences from an XML resource
            addPreferencesFromResource(R.xml.preferences);

        }
    }
}


