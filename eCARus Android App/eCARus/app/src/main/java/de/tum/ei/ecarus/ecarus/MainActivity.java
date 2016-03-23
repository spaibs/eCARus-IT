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

        //starts a new UDP thread
        udpThread = (UDPThread) new UDPThread(this).execute();

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
                if(udpThread != null) {
                    //stop UDP thread
                    Log.d("ecarus", "udp thread gestartet");
                }
            }
            //Data fragment
            else {
                rootView = inflater.inflate(R.layout.fragment_data, container, false);

                // Set text view monitoring the current speed
                TextView speed = (TextView) rootView.findViewById(R.id.speed_text);
                String speedValue = interpretedData[6][0];
                speed.setText(speedValue);

                // Set battery widget monitoring the current battery level
                BatteryWidget batteryWidget = (BatteryWidget) rootView.findViewById(R.id.batterywidget);

                int batteryLevel = Integer.parseInt(interpretedData[0][0]);
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
    public void setImage(boolean headlightsState, boolean backlightsState, boolean leftBlinkerState, boolean rightBlinkerState, boolean brakelightsState, boolean fullBeamState, boolean warningLightsState) {
        Log.d("ecarus", "new image");

        backlights_img= (ImageView) findViewById(R.id.backlights_view);
        headlights_img= (ImageView) findViewById(R.id.headlights_view);
        left_blinker_img = (ImageView) findViewById(R.id.left_blinker_view);
        right_blinker_img = (ImageView) findViewById(R.id.right_blinker_view);
        full_beam_img = (ImageView) findViewById(R.id.full_beam_view);
        brakelights_img = (ImageView) findViewById(R.id.brakelights_view);

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

        // Set backlights/brakelights
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

    // This function receives interpreted data stored in the string array data.....
    // It is called by the UDP thread class.
    // This way the MainActivity gets access to the received information...
    public void setData(String[][] data)
    {
        interpretedData = data;
    }

    // This function interpretes incoming messages from eCARus and updates the animation
    public void update_eCARus_Image(String[][] interpretedData)
    {
        String batteryLevel = interpretedData[0][0];
        String leftBlinkerValue = interpretedData[1][0];
        String rightBlinkerValue = interpretedData[2][0];
        String warningLightsValue = interpretedData[3][0];
        String headlightsValue = interpretedData[4][0];
        String fullBeamValue = interpretedData[5][0];
        String speed = interpretedData[6][0];
        String backlightsValue = interpretedData[7][0];
        String brakelightsValue = interpretedData[8][0];


        // conversion from string to boolean because the setImage function requires boolean params
        boolean leftBlinkerState = stringToBoolean(leftBlinkerValue);
        boolean rightBlinkerState = stringToBoolean(rightBlinkerValue);
        boolean warningLightsState = stringToBoolean(warningLightsValue);
        boolean headlightsState = stringToBoolean(headlightsValue);
        boolean fullBeamState = stringToBoolean(fullBeamValue);
        boolean backlightsState = stringToBoolean(backlightsValue);
        boolean brakelightsState = stringToBoolean(brakelightsValue);




        setImage(headlightsState, backlightsState, leftBlinkerState, rightBlinkerState, brakelightsState, fullBeamState, warningLightsState);

    }

    // This function should be used to turn a string variable with the value "1" or "0" into a boolean variable.
    public boolean stringToBoolean(String string) {
        return string.equals("1");
    }

}