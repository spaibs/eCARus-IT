package de.tum.ei.ecarus.ecarus;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import java.util.Locale;

public class MainActivity extends AppCompatActivity {

    private SectionsPagerAdapter mSectionsPagerAdapter;
    private ViewPager mViewPager;
    public String langval;
    ImageView backlights_img;
    ImageView headlights_img;
    ImageView left_blinker_img;
    ImageView right_blinker_img;
    ImageView full_beam_img;
    ImageView brakelights_img;




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




           }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    /**react to the user tapping/selecting an options menu item
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_settings) {

            Log.d("eCARus", "YourOutput");
            showChangeLangDialog();
            //LanguageDialog ld = new LanguageDialog();
            //ld.show(getSupportFragmentManager(), "");
            return true;
        }
        return super.onOptionsItemSelected(item);
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

    @Override
    protected void onStop() {
        super.onStop();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        /*backlights_img.setImageDrawable(null);
        headlights_img.setImageDrawable(null);
        left_blinker_img.setImageDrawable(null);
        right_blinker_img.setImageDrawable(null);
        full_beam_img.setImageDrawable(null);
        brakelights_img.setImageDrawable(null);

        super.onDestroy();*/
    }

    public void setImage(boolean headlightsState, boolean backlightsState, boolean leftBlinkerState, boolean rightBlinkerState, boolean brakelightsState, boolean fullBeamState) {
        Log.d("ecarus", "new image");

        backlights_img= (ImageView) findViewById(R.id.backlights_view);
        headlights_img= (ImageView) findViewById(R.id.headlights_view);
        left_blinker_img = (ImageView) findViewById(R.id.left_blinker_view);
        right_blinker_img = (ImageView) findViewById(R.id.right_blinker_view);
        full_beam_img = (ImageView) findViewById(R.id.full_beam_view);
        brakelights_img = (ImageView) findViewById(R.id.brakelights_view);


        //Set headlights/full beam
        if(headlightsState){
            if(fullBeamState){
                full_beam_img.setVisibility(View.VISIBLE);
            }
            else {
                headlights_img.setVisibility(View.VISIBLE);
                full_beam_img.setVisibility(View.INVISIBLE);
            }
        }
        else {
            headlights_img.setVisibility(View.INVISIBLE);
            full_beam_img.setVisibility(View.INVISIBLE);
        }

        //Set backlights/brakelights
        if(backlightsState) {
            if(brakelightsState) {
                backlights_img.setVisibility(View.INVISIBLE);
                brakelights_img.setVisibility(View.VISIBLE);
            }
            else {
                backlights_img.setVisibility(View.VISIBLE);
                brakelights_img.setVisibility(View.INVISIBLE);
            }
        }
        else {
            backlights_img.setVisibility(View.INVISIBLE);
            if(brakelightsState) {
                brakelights_img.setVisibility(View.VISIBLE);
            }
            else {
                brakelights_img.setVisibility(View.INVISIBLE);
            }
        }

        //Set left blinker
        if(leftBlinkerState){
            left_blinker_img.setVisibility(View.VISIBLE);
            right_blinker_img.setVisibility(View.INVISIBLE);
        }
        else {
            left_blinker_img.setVisibility(View.INVISIBLE);
        }

        //Set right blinker
        if(rightBlinkerState){
            right_blinker_img.setVisibility(View.VISIBLE);
            left_blinker_img.setVisibility(View.INVISIBLE);
        }
        else {
            right_blinker_img.setVisibility(View.INVISIBLE);
        }

    }
}
