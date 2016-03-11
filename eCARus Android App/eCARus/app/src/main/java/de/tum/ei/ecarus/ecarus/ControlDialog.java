package de.tum.ei.ecarus.ecarus;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.DialogFragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.ImageView;
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
    // the listener checking for changes of the respective switch's status is set
    @Override
    public void onStart() {
        super.onStart();

        headlightsSwitch = (Switch) getDialog().findViewById(R.id.headlights_switch);
        final boolean headlightsState = headlightsSwitch.isChecked();

        backlightsSwitch = (Switch) getDialog().findViewById(R.id.backlights_switch);
        final boolean backlightsState = backlightsSwitch.isChecked();

        leftBlinkerSwitch = (Switch) getDialog().findViewById(R.id.left_blinker_switch);
        final boolean leftBlinkerState = leftBlinkerSwitch.isChecked();

        rightBlinkerSwitch = (Switch) getDialog().findViewById(R.id.right_blinker_switch);
        final boolean rightBlinkerState = rightBlinkerSwitch.isChecked();

        //Headlights listener
        headlightsSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                //Log.d("test", "dasdjak");
                if (isChecked) {
                    Log.d("ecarus", "Headlights are ON");
                    //TO DO: sendStatusUpdate
                    ((MainActivity)getActivity()).setImage(headlightsState, backlightsState, leftBlinkerState, rightBlinkerState);
                } else {
                    Log.d("ecarus", "Headlights are OFF");
                    //TO DO: sendStatusUpdate

                }
            }
        });

        //Backlights listener
        backlightsSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    Log.d("ecarus", "Backlights are ON");
                    //TO DO: sendStatusUpdate

                }
                else{
                    Log.d("ecarus", "Backlights are OFF");
                    //TO DO: sendStatusUpdate
                }
            }
        });

        //Left blinker listener
        leftBlinkerSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    Log.d("ecarus", "Left blinker is ON");
                    //TO DO: sendStatusUpdate
                } else {
                    Log.d("ecarus", "Left blinker is OFF");
                    //TO DO: sendStatusUpdate
                }
            }
        });

        //Right blinker listener
        rightBlinkerSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    Log.d("ecarus", "Right blinker is ON");
                    //TO DO: sendStatusUpdate
                }
                else{
                    Log.d("ecarus", "Right blinker is OFF");
                    //TO DO: sendStatusUpdate
                }
            }
        });
    }
}


