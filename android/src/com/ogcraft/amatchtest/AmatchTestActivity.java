/*
AmatchTestActivity.java:
Copyright (c) 2014, Oleg Galbert
All rights reserved.
*/

package com.ogcraft.amatchtest;
import com.lamerman.FileDialog;
import com.lamerman.SelectionMode;

import amatch_generated.amatch_interface;
import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class AmatchTestActivity extends Activity {
    /** Called when the activity is first created. */
	private static final String TAG = "Amatch";
	private static final String data_root_path = "/storage/sdcard0/asearch";
	private String track_keys_fn = data_root_path;
	private String translation_fn = data_root_path;
	private static double SEC_PER_KEY = 0.011609977324263039;
	int file_selecting_button_id = R.id.btn_load_fpkeys;
	Thread load_fpkeys_thread;
	Handler load_fpkeys_thread_handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {			  
				Bundle bundle = msg.getData();
				long n = bundle.getLong("LoadedFpkeys");
				TextView v = (TextView)findViewById(R.id.btn_load_fpkeys);
				v.setText("Loaded " + n + " fpkeys");
				TextView v1 = (TextView)findViewById(R.id.btn_start_search);
				v1.setEnabled(true);
			}
		}; 
	Thread recorder_thread;
	Handler recorder_thread_handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {			  
				Bundle bundle = msg.getData();
				int i = bundle.getInt("FoundIndex");
				Log.d(TAG,"FoundIndex: n = " + i);
				double found_sec = i * SEC_PER_KEY;
				Log.d(TAG,"Found: sec = " + found_sec);
				TextView v = (TextView)findViewById(R.id.found_display);
				if(i > 10) {
					v.setText("Found sec: " + found_sec);
				} else {
					v.setText("Not found");
				}
			}
		}; 
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG,"In onCreate()");
        setContentView(R.layout.activity_main);
        final Button btnLoadFpkeys = (Button)findViewById(R.id.btn_load_fpkeys);
        btnLoadFpkeys.setOnClickListener(new OnClickListener()
        {
            @Override
            public void onClick(final View v)
            {
            	file_selecting_button_id = R.id.btn_load_fpkeys;
            	Log.d(TAG,"onClick() btnLoadFpkeys");
                final Intent intent = new Intent(AmatchTestActivity.this.getBaseContext(), FileDialog.class);
				intent.putExtra(FileDialog.START_PATH, data_root_path);
                intent.putExtra(FileDialog.CAN_SELECT_DIR, false);
                // set file filter
                intent.putExtra(FileDialog.FORMAT_FILTER, new String[] { "fpkeys" });
                intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
                AmatchTestActivity.this.startActivityForResult(intent, FileAction.LOAD.value);
            }
        });
        final Button btnLoadTranslation = (Button)findViewById(R.id.btn_load_translation);
        btnLoadTranslation.setOnClickListener(new OnClickListener()
        {
            @Override
            public void onClick(final View v)
            {
            	file_selecting_button_id = R.id.btn_load_translation;
            	Log.d(TAG,"onClick() btnLoadTranslation");
                final Intent intent = new Intent(AmatchTestActivity.this.getBaseContext(), FileDialog.class);
				intent.putExtra(FileDialog.START_PATH, data_root_path);
                intent.putExtra(FileDialog.CAN_SELECT_DIR, false);
                // set file filter
                intent.putExtra(FileDialog.FORMAT_FILTER, new String[] { "mp3","ogg","wav" });
                intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
                AmatchTestActivity.this.startActivityForResult(intent, FileAction.LOAD.value);
            }
        });
        TextView v = (TextView)findViewById(R.id.fpkeys_fn);
		v.setText("Amatch ver: " + amatch_interface.amatch_version());
		((TextView)findViewById(R.id.btn_start_search)).setEnabled(false);
		TextView v1 = (TextView)findViewById(R.id.found_display);
		v1.setText("");

        //thread = new Thread() {
		//	public void run() {
		//		setPriority(Thread.MAX_PRIORITY);
		//		//amatch_interface.start_process();
		//	}
		//};
		//thread.start();   
    }
    public void onDestroy(){
    	
    	super.onDestroy();
    	//amatch_interface.stop_process();
    	//try {
			//thread.join();
    		
			
		//} catch (InterruptedException e) {
			// TODO Auto-generated catch block
		//	e.printStackTrace();
		//}
    	load_fpkeys_thread = null;
    	
    }
    @Override
    public synchronized void onActivityResult(final int requestCode, final int resultCode, final Intent data)
    {
        if (resultCode == Activity.RESULT_OK)
        {
            final String filename = data.getStringExtra(FileDialog.RESULT_PATH);
            switch (FileAction.fromValue(requestCode))
            {
                case LOAD:
                    Log.d(TAG, "onActivity(): filename: " + filename);
                    if(file_selecting_button_id == R.id.btn_load_fpkeys) {
                    	Button b = (Button)findViewById(R.id.btn_load_fpkeys);
                    	b.setText(filename);
                    	btn_load_fpkeysClick(b);
                    	track_keys_fn = filename;
                    } else if(file_selecting_button_id == R.id.btn_load_translation) {
                    	Button b = (Button)findViewById(R.id.btn_load_translation);
                    	b.setText("Loaded");
                    	b.setEnabled(false);
                    	translation_fn = filename;
                    	Log.d(TAG, "translation_fn: " + translation_fn);
                    }
                    break;
                case SAVE:
                    break;
                case KEY:
                    break;
            }
           
        }
        else if (resultCode == Activity.RESULT_CANCELED)
            ; // do nothing
    }

    public void btn_load_fpkeysClick(View view)
    {
    	TextView v = (TextView)findViewById(R.id.btn_load_fpkeys);
    	v.setEnabled(false);
		Runnable runnable = new Runnable() {
	        public void run() {     	
	        	Log.d(TAG,"Start loading fpkeys from " + track_keys_fn);
	        	long keys = amatch_interface.read_track_fpkeys(track_keys_fn);
	        	Log.d(TAG,"read_track_fpkeys(): " + keys);
	        	Message msg = load_fpkeys_thread_handler.obtainMessage();
    			Bundle bundle = new Bundle();
    			bundle.putLong("LoadedFpkeys", keys);
                msg.setData(bundle);
                load_fpkeys_thread_handler.sendMessage(msg);
	        }
      };
      load_fpkeys_thread  = new Thread(runnable);
  	  load_fpkeys_thread.start();
    }
    
    public void btn_start_searchClick(View view)
    {
    	TextView v = (TextView)findViewById(R.id.btn_load_fpkeys);
    	v.setEnabled(false);
    	TextView v1 = (TextView)findViewById(R.id.found_display);
		v1.setText("Searching...");
		Runnable runnable = new Runnable() {
	        public void run() {     	
	        	Log.d(TAG,"Start searching");
	        	int found_index = match_sample();
	        	Log.d(TAG,"fff: " + found_index);
	        	Message msg = recorder_thread_handler.obtainMessage();
    			Bundle bundle = new Bundle();
    			bundle.putInt("FoundIndex", found_index);
                msg.setData(bundle);
                recorder_thread_handler.sendMessage(msg);
	        }
      };
      recorder_thread  = new Thread(runnable);
  	  recorder_thread.start();
    }
    
    private int match_sample()
    {
//    	if(!amatch_interface.init_amatch_context()) {
//			Log.e(TAG, "Failed to create amatch_context");	
//		} else {
//			Log.d(TAG,"amatch_context created");
//		}
    	
		if(!amatch_interface.open_audio_device()) {
    		Log.e(TAG,"Failed to open audio_device");
    		return 0;
		}
    	
    	Log.d(TAG,"Skip 25 frames...");
    	amatch_interface.skip_samples(25);

       	Log.d(TAG,"START RECORDING...");			
       	amatch_interface.generate_fp_keys_from_in();
    	int found_index = amatch_interface.match_sample();
    	
    	amatch_interface.close_audo_device();
    	
//    	amatch_interface.destroy_amatch_context();
//		Log.d(TAG,"amatch_context destroyed");
		
		return found_index;
    
    }
}
