/*
AmatchTestActivity.java:
Copyright (c) 2014, Oleg Galbert
All rights reserved.
*/

package com.ogcraft.amatchtest;
import amatch_generated.amatch_interface;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder; 
import java.nio.FloatBuffer; 
import java.util.concurrent.TimeUnit;

import com.lamerman.FileDialog;
import com.lamerman.SelectionMode;
import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
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
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class AmatchTestActivity extends Activity {
    /** Called when the activity is first created. */
	private static final String TAG = "Amatch";
	private boolean isEngineInitialized = false;
	private boolean isRecordedEnough = false;
	private String data_root_path = "";
	private String track_keys_fn = data_root_path;
	private String translation_fn = data_root_path;
	private static double SEC_PER_KEY = 0.011609977324263039;
	private double found_sec = 0;
	private double currentPlayingTime_ms = 0;
	private double finalTime_ms = 0;
	private long recording_start_ms = 0;
	private long recording_end_ms = 0;
	int file_selecting_button_id = R.id.btn_load_fpkeys;
	// Media Player
    private  MediaPlayer mp;
    private SeekBar seekbar;
    private Handler seekbar_handler = new Handler();
    private TextView progress_display_view;
	Thread load_fpkeys_thread;
	Thread match_thread;
	Thread player_thread;
	Thread recorder_thread;
	
	Handler load_fpkeys_thread_handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {			  
				Bundle bundle = msg.getData();
				long n = bundle.getLong("LoadedFpkeys");
				TextView v = (TextView)findViewById(R.id.btn_load_fpkeys);
				v.setText("Index: " + track_keys_fn.substring(data_root_path.length()+1));
				TextView v1 = (TextView)findViewById(R.id.btn_start_search);
				v1.setEnabled(true);
			}
		}; 

	Handler player_thread_handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {			  
				Bundle bundle = msg.getData();
				int i = bundle.getInt("FoundIndex");
				long time_to_match_ms = bundle.getLong("time_to_match_ms");
				Log.d(TAG,"FoundIndex: n = " + i);
				recording_end_ms = System.currentTimeMillis();
				found_sec = i * SEC_PER_KEY + (time_to_match_ms / 1000.0);
				Log.d(TAG,"Starting playing from " + found_sec + " sec");
				TextView v = (TextView)findViewById(R.id.found_display);
				double search_time_ms = recording_end_ms - recording_start_ms;
				if(i > 10) {
					v.setText("Found sec: " + found_sec + " Search took: " + search_time_ms/1000.0 + " sec" );
                	play_translation(translation_fn, found_sec);
				} else {
					v.setText("Not found. " + " Search took: " + search_time_ms/1000.0 + " sec.\n Please sync again");
					play_recorded();
				}
			}
		}; 

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        File f = Environment.getExternalStorageDirectory();
        data_root_path = f.getAbsolutePath();
        Log.d(TAG,"In onCreate()");
        mp = new MediaPlayer();
       
        seekbar = (SeekBar)findViewById(R.id.seekbar);
        seekbar.setClickable(false);
        progress_display_view = (TextView)findViewById(R.id.progress_display);
        
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
		start_recording_thread();
    }
    public void onDestroy(){
    	
    	super.onDestroy();
    	Log.d(TAG, "onDestroy(): Stop recording.");
    	amatch_interface.stop_recording();
    	amatch_interface.close_audo_device();
    	recorder_thread.interrupt();
    	isEngineInitialized = false;
    	mp.stop();
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
                	File f = new File(filename);
                	data_root_path = f.getParent();
                    Log.d(TAG, "onActivity(): filename: " + filename + " data_root_path: " + data_root_path);
                    if(file_selecting_button_id == R.id.btn_load_fpkeys) {
                    	track_keys_fn = filename;
                    	Button b = (Button)findViewById(R.id.btn_load_fpkeys);
                    	
                    	btn_load_fpkeysClick(b);        	
                    	
                    } else if(file_selecting_button_id == R.id.btn_load_translation) {
                    	Button b = (Button)findViewById(R.id.btn_load_translation);   	
                    	b.setEnabled(false);
                    	translation_fn = filename;
                    	Log.d(TAG, "translation_fn: " + translation_fn);
                    	b.setText("Translation: " + translation_fn.substring(data_root_path.length()+1));
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
    
    public void btn_stopClick(View view)
    {
		mp.stop();
	}
    
    public void btn_start_searchClick(View view)
    {
    	TextView v = (TextView)findViewById(R.id.btn_load_fpkeys);
    	v.setEnabled(false);
    	TextView v1 = (TextView)findViewById(R.id.found_display);
		v1.setText("Please wait. Synchronizing...");
		mp.stop();
		Runnable runnable = new Runnable() {
	        public void run() {     	
	        	Log.d(TAG,"Start searching");
	        	recording_start_ms = System.currentTimeMillis();
	        	int found_index = 0; //85 * 2 * 60; //match_sample();
	        	//int found_index = match_sample();
	        	//String fn = data_root_path + "/recorded.wav";
	        	//amatch_interface.write_recorded_as_file(fn);
	        	//Log.d(TAG,"Written file: " + fn);
	        	long index_found_ms = System.currentTimeMillis();
	        	long time_to_match_ms = index_found_ms - recording_start_ms; 
	        	Log.d(TAG,"fff: " + found_index + " ms took: " + time_to_match_ms);
	        	Message msg = player_thread_handler.obtainMessage();
    			Bundle bundle = new Bundle();
    			bundle.putInt("FoundIndex", found_index);
    			bundle.putLong("time_to_match_ms", time_to_match_ms);
                msg.setData(bundle);
                player_thread_handler.sendMessage(msg);
	        }
      };
      match_thread  = new Thread(runnable);
  	  match_thread.start();
    }
    
    public void start_recording_thread()
    {	
    	if(!isEngineInitialized) {
    		if(!amatch_interface.open_audio_device()) {
    			Log.e(TAG,"Failed to open audio_device");
    			return;
    		} else {
    			
    			isEngineInitialized = true;
    		}
    	}
 
    	Runnable runnable = new Runnable() {
	        public void run() {     	
	        	Log.d(TAG,"Start recording thread");
	           	amatch_interface.start_recording();
	        	recording_start_ms = System.currentTimeMillis();
	        	Log.d(TAG,"Recorder state: " + amatch_interface.recorder_state());
	        	try {
	        		int nmax = amatch_interface.nrecsamples();
	        		while(!Thread.currentThread().isInterrupted()){
		        	      int n = amatch_interface.recording();
		        	      if(n >= nmax) {
		        	    	  isRecordedEnough = true;
		        	    	  Log.d(TAG,"Recording thread: samples:" + n);
		        	    	  Thread.sleep(1000);  
		        	      }
		        	}
	             }
	             catch (InterruptedException x) {
	                return;
	             }
	        }
      };
      recorder_thread  = new Thread(runnable);
  	  recorder_thread.start();
    }
    
    private int match_sample()
    {
    	
    	Log.d(TAG,"Start match_samples()");
    	while(!isRecordedEnough) {
    		try {
    			Thread.sleep(1000);
    		} catch (InterruptedException x) {
                return 0;
             }
    	}
       	Log.d(TAG,"START MATCHING...");			
       	amatch_interface.generate_fp_keys_from_in();
    	int found_index = amatch_interface.match_sample();
 		return found_index;
    
    }
    
    public void  play_recorded(){
    	Log.d(TAG,"Play recorded");
    	//try
        //{
    		int sz = amatch_interface.get_recorded_samples_size();
    		Log.d(TAG,"sz: " + sz);
            AudioTrack audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 11025, 
            		AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT, 4096, AudioTrack.MODE_STREAM);
            
            audioTrack.play();
        	
        	float recs[] = new float[sz];
        	
        	amatch_interface.get_recorded_samples(recs);
        	
        	short samples[] = new short[sz];
        	Log.d(TAG, "-----------------\n");
        	for( int i = 0; i < sz; i++ ) {
        		
        		if(i<100 ) Log.d(TAG, "recs: " + recs[i]);
        	    samples[i] = (short)(recs[i]*Short.MAX_VALUE);
        	}

            audioTrack.write(samples, 0, sz);
            
            audioTrack.stop();
            audioTrack.release();
        //}
        //catch (IOException e)
        //{
           
        //}

    }
    public void  play_translation(String fn, double from_sec){
        // Play translation
        try {
            mp.reset();
            mp.setDataSource(fn);
            mp.prepare();
            
            Log.d(TAG,"play_translation from sec: " + from_sec*1000);
            // Move song to particular second
            mp.seekTo((int)(from_sec*1000)); // position in milliseconds
             
            mp.start();
            
            finalTime_ms = mp.getDuration();
            Log.d(TAG,"play_translation finalTime_ms: " + (finalTime_ms / 1000.0));
            Log.d(TAG,"seekbar: " + seekbar);
            seekbar.setMax((int)finalTime_ms);
            long t = mp.getCurrentPosition();
            Log.d(TAG, "play_translation(): t: " + (t / 1000.0));
            seekbar.setProgress((int)t);
            seekbar_handler.postDelayed(UpdateTranslationTime,100);
          
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalStateException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    private Runnable UpdateTranslationTime = new Runnable() {
        public void run() {
           currentPlayingTime_ms = mp.getCurrentPosition();
           long min = TimeUnit.MILLISECONDS.toMinutes((long) currentPlayingTime_ms);
           long sec = TimeUnit.MILLISECONDS.toSeconds((long) currentPlayingTime_ms) - 
                   TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes((long) currentPlayingTime_ms));
           
           progress_display_view.setText(
        		   String.format("Playing %.2f ( %3d:%02d ) sec from %.2f sec",
                		   			currentPlayingTime_ms / 1000.0,
                		   			min, sec,
                		   			finalTime_ms / 1000.0
        		   ));
           
           seekbar.setProgress((int)currentPlayingTime_ms);
           seekbar_handler.postDelayed(this, 100);
        }
     };
}
