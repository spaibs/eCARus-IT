package de.tum.ei.ecarus.ecarus;


import android.content.DialogInterface;
import android.content.res.Configuration;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.app.AlertDialog;
import android.app.Dialog;
import android.support.v4.app.DialogFragment;
import android.view.LayoutInflater;
import android.widget.Spinner;
import java.util.Locale;

public class LanguageDialog extends DialogFragment{

    Spinner langSpinner;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {

        AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(getActivity(), R.style.AlertDialogCustom);
        LayoutInflater inflater = getActivity().getLayoutInflater();
        dialogBuilder.setView(inflater.inflate(R.layout.language_dialog, null));
        /*
        langSpinner = (Spinner) getDialog().findViewById(R.id.change_lang_dialog);

        if (savedInstanceState != null) {
            langSpinner.setSelection(savedInstanceState.getInt("mySpinner", 0));
        }
        */
        dialogBuilder.setTitle(getResources().getString(R.string.lang_dialog_title));

        dialogBuilder.setPositiveButton(R.string.positive_button, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                int langpos = langSpinner.getSelectedItemPosition();
                switch (langpos) {
                    case 0: //German
                        PreferenceManager.getDefaultSharedPreferences(getActivity().getApplicationContext()).edit().putString("LANGUAGE", "de").commit();
                        setLangRecreate("de");
                        return;
                    case 1: //English
                        PreferenceManager.getDefaultSharedPreferences(getActivity().getApplicationContext()).edit().putString("LANGUAGE", "en").commit();
                        setLangRecreate("en");
                        return;
                    case 2: //French
                        PreferenceManager.getDefaultSharedPreferences(getActivity().getApplicationContext()).edit().putString("LANGUAGE", "fr").commit();
                        setLangRecreate("fr");
                        return;
                    default: //By default set to German
                        PreferenceManager.getDefaultSharedPreferences(getActivity().getApplicationContext()).edit().putString("LANG", "de").commit();
                        setLangRecreate("de");
                }
            }
        });
        dialogBuilder.setNegativeButton(R.string.negative_button, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                //pass
            }
        });
        return dialogBuilder.create();
    }


    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putInt("mySpinner", langSpinner.getSelectedItemPosition());
    }

    @Override
    public void onStart() {
        super.onStart();
        //langSpinner = (Spinner) getDialog().findViewById(R.id.change_lang_dialog);

    }

    public void setLangRecreate(String langval) {
        Configuration config = getActivity().getBaseContext().getResources().getConfiguration();
        Locale locale = new Locale(langval);
        Locale.setDefault(locale);
        config.locale = locale;
        getActivity().getBaseContext().getResources().updateConfiguration(config, getActivity().getBaseContext().getResources().getDisplayMetrics());
        getActivity().recreate();
    }

}
