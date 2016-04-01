package de.tum.ei.ecarus.ecarus;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.view.LayoutInflater;
import android.widget.ArrayAdapter;
import android.widget.RadioButton;
import android.widget.Spinner;
import android.widget.Switch;


/****************************
 * The ControlDialog class is used for modifying
 * the values controlling the blinker and the headlights.
 * It is called by clicking on the control button.
 * Then the Dialog Fragment is opened displaying the current values.
 * These can be changed by using checkboxes and clicking on the positive button.
 */

public class ControlDialog extends DialogFragment{

    Switch headlightsSwitch;
    Switch backlightsSwitch;
    Switch leftBlinkerSwitch;
    Switch rightBlinkerSwitch;
    Switch fullBeamSwitch;
    Switch warningLightsSwitch;
    public boolean headlightsState;
    public boolean backlightsState;
    public boolean leftBlinkerState;
    public boolean rightBlinkerState;
    public boolean fullBeamState;
    public boolean warningLightsState;



    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity(), R.style.AlertDialogCustom);
        // Get the layout inflater
        LayoutInflater inflater = getActivity().getLayoutInflater();

        // Inflate and set the layout for the dialog
        // Pass null as the parent view because its going in the dialog layout
        builder.setView(inflater.inflate(R.layout.control_dialog, null))
                // Add action buttons
                .setPositiveButton(R.string.positive_button, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int id) {
                        // sign in the user ...
                        headlightsState = headlightsSwitch.isChecked();
                        backlightsState = backlightsSwitch.isChecked();
                        fullBeamState = fullBeamSwitch.isChecked();
                        leftBlinkerState = leftBlinkerSwitch.isChecked();
                        rightBlinkerState = rightBlinkerSwitch.isChecked();
                        warningLightsState = warningLightsSwitch.isChecked();
                        ((MainActivity)getActivity()).setImage(headlightsState, backlightsState, leftBlinkerState, rightBlinkerState, fullBeamState, warningLightsState);
                        // TODO: send status update messages
                    }
                })
                .setNegativeButton(R.string.negative_button, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });
        return builder.create();
    }


    //This function is used when the dialog is opened. Here the switches are initialized and
    // switches' states are set
    @Override
    public void onStart() {
        super.onStart();

        Boolean[] interpretedLightData = ((MainActivity)getActivity()).getLightData();


        headlightsSwitch = (Switch) getDialog().findViewById(R.id.headlights_switch);
        backlightsSwitch = (Switch) getDialog().findViewById(R.id.backlights_switch);
        leftBlinkerSwitch = (Switch) getDialog().findViewById(R.id.left_blinker_switch);
        rightBlinkerSwitch = (Switch) getDialog().findViewById(R.id.right_blinker_switch);
        fullBeamSwitch = (Switch) getDialog().findViewById(R.id.full_beam_switch);
        warningLightsSwitch = (Switch) getDialog().findViewById(R.id.warning_light_switch);

        if(interpretedLightData != null) {
            leftBlinkerState = interpretedLightData[0];
            rightBlinkerState = interpretedLightData[1];
            warningLightsState = interpretedLightData[2];
            headlightsState = interpretedLightData[3];
            fullBeamState = interpretedLightData[4];
            backlightsState = interpretedLightData[5];
        }
        else {
            // default values
            leftBlinkerState = false;
            rightBlinkerState = false;
            warningLightsState = false;
            headlightsState = false;
            fullBeamState = false;
            backlightsState = false;
        }

        headlightsSwitch.setChecked(headlightsState);
        backlightsSwitch.setChecked(backlightsState);
        leftBlinkerSwitch.setChecked(leftBlinkerState);
        rightBlinkerSwitch.setChecked(rightBlinkerState);
        fullBeamSwitch.setChecked(fullBeamState);
        warningLightsSwitch.setChecked(warningLightsState);
    }


}



