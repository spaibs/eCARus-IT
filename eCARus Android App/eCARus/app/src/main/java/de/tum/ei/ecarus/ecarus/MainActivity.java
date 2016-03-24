package de.tum.ei.ecarus.ecarus;

import android.app.AlertDialog;
import android.content.DialogInterface;
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
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;


public class MainActivity extends AppCompatActivity {

    private SectionsPagerAdapter mSectionsPagerAdapter;
    private ViewPager mViewPager;

    private static UDPThread udpThread;
    private static String[][] interpretedData;
    private static int[] infoData;
    private static Boolean[] lightData;
    private static boolean error;

    ImageView backlights_img;
    ImageView headlights_img;
    ImageView left_blinker_img;
    ImageView right_blinker_img;
    ImageView full_beam_img;



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
        //mViewPager.setOffscreenPageLimit(2);

        TabLayout tabLayout = (TabLayout) findViewById(R.id.tabs);
        tabLayout.setupWithViewPager(mViewPager);

        //starts a new UDP thread
        udpThread = (UDPThread) new UDPThread(this).execute();

        infoData = this.getInfoData();
        lightData = this.getLightData();

        error = false;


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
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

        // Create the fragment corresponding to the selected section tab
        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {

            View rootView;


            // Info fragment
            if (getArguments().getInt(ARG_SECTION_NUMBER) == 1) {
                rootView = inflater.inflate(R.layout.fragment_info, container, false);
            }
            // Control fragment
            else if(getArguments().getInt(ARG_SECTION_NUMBER) == 2) {
                rootView = inflater.inflate(R.layout.fragment_control, container, false);

            }
            //Data fragment
            else {
                rootView = inflater.inflate(R.layout.fragment_data, container, false);


                // Set text view monitoring the current speed
                TextView speed = (TextView) rootView.findViewById(R.id.speed_text);
                int speedValue;
                if (error) {
                    speedValue = 0;
                }
                else {
                    speedValue = infoData[0];
                }
                speed.setText(Integer.toString(speedValue) + " km/h");

                // Set battery widget monitoring the current battery level
                BatteryWidget batteryWidget = (BatteryWidget) rootView.findViewById(R.id.batterywidget);
                int batteryLevel;
                if (error) {
                    batteryLevel = 20;
                }
                else {
                    batteryLevel = infoData[1];
                }
                batteryWidget.setBatteryLevel(batteryLevel);
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

        // return number of tabs
        @Override
        public int getCount() {
            // Show 3 total pages.
            return 3;
        }

        // return tab title
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

    // Control button function
    // open control dialog
    public void startControl(View view){
        ControlDialog cd = new ControlDialog();
        cd.show(getSupportFragmentManager(), "");
    }


    // Called when the application is closed
    @Override
    protected void onStop() {
        super.onStop();

    }


    // This function updates the eCARus images according to the states of the lights
    // that are set by using the switches in the control dialog.
    // It is called when the positive button in the control dialog is clicked.
    public void setImage(boolean headlightsState, boolean backlightsState, boolean leftBlinkerState, boolean rightBlinkerState, boolean fullBeamState, boolean warningLightsState) {
        Log.d("ecarus", "new image");

        backlights_img= (ImageView) findViewById(R.id.backlights_view);
        headlights_img= (ImageView) findViewById(R.id.headlights_view);
        left_blinker_img = (ImageView) findViewById(R.id.left_blinker_view);
        right_blinker_img = (ImageView) findViewById(R.id.right_blinker_view);
        full_beam_img = (ImageView) findViewById(R.id.full_beam_view);

        // Set headlights/full beam
        if(headlightsState){
            if(fullBeamState){
                full_beam_img.setVisibility(View.VISIBLE);
                headlights_img.setVisibility(View.INVISIBLE);
            }
            else {
                headlights_img.setVisibility(View.VISIBLE);
                full_beam_img.setVisibility(View.INVISIBLE);
            }
        }
        else {
            headlights_img.setVisibility(View.INVISIBLE);
            if(fullBeamState) {
                full_beam_img.setVisibility(View.VISIBLE);
            }
            else {
                full_beam_img.setVisibility(View.INVISIBLE);
            }
        }

        // Set backlights
        if(backlightsState) {
            backlights_img.setVisibility(View.VISIBLE);
        }
        else {
            backlights_img.setVisibility(View.INVISIBLE);
        }

        // Only one blinker can be on at a time
        // Set left blinker
        if(leftBlinkerState) {
            left_blinker_img.setVisibility(View.VISIBLE);
            right_blinker_img.setVisibility(View.INVISIBLE);
        }
        else {
            left_blinker_img.setVisibility(View.INVISIBLE);
        }

        // Set right blinker
        if(rightBlinkerState) {
            right_blinker_img.setVisibility(View.VISIBLE);
            left_blinker_img.setVisibility(View.INVISIBLE);
        }
        else {
            right_blinker_img.setVisibility(View.INVISIBLE);
        }

        // Set warning light
        if(warningLightsState) {
            right_blinker_img.setVisibility(View.VISIBLE);
            left_blinker_img.setVisibility(View.VISIBLE);
        }
    }



    // Create a warning alert if not connected
    public void setStatus(String title, String statusMessage) {
        // if the udp-Thread has a connection problem, stop updating the eCARus animation
        if(statusMessage.equals("YOU ARE NOT CONNECTED!")) {
            if(udpThread!=null)
                udpThread.cancel(true);
            udpThread = null;
            error = true;
        }
        else {
            error = false;
        }
        new AlertDialog.Builder(this)
                .setTitle(title)
                .setMessage(statusMessage)
                .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        // continue with delete
                    }
                })
                .setNegativeButton(android.R.string.no, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        // do nothing
                    }
                })
                .setIcon(android.R.drawable.ic_dialog_alert)
                .show();

    }

    // This function receives interpreted data stored in the string array data.
    // It is called by the UDP thread class.
    // This way the MainActivity gets access to the received information.
    public void setInfoData(int[] data) {

        infoData = data;
    }

    // This function returns the interpreted data speed and battery value.
    // It is called by the control dialog.
    public int[] getInfoData() {

        return infoData;
    }

    public Boolean[] getLightData() {
        return lightData;
    }

    // This function interpretes incoming messages from eCARus and updates the animation
    public void updateImage(Boolean[] interpretedLightData) {
        boolean leftBlinkerState = interpretedLightData[0];
        boolean rightBlinkerState = interpretedLightData[1];
        boolean warningLightsState = interpretedLightData[2];
        boolean headlightsState = interpretedLightData[3];
        boolean fullBeamState = interpretedLightData[4];
        boolean backlightsState = interpretedLightData[5];

        setImage(headlightsState, backlightsState, leftBlinkerState, rightBlinkerState, fullBeamState, warningLightsState);
    }

}