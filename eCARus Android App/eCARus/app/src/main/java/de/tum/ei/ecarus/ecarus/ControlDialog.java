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
import android.widget.Switch;


/****************************
 * The ControlDialog class is used for modifying
 * the values controlling the blinker and the headlights.
 * It is called by clicking on the control button.
 * Then the Dialog Fragment is opened displaying the current values.
 * These can be changed by using checkboxes and clicking on the positive button.
 */

public class ControlDialog extends DialogFragment{

    Switch mySwitch1;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity(), R.style.AlertDialogCustom);
        // Get the layout inflater
        LayoutInflater inflater = getActivity().getLayoutInflater();


        //set the switch to ON
        /*mySwitch1.setChecked(true);
        Log.d(getClass().getSimpleName(), "Switch is currently ON");
        //attach a listener to check for changes in state
        mySwitch1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

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

    @Override
    public void onStart() {
        super.onStart();
        mySwitch1 = (Switch) getDialog().findViewById(R.id.switch1);
        mySwitch1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                Log.d("test", "dasdjak");
            }
        });
    }
}


