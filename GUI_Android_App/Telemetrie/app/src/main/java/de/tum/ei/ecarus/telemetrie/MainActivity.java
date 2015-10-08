package de.tum.ei.ecarus.telemetrie;

import android.app.AlertDialog;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.GridLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import java.util.LinkedList;
import java.util.List;
import java.util.Queue;


public class MainActivity extends AppCompatActivity
        implements NavigationDrawerCallbacks {
    Button connectButton;
    Button interpretButton;
    CheckBox pauseCheckbox;
    TextView statusBar;
    TextView lstMsgTextView;
    EditText hostEdit;
    EditText portEdit;
    GridLayout interpretedGrid;
    GridLayout RawDataGrid;
    private String host;
    private int port;
    private int interpretedGridIndex = 4;
    private int RawDataGridIndex = 4;
    int tabIndex = 0;
    int lightorder;
    private UDPThread udpThread;
    private int spinnerPosition=0;
    /**
     * Fragment managing the behaviors, interactions and presentation of the navigation drawer.
     */
    //init Fragments, Toolbar and NavigationDrawer
    private NavigationDrawerFragment mNavigationDrawerFragment;
    private Toolbar mToolbar;
    private Fragment mainFragment=null;
    private Fragment interpretedFragment=null;
    private Fragment RawDataFragment=null;
    private Fragment eCARusFragment=null;
    private Fragment aboutFragment=null;
    private Queue <byte[]> outGoingMessageQueue=null;

    private boolean headlight = true;
    private boolean fullbeam = false;
    private boolean backlight = true;
    private boolean indicatorright = true;
    private boolean indicatorleft = true;
    private boolean remotecontrolenabled = false;
    private  ImageView lightImageView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mToolbar = (Toolbar) findViewById(R.id.toolbar_actionbar);
        setSupportActionBar(mToolbar);

        mNavigationDrawerFragment = (NavigationDrawerFragment)
                getFragmentManager().findFragmentById(R.id.fragment_drawer);
        // Set up the drawer.
        mNavigationDrawerFragment.setup(R.id.fragment_drawer, (DrawerLayout) findViewById(R.id.drawer), mToolbar);

        try {
        //create Fragments <-!!! you have to comment out  "//mListener = (OnFragmentInteractionListener) activity;" in each created Fragment!!!
            Class fragmentClass;
            fragmentClass = MainFragment.class;
            if (mainFragment == null)
                mainFragment = (Fragment) fragmentClass.newInstance();
            FragmentManager fragmentManager = getFragmentManager();
            fragmentManager.beginTransaction().replace(R.id.container, mainFragment).commit();

            if(interpretedFragment == null)
                interpretedFragment = (Fragment) InterpretedFragment.class.newInstance();
            if(RawDataFragment == null)
                RawDataFragment = (Fragment)RawDataFragment.class.newInstance();
            if(eCARusFragment == null)
                eCARusFragment =  (Fragment)eCARusFragment.class.newInstance();
            if(aboutFragment == null)
                aboutFragment =  (Fragment)AboutFragment.class.newInstance();
            setTitle("Main");
        }catch(Exception e){}

        outGoingMessageQueue = new LinkedList<byte[]>();
    }
    public void initViews() //Views
    {
       //init view elements
            connectButton = (Button) findViewById(R.id.connect_Button);

            statusBar = (TextView) findViewById(R.id.status_TextView);
            hostEdit = (EditText) findViewById(R.id.host_Edit);
            portEdit = (EditText) findViewById(R.id.port_Edit);
            lstMsgTextView = (TextView) findViewById(R.id.lstMsg_TextView);
            if(lstMsgTextView != null)
                lstMsgTextView.setMovementMethod(new ScrollingMovementMethod());

            pauseCheckbox = (CheckBox) findViewById(R.id.Pause_Checkbox);
            interpretedGrid = (GridLayout) findViewById(R.id.Interpreted_Grid);
            interpretButton = (Button) findViewById(R.id.interpretButton);

    }
    @Override
    public void onNavigationDrawerItemSelected(int position) { //is activated once a navigation drawer item is selected
        // update the main content by replacing fragments
        /*
            This method only replaces the content of the container, by replacing the current fragment
            in the container with the new fragment. Each fragment contains information about 1 tab
         */
        try{
            if(udpThread!=null)
                udpThread.cancel(true);
            udpThread = null;
            tabIndex = position;
            if(position == 0) {
                FragmentManager fragmentManager = getFragmentManager();
                fragmentManager.beginTransaction().replace(R.id.container, mainFragment).commit(); // replace the old fragment with the new fragment
                setTitle("Main");
            }
            if(position == 1) {

                FragmentManager fragmentManager = getFragmentManager();
                fragmentManager.beginTransaction().replace(R.id.container, interpretedFragment).commit();
                setTitle("Interpreted Data");
            }
            if(position == 2) {

                FragmentManager fragmentManager = getFragmentManager();
                fragmentManager.beginTransaction().replace(R.id.container, RawDataFragment).commit();
                setTitle("Raw Data");
            }
            if(position == 3) {

                FragmentManager fragmentManager = getFragmentManager();
                fragmentManager.beginTransaction().replace(R.id.container, eCARusFragment).commit();
                setTitle("eCARus 2.0");
            }
            if(position == 4) {

                FragmentManager fragmentManager = getFragmentManager();
                fragmentManager.beginTransaction().replace(R.id.container, aboutFragment).commit();
                setTitle("About");
            }

            interpretedGridIndex = 4;
            RawDataGridIndex = 4;
        }catch(Exception e){
            Log.d("Exception",e.toString());}
    }

    @Override
    public void onBackPressed() {
        if (mNavigationDrawerFragment.isDrawerOpen())
            mNavigationDrawerFragment.closeDrawer();
        else
            super.onBackPressed();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        if (!mNavigationDrawerFragment.isDrawerOpen()) {
            // Only show items in the action bar relevant to this screen
            // if the drawer is not showing. Otherwise, let the drawer
            // decide what to show in the action bar.
            getMenuInflater().inflate(R.menu.main, menu);
            return true;
        }
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public void on_ConnectButton_Clicked(View v)
    {
        initViews();
        //  setup_Connection();
        check_Connection();
    }
    public void on_InterpretButton_Clicked(View view) // activates the interpretion in the interpreted tab
    {
        initViews();
        if(interpretButton.getText().toString().equals("Start")) {
            interpretButton.setText("Stop");                            //start UDP thread
            udpThread = (UDPThread) new UDPThread(this).execute();
        }
        else{
            interpretButton.setText("Start");
            try{
                if(udpThread != null)
                    udpThread.cancel(true);         //stop UDP thread
                udpThread = null;
            }catch(Exception e){Log.d("Exception",e.toString());}
        }
    }
    public void on_RawDataButton_Clicked(View view)// activates the data view in the RawData tab
    {
        Button RawDataButton = ((Button)(this.findViewById(R.id.RawDataStartButton)));
        if(RawDataButton.getText().toString().equals("Start"))  {
            RawDataButton.setText("Stop");
            udpThread = (UDPThread) new UDPThread(this).execute();      //start UDP thread
        }
        else{
            RawDataButton.setText("Start");
            try{
                if(udpThread != null)
                    udpThread.cancel(true);         //stop UDP thread
                udpThread = null;
            }catch(Exception e){Log.d("Exception",e.toString());}
        }
    }


    public void on_ecarusStartButton_Clicked(View view) // activates the eCARus-animation in the eCARus tab
    {
        initIndicatorImageViews();
        createWatchControlSpinner();

        Button ecarusStartButton = ((Button)(this.findViewById(R.id.ecarusStartButton)));
        if(ecarusStartButton.getText().toString().equals("Start"))  {
            ecarusStartButton.setText("Stop");
            remotecontrolenabled = true && (spinnerPosition==1);        //Remote control must be enabled by the spinner and Start Button
            udpThread = (UDPThread) new UDPThread(this).execute();      //start UDP thread
        }
        else{
            remotecontrolenabled = false;
            ecarusStartButton.setText("Start");
            try{
                if(udpThread != null)
                    udpThread.cancel(true);                             //stop UDP thread
                udpThread = null;
            }catch(Exception e){Log.d("Exception",e.toString());}
        }
    }
    public void initIndicatorImageViews(){
        //to prevent position failures, both indicator ImageViews are as wide as the eCARus ImageView, causing the problem that the right indicator imageView overlaps
        // the left indicator ImageView. To enable successfully touching on each image, the position of the touch must be captured by an onTouchEvent
        ImageView indicatorRightImageView = (ImageView) findViewById(R.id.indicatorRightImageView);
        indicatorRightImageView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    if (event.getX() < getResources().getDisplayMetrics().widthPixels / 2)
                        lightImageView = (ImageView) findViewById(R.id.indicatorLeftImageView); //the left indicator is activated, if the touch occurred in the left side of the screen
                    else
                        lightImageView = (ImageView) findViewById(R.id.indicatorRightImageView);
                    on_Light_clicked(lightImageView);
                }
                return true;
            }
        });
    }
    /* The spinner has two tabs: Watch and Control. If Control is selected, the animation of eCARus is updated
    *  by the touch gestures on the light images. If Watch is selected, the animation of eCARus is updated by the data,
     * sent by the Car
     */
    public void createWatchControlSpinner(){// creates a spinner view allowing to switch between control and watch mode;
        Spinner watch_Control_Spinner = (Spinner) findViewById(R.id.Watch_Control_Spinner);
        //Initiation of the Spinner
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this,  // Create an adapter between the string array, which contains the names of the items and the view
                R.array.spinner_text_array, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        watch_Control_Spinner.setAdapter(adapter);

        watch_Control_Spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                spinnerPosition = position; //item select will cause a update of the Integer spinnerPosition
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });
    }


     //checks if the connection was setup correctly.
    public void check_Connection()
    {
        WifiConfiguration wifiConf = null;
        WifiManager wifiManager = (WifiManager)getSystemService(Context.WIFI_SERVICE);
        WifiInfo connectionInfo = wifiManager.getConnectionInfo();
        List<WifiConfiguration> configuredNetworks = wifiManager.getConfiguredNetworks();
        if(configuredNetworks==null)
        {
            this.statusBar.setText("No wireless network found. WLAN disabled?");
            return;
        }
        for (WifiConfiguration conf : configuredNetworks){
            if (conf.networkId == connectionInfo.getNetworkId()){
                wifiConf = conf;
                break;
            }
        }
        if(wifiManager.getConnectionInfo().getSSID().toString().equals("\"eCarus2\""))
            statusBar.setText("You are correctly connected to " + wifiManager.getConnectionInfo().getSSID());
        else
            statusBar.setText("You are connected to the wrong network: " + wifiManager.getConnectionInfo().getSSID());

        if(wifiManager.getConnectionInfo().getIpAddress()==83994816)
            statusBar.append("\nIP: 192.168.1.5");
        else
            statusBar.append("\nModify your IP address to 192.168.1.5 !");
        // Log.d("d", "ip address:" + Formatter.formatIpAddress(wifiManager.getConnectionInfo().getIpAddress())+"unformatiert"+wifiManager.getConnectionInfo().getIpAddress());
    }

    //add RawData items to the Grid in the RawData Tab
    public void addItemRawDataGrid(String column1, String column2)
    {
        RawDataGrid = (GridLayout) findViewById(R.id.RawDataGrid);
        if(RawDataGrid == null)
            return;
        if(RawDataGrid.getRowCount()*2>RawDataGridIndex+1) {
            RawDataGrid.removeViewAt(RawDataGridIndex + 1);
            RawDataGrid.removeViewAt(RawDataGridIndex);
        }
        TextView textViewColumn1 = new TextView(this);
        textViewColumn1.setText(column1);
        textViewColumn1.setTextColor(Color.RED);
        RawDataGrid.addView(textViewColumn1, RawDataGridIndex); //add item to 1st column

        TextView textViewColumn2 = new TextView(this);
        textViewColumn2.setText(column2);
        float density = getResources().getDisplayMetrics().density;     //add padding-left to item in 2nd column
        int paddingDp = 25;
        int paddingPixel = (int) (paddingDp * density);
        textViewColumn2.setPadding(paddingPixel, 0, 0, 0);
        RawDataGrid.addView(textViewColumn2, RawDataGridIndex + 1); //add item to 2nd column

        RawDataGridIndex += 2;
    }
    public void addItemInterpretedGrid(String column1, String column2)
    {
        if(interpretedGrid == null)
            return;
        if(interpretedGrid.getRowCount()*2>interpretedGridIndex+1) {
            interpretedGrid.removeViewAt(interpretedGridIndex + 1);
            interpretedGrid.removeViewAt(interpretedGridIndex);
        }
        // else
        // interpretedGrid.setRowCount(interpretedGrid.getRowCount()+1);

        TextView textViewColumn1 = new TextView(this);
        textViewColumn1.setText(column1);
        textViewColumn1.setTextColor(Color.RED);
        interpretedGrid.addView(textViewColumn1, interpretedGridIndex); //add item to 1st column

        TextView textViewColumn2 = new TextView(this);
        textViewColumn2.setText(column2);
        float density = getResources().getDisplayMetrics().density;     //add padding-left to item in 2nd column
        int paddingDp = 25;
        int paddingPixel = (int) (paddingDp * density);
        textViewColumn2.setPadding(paddingPixel, 0, 0, 0);
        interpretedGrid.addView(textViewColumn2, interpretedGridIndex + 1); //add item to 2nd column

        interpretedGridIndex += 2;
    }
    public void setMessage(int ID, String data)// add raw data information to the RawDataGrid
    {
        if(tabIndex == 2)
        {
            CheckBox PauseRawDataCheckBox = (CheckBox) findViewById(R.id.PauseRawDataCheckBox);
            //RawDataGridIndex = 4;
            if(tabIndex == 2 && !PauseRawDataCheckBox.isChecked())
                addItemRawDataGrid(""+ID,data); //create list of raw data
        }
    }
    public void setData(String[][] data)    // param data provides information for the interpreted tab
    {
        if(tabIndex == 1)
        {
            pauseCheckbox = (CheckBox) findViewById(R.id.Pause_Checkbox);
            interpretedGridIndex = 4;

            for(int i = 1; i<58; i++)
            {
                    if(tabIndex == 1 && !pauseCheckbox.isChecked())
                        addItemInterpretedGrid(data[i][0],data[i][1]);  //create list of interpreted data
            }

        }
    }

    public void update_eCARus_Image(InterpretedData interpretedData) // interpretes incoming messages from eCARus and updates the animation
    {
        try {
            if(spinnerPosition == 0)//only update the eCARus animation ImageViews if "watch" is enabled by the Watch_Control_Spinner, update the the ImageViews by touch inputs otherwise
            {
                //image init
                ImageView brakelightImageView = (ImageView) this.findViewById(R.id.brakelightsImageView);
                ImageView backlightImageView = (ImageView) this.findViewById(R.id.backlightsImageView);
                ImageView headlightImageView = (ImageView) this.findViewById(R.id.headLightsImageView);
                ImageView indicatorLeftImageView = (ImageView) this.findViewById(R.id.indicatorLeftImageView);
                ImageView indicatorRightImageView = (ImageView) this.findViewById(R.id.indicatorRightImageView);
                if (tabIndex != 3 || brakelightImageView == null || backlightImageView == null || headlightImageView == null || indicatorLeftImageView == null || indicatorRightImageView == null)
                    return;
                if (interpretedData.BrakeLight)
                    brakelightImageView.setAlpha((float) 1);
                else
                    brakelightImageView.setAlpha((float) 0.005); //set the image transparent but not fully invisible, to keep the elements onClick feature enabled
                if (interpretedData.BackwardLight)
                    backlightImageView.setAlpha((float) 1);
                else
                    backlightImageView.setAlpha((float) 0.005);
                if (interpretedData.DirectionLeft)
                    indicatorLeftImageView.setAlpha((float) 1);
                else
                    indicatorLeftImageView.setAlpha((float) 0.005);
                if (interpretedData.DirectionRight)
                    indicatorRightImageView.setAlpha((float) 1);
                else
                    indicatorRightImageView.setAlpha((float) 0.005);
                if (interpretedData.ForwardLight || interpretedData.FullBeamLight)
                    headlightImageView.setAlpha((float) 1);
                else
                    headlightImageView.setAlpha((float) 0.005);
                if (!interpretedData.FullBeamLight && interpretedData.ForwardLight)
                    headlightImageView.setImageDrawable(getResources().getDrawable(R.drawable.full_beam_65_vertical));
                else
                    headlightImageView.setImageDrawable(getResources().getDrawable(R.drawable.headlights_65_vertical)); //switch between full beam and headlight-image
            }
        }catch(Exception e){}
    }

    public void setStatus(String title, String statusMessage) //creates a warning alert
    {
        if(statusMessage == "YOU ARE NOT CONNECTED!")   //if the udp-Thread has a connection problem, stop updating the eCARus animation
        {
            interpretButton = (Button)findViewById(R.id.interpretButton);
            Button RawDataButton = (Button) findViewById(R.id.RawDataStartButton);
            Button ecarusStartButton = (Button) findViewById(R.id.ecarusStartButton);
            if(interpretButton != null)
                interpretButton.setText("Start");
            if(RawDataButton != null)
                RawDataButton.setText("Start");
            if(ecarusStartButton != null)
            {
                ecarusStartButton.setText("Start");
                remotecontrolenabled = false;
            }
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

    public void on_Light_clicked(View view) //activated by touch inputs on eCARus animations ImageViews
    {
        if(!(view instanceof ImageView) || spinnerPosition!=1) // if "watch" is selected by the spinner, let the incoming UDP messages update the animation
            return;
        lightImageView = (ImageView) view;

        if(lightImageView == null || tabIndex != 3)
            return;
        byte[] message = new byte[14];
        byte[] data;
        if(udpThread!=null)
            data = udpThread.lightbytes; //get the last incoming information about the light status
        else
            data = new byte[8];

        if(lightImageView.getAlpha() == (float)1)   //turn off the lights, if they're turned on
        {
            lightImageView.setAlpha((float) 0.005);
            backlight = (view.getId() == R.id.backlightsImageView) ? false : backlight;
            indicatorleft = (view.getId() == R.id.indicatorLeftImageView) ? false : indicatorleft;
            indicatorright = (view.getId() == R.id.indicatorRightImageView) ? false : indicatorright;
        }
        else if(lightImageView.getAlpha() == (float)0.005) //turn on the lights, if they're turned off
        {
            lightImageView.setAlpha((float)1);
            backlight = (view.getId() == R.id.backlightsImageView) ? true : backlight;
            indicatorleft = (view.getId() == R.id.indicatorLeftImageView) ? true : indicatorleft;
            indicatorright = (view.getId() == R.id.indicatorRightImageView) ? true : indicatorright;
        }
        if(view.getId() == R.id.headLightsImageView) // if the headlightimage is clicked
        {
            boolean tmp = fullbeam;
            fullbeam =  headlight; // headlight action chronology: 1.headlight off, 2.headlight on, 3.full beam, 4.headlight off
            headlight = !tmp && !headlight;
            if(fullbeam) { //full beam is on
                lightImageView.setImageDrawable(getResources().getDrawable(R.drawable.full_beam_65_vertical));

                float density = getResources().getDisplayMetrics().density;
                int margin = 74; // the fullbeam image is 74 pixels bigger then the image of the headlights
                int marginPixel = (int) (margin * density);
                ViewGroup.MarginLayoutParams lp = (ViewGroup.MarginLayoutParams) lightImageView.getLayoutParams();
                lp.setMargins(0,-marginPixel,0,0); //add negative margin to avoid ImageView movements;
                lightImageView.setLayoutParams(lp);

                lightImageView.setAlpha((float) 1);
            }
            else {
                ViewGroup.MarginLayoutParams lp = (ViewGroup.MarginLayoutParams) lightImageView.getLayoutParams();
                lp.setMargins(0,0,0,0);
                lightImageView.setLayoutParams(lp);
                lightImageView.setImageDrawable(getResources().getDrawable(R.drawable.headlights_65_vertical));
            }
        }

        if(udpThread!=null && remotecontrolenabled)
            createCanMessage();                     //create a Can message with the information about the lights.
    }


    public byte[] getMessageToBeSent() //dequeue messages from the outGoingMessageQueue
    {
        if(!outGoingMessageQueue.isEmpty())
            return outGoingMessageQueue.remove(); // move the oldest message on the Queue to udp-Thread
        else
            return null;
    }

    public int createCanMessage ()  // create a remote control message only with information about the lights
    {
        int ID = 0x190;     // remote control id
        byte[] data_tmp = new byte[8]; // the message
        byte[] message_tmp = new byte[14]; // the message

        if(remotecontrolenabled)
            data_tmp[0] += remotecontrolenabled ? 0x0C: 0;
        data_tmp[1] += headlight ? 0x1 : 0; // Head Light
        //            tmp[1] += (fullBeamIsOn() || RemoteHeadLightFlashBool()) ? 0x2: 0; // Full Beam
        data_tmp[1] += fullbeam ? 0x2: 0; // Full Beam
        data_tmp[1] += indicatorright ? 0x4: 0;	// direction indicator left
        data_tmp[1] += indicatorleft ? 0x8: 0; // direction indicator right
        // direct transfer digital values

        // set Start and Config Field
        message_tmp[0] =(byte) 0xAA;	// start
        message_tmp[1] =(byte) 0x00; // config

        // set ID
        message_tmp[2] = (byte)(ID & 0xFF);
        message_tmp[3] = (byte)((ID >> 8) & 0xFF);
        message_tmp[4] = (byte) ((ID >> 16) & 0xFF);

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
