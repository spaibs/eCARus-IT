import android.os.Bundle;
import android.preference.PreferenceFragment;

import de.tum.ei.ecarus.ecarus.R;

/**
 * The SettingsFragment is used to enable the user to change the settings of the app
 * such as the language.
 */

public class SettingsFragment extends PreferenceFragment {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Load the preferences from an XML resource
        addPreferencesFromResource(R.xml.preferences);
    }
}

