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
    public boolean headlightsState;
    public boolean backlightsState;
    public boolean leftBlinkerState;
    public boolean rightBlinkerState;

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
                        leftBlinkerState = leftBlinkerSwitch.isChecked();
                        rightBlinkerState = rightBlinkerSwitch.isChecked();
                        ((MainActivity)getActivity()).setImage(headlightsState, backlightsState, leftBlinkerState, rightBlinkerState);
                        // TODO: send status update
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
    // switchs' status are set
    @Override
    public void onStart() {
        super.onStart();

        // TODO: get real values for states
        headlightsSwitch = (Switch) getDialog().findViewById(R.id.headlights_switch);
        headlightsState = false;
        headlightsSwitch.setChecked(headlightsState);

        backlightsSwitch = (Switch) getDialog().findViewById(R.id.backlights_switch);
        backlightsState = false;
        backlightsSwitch.setChecked(backlightsState);

        leftBlinkerSwitch = (Switch) getDialog().findViewById(R.id.left_blinker_switch);
        leftBlinkerState = false;
        leftBlinkerSwitch.setChecked(leftBlinkerState);

        rightBlinkerSwitch = (Switch) getDialog().findViewById(R.id.right_blinker_switch);
        rightBlinkerState = false;
        rightBlinkerSwitch.setChecked(rightBlinkerState);

    }

}



