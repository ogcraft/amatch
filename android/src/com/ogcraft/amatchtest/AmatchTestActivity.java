/*
AmatchTestActivity.java:
Copyright (c) 2014, Oleg Galbert
All rights reserved.
*/

package com.ogcraft.amatchtest;

//import amatch_generated.amatch_interface;
import android.app.Activity;
import android.os.Bundle;

public class AmatchTestActivity extends Activity {
    /** Called when the activity is first created. */
	Thread thread;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        thread = new Thread() {
			public void run() {
				setPriority(Thread.MAX_PRIORITY);
				//amatch_interface.start_process();
			}
		};
		thread.start();   
    }
    public void onDestroy(){
    	
    	super.onDestroy();
    	//amatch_interface.stop_process();
    	try {
			thread.join();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    	thread = null;
    	
    }
}
