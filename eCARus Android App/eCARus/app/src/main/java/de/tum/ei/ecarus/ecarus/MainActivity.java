package de.tum.ei.ecarus.ecarus;


import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.preference.PreferenceManager;
import android.support.design.widget.TabLayout;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.content.DialogInterface;
import android.widget.Spinner;

import java.util.Locale;

public class MainActivity extends AppCompatActivity {

    private SectionsPagerAdapter mSectionsPagerAdapter;
    private ViewPager mViewPager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        // Create the adapter that will return a fragment for each of the three
        // primary sections of the activity.
        mSectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager());

        // Set up the ViewPager with the sections adapter.
        mViewPager = (ViewPager) findViewById(R.id.container);
        mViewPager.setAdapter(mSectionsPagerAdapter);

        TabLayout tabLayout = (TabLayout) findViewById(R.id.tabs);
        tabLayout.setupWithViewPager(mViewPager);

        SharedPreferences settings = PreferenceManager.getDefaultSharedPreferences(this);
        //The Configuration class describes all device configuration information that can impact the resources the application retrieves.
        //This includes both user-specified configuration options (locale and scaling)
        //as well as device configurations (such as input modes, screen size and screen orientation).
        Configuration config = getBaseContext().getResources().getConfiguration();

        String language = settings.getString("LANGUAGE", "");
        //check if the language value saved in the preferences is NOT equal to the language value saved in the device's configuration
        if (! "".equals(language) && ! config.locale.getLanguage().equals(language)) {
            //The locale object represents a language/country/variant combination (e.g. "en" for the language code)
            Locale mylocale = new Locale(language);
            Locale.setDefault(mylocale);
            config.locale = mylocale;
            //update the configuration information
            getBaseContext().getResources().updateConfiguration(config, getBaseContext().getResources().getDisplayMetrics());
        }

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    //react to the user tapping/selecting an options menu item
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {

            Log.d("eCARus", "YourOutput");
            //Intent i = new Intent(this, SettingsActivity.class);
            //startActivity(i);
            showChangeLangDialog();
            //LanguageDialog ld = new LanguageDialog();
            //ld.show(getSupportFragmentManager(), "");
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    public void showChangeLangDialog() {
        AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this);
        LayoutInflater inflater = this.getLayoutInflater();
        final View dialogView = inflater.inflate(R.layout.language_dialog, null);
        dialogBuilder.setView(dialogView);

        final Spinner langSpinner = (Spinner) dialogView.findViewById(R.id.change_lang_dialog);

        dialogBuilder.setTitle(getResources().getString(R.string.lang_dialog_title));

        dialogBuilder.setPositiveButton(R.string.positive_button, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                int langpos = langSpinner.getSelectedItemPosition();
                switch(langpos) {
                    case 0: //German
                        PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("LANGUAGE", "de").commit();
                        setLangRecreate("de");
                        return;
                    case 1: //English
                        PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("LANGUAGE", "en").commit();
                        setLangRecreate("en");
                        return;
                    case 2: //French
                        PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("LANGUAGE", "fr").commit();
                        setLangRecreate("fr");
                        return;
                    default: //By default set to German
                        PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("LANG", "de").commit();
                        setLangRecreate("de");
                        }
            }
        });
        dialogBuilder.setNegativeButton(R.string.negative_button, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                //pass
            }
        });
        AlertDialog b = dialogBuilder.create();
        b.show();
    }

    public void setLangRecreate(String langval) {
        Configuration config = getBaseContext().getResources().getConfiguration();
        Locale locale = new Locale(langval);
        Locale.setDefault(locale);
        config.locale = locale;
        getBaseContext().getResources().updateConfiguration(config, getBaseContext().getResources().getDisplayMetrics());
        recreate();
    }

    /*
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putInt("mySpinner", langSpinner.getSelectedItemPosition());
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // initialize all your visual fields
        if (savedInstanceState != null) {
            langSpinner.setSelection(savedInstanceState.getInt("yourSpinner", 0));
            // do this for each of your text views
        }
    }
    */

    /**
     * A placeholder fragment containing a simple view.
     */
    public static class PlaceholderFragment extends Fragment {
        /**
         * The fragment argument representing the section number for this
         * fragment.
         */
        private static final String ARG_SECTION_NUMBER = "section_number";

        public PlaceholderFragment() {
        }
        /**
         * Returns a new instance of this fragment for the given section
         * number.
         */
        public static PlaceholderFragment newInstance(int sectionNumber) {
            PlaceholderFragment fragment = new PlaceholderFragment();
            Bundle args = new Bundle();
            args.putInt(ARG_SECTION_NUMBER, sectionNumber);
            fragment.setArguments(args);
            return fragment;
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {

            View rootView;
            if (getArguments().getInt(ARG_SECTION_NUMBER) == 1) { // Info Fragment
                rootView = inflater.inflate(R.layout.fragment_info, container, false);
            }
            else if(getArguments().getInt(ARG_SECTION_NUMBER) == 2) { // Control Fragment
                rootView = inflater.inflate(R.layout.fragment_control, container, false);
                BatteryWidget batteryWidget = (BatteryWidget) rootView.findViewById(R.id.batterywidget);
                batteryWidget.setBatteryLevel(90);
            }
            else { //Data Fragment
                rootView = inflater.inflate(R.layout.fragment_data, container, false);
            }
            return rootView;
        }
    }

    /**
     * A {@link FragmentPagerAdapter} that returns a fragment corresponding to
     * one of the sections/tabs/pages.
     */
    public class SectionsPagerAdapter extends FragmentPagerAdapter {

        public SectionsPagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int position) {
            // getItem is called to instantiate the fragment for the given page.
            // Return a PlaceholderFragment (defined as a static inner class below).
            return PlaceholderFragment.newInstance(position + 1);
        }

        //returns number of tabs
        @Override
        public int getCount() {
            // Show 3 total pages.
            return 3;
        }

        //returns tab title
        @Override
        public CharSequence getPageTitle(int position) {
            switch (position) {
                case 0:
                    return getString(R.string.fragment_info_title);
                case 1:
                    return getString(R.string.fragment_control_title);
                case 2:
                    return getString(R.string.fragment_data_title);
            }
            return null;
        }
    }

    //Control button function
    // opens control dialog
    public void startControl(View view){
        ControlDialog cd = new ControlDialog();
        cd.show(getSupportFragmentManager(), "");
    }

    //This function updates the eCARus images according to the states of the lights
    //that are set by using the switches in the control dialog.
    //It is called when the positive button in the control dialog is clicked.

    public void setImage(boolean headlightsState, boolean backlightsState, boolean leftBlinkerState, boolean rightBlinkerState) {
        Log.d("ecarus", "new image");
        ImageView img= (ImageView) findViewById(R.id.imageView);

        if(headlightsState){
            if(backlightsState){
                if(leftBlinkerState){
                    img.setImageResource(R.drawable.ecarus_image_backlights_headlights_blinker_left);
                }
                else if (rightBlinkerState){
                    img.setImageResource(R.drawable.ecarus_image_backlights_headlights_blinker_right);
                }
                else {
                    img.setImageResource(R.drawable.ecarus_image_backlights_headlights);
                }
            }
            else {
                if(leftBlinkerState){
                    img.setImageResource(R.drawable.ecarus_image_headlights_blinker_left);
                }
                else if(rightBlinkerState){
                    img.setImageResource(R.drawable.ecarus_image_headlights_blinker_right);
                }
                else {
                    img.setImageResource(R.drawable.ecarus_image_headlights);
                }
            }
        }
        else {
            if(backlightsState){
                if(leftBlinkerState){
                    img.setImageResource(R.drawable.ecarus_image_backlights_blinker_left);
                }
                else if (rightBlinkerState){
                    img.setImageResource(R.drawable.ecarus_image_backlights_blinker_right);
                }
                else {
                    img.setImageResource(R.drawable.ecarus_image_backlights);
                }
            }
            else {
                if(leftBlinkerState){
                    img.setImageResource(R.drawable.ecarus_image_blinker_left);
                }
                else if(rightBlinkerState){
                    img.setImageResource(R.drawable.ecarus_image_blinker_right);
                }
                else {
                    img.setImageResource(R.drawable.ecarus_image_normal);
                }
            }

        }
    }
}
