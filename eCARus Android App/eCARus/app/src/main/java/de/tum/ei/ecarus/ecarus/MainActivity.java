package de.tum.ei.ecarus.ecarus;

import android.app.FragmentTransaction;
import android.content.Intent;
import android.support.design.widget.TabLayout;
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
import android.widget.CompoundButton;
import android.widget.Switch;


public class MainActivity extends AppCompatActivity {

    private SectionsPagerAdapter mSectionsPagerAdapter;
    private ViewPager mViewPager;
    private Switch mySwitch1;

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

        //mySwitch1 = (Switch) findViewById(R.id.myswitch1);
        //Log.d(getClass().getSimpleName(), "Switch is currently ON");
        //Switch mySwitch1 = new Switch(getApplicationContext());
        //set the switch to ON
        //mySwitch1.setChecked(true);
        //attach a listener to check for changes in state
        /**mySwitch1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked){

                if(isChecked){
                    Log.d(getClass().getSimpleName(), "Switch is currently ON");
                    //TO DO: sendStatusUpdate
                }else{
                    Log.d(getClass().getSimpleName(), "Switch is currently OFF");
                    //TO DO: sendStatusUpdate
                }

            }
        });
         */

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

            Log.d(getClass().getSimpleName(), "YourOutput");
            //Intent i = new Intent(this, SettingsActivity.class);
            //startActivity(i);

            return true;

        }

        return super.onOptionsItemSelected(item);
    }

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
            if (getArguments().getInt(ARG_SECTION_NUMBER) == 1) { // Info
                rootView = inflater.inflate(R.layout.fragment_info, container, false);
            }
            else if(getArguments().getInt(ARG_SECTION_NUMBER) == 2) { // Control
                rootView = inflater.inflate(R.layout.fragment_control, container, false);
                BatteryWidget batteryWidget = (BatteryWidget) rootView.findViewById(R.id.batterywidget);
                batteryWidget.setBatteryLevel(90);
            }
            else { //Data
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
                    return "Info";
                case 1:
                    return "Steuerung";
                case 2:
                    return "Daten";
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

}
