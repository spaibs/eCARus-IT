package com.example.displaysocketservice;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class MyActivity extends Activity {

	public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        Intent intent = new Intent(this,SocketService.class);
        this.startService(intent);
        finish();
    }
}
