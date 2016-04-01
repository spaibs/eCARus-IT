package de.tum.ei.ecarus.ecarus;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
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

import java.util.LinkedList;
import java.util.List;
import java.util.Queue;


public class MainActivity extends AppCompatActivity {

    private SectionsPagerAdapter mSectionsPagerAdapter;
    private ViewPager mViewPager;

    public UDPThread udpThread;
    private static int[] infoData;
    private static Boolean[] lightData;
    private static byte[] lightIntArray;
    private static boolean error;
    private LinkedList<byte[]> outGoingMessageQueue=null;

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


        TabLayout tabLayout = (TabLayout) findViewById(R.id.tabs);
        tabLayout.setupWithViewPager(mViewPager);

        //starts a new UDP thread
        udpThread = (UDPThread) new UDPThread(this).execute();

        error = false;

        outGoingMessageQueue = new LinkedList<>();
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == R.id.connect_button) {
            Log.d("ecarus", "Trying to connect");
            ConnectDialog condiag = new ConnectDialog();
            condiag.show(getSupportFragmentManager(), "");
            return true;
        }
        return super.onOptionsItemSelected(item);
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


                // Set text view monitoring the current speed and set battery widget monitoring the current battery level
                TextView speed = (TextView) rootView.findViewById(R.id.speed_text);
                BatteryWidget batteryWidget = (BatteryWidget) rootView.findViewById(R.id.batterywidget);
                int speedValue;
                int batteryLevel;

                if(infoData != null) {
                    speedValue = infoData[0];
                    batteryLevel = infoData[1];
                }
                else {
                    speedValue = 0;
                    batteryLevel = 100;
                }
                speed.setText(Integer.toString(speedValue) + " km/h");
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
        if(statusMessage.equals(R.string.not_connected)) {
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

    // This function returns the interpreted data about the lights.
    // It is called by the control dialog and by the main activity itself in getMessageToBeSent().
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

    //checks if the connection was set up correctly.
    public void check_Connection()
    {
        WifiManager wifiManager = (WifiManager)getSystemService(Context.WIFI_SERVICE);
        List<WifiConfiguration> configuredNetworks = wifiManager.getConfiguredNetworks();
        if(configuredNetworks==null) {
            setStatus("ERROR", getString(R.string.no_network));

            return;
        }

        if(wifiManager.getConnectionInfo().getSSID().equals("\"eCarus2\"")) {
            setStatus("INFORMATION", getString(R.string.connection_info) + wifiManager.getConnectionInfo().getSSID());

        }
        else {
            setStatus("ERROR", getString(R.string.wrong_network) + wifiManager.getConnectionInfo().getSSID());
        }

        if(wifiManager.getConnectionInfo().getIpAddress()==83994816) {
            setStatus("INFORMATION", "\nIP: 192.168.1.5");
        }
        else {
            setStatus("INFORMATION", "\n" + getString(R.string.modify_network));
        }
    }

    // Dequeue messages from the outGoingMessageQueue
    public byte[] getMessageToBeSent()
    {
        if(!outGoingMessageQueue.isEmpty())
            return outGoingMessageQueue.remove(); // move the oldest message on the Queue to udp-Thread
        else
            return null;
    }

    // Create a remote control message
    // Only Byte 6 contains information about the lights
    public int createCanMessage ()
    {
        int ID = 0x100;     // remote control id
        byte[] data_tmp = new byte[8]; // the message
        byte[] message_tmp = new byte[14]; // the message

        lightData = getLightData();


        Boolean leftBlinkerValue = lightData[0];
        Boolean rightBlinkerValue = lightData[1];
        Boolean warningLightValue = lightData[2];
        Boolean headlightsValue = lightData[3];
        Boolean fullBeamValue = lightData[4];
        Boolean backlightsValue = lightData[5];

        data_tmp[0] += leftBlinkerValue ? 0x1 : 0;
        data_tmp[0] += rightBlinkerValue ? 0x2 : 0;
        data_tmp[0] += warningLightValue ? 0x4 : 0;
        data_tmp[0] += headlightsValue ? 0x8 : 0;
        data_tmp[0] += fullBeamValue ? 0x10 : 0;
        data_tmp[0] += backlightsValue ? 0x20 : 0;

        // direct transfer digital values

        // set Start and Config Field
        message_tmp[0] =(byte) 0xAA;	// start
        message_tmp[1] =(byte) 0x00; // config

        // set ID
        message_tmp[2] = (byte)(ID & 0xFF);
        message_tmp[3] = (byte)((ID >> 8) & 0xFF);
        message_tmp[4] = (byte) ((ID >> 16) & 0xFF);


        // TODO: Check with software tutor about what to send in Bytes 0-5 and adapt message_tmp according to that

        // set Message
        for (int i=0; i < 8; i++)
        {
            message_tmp[5 + i] = data_tmp[i];
        }

        // calculate CRC
        int crc = 0;	// the checksum
        for (int i = 1; i <= 12; i++)
        {
            crc += message_tmp[i];
            // add the value of all data, id and config bytes
        }
        crc &= 0xFF; // drop everything but the last byte
        crc ^= 0xFF; // Invert (XOR with 0xFF)
        // write the checksum to the message
        message_tmp[13] = (byte)crc;

        // send the message
        try {
            outGoingMessageQueue.add(message_tmp); // stack all messages
        }catch(Exception e){}
        return 0;
    }



}