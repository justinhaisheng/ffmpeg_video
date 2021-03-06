package com.haisheng.video.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.haisheng.HsTimeInfoBean;
import com.haisheng.listener.HsCompleteListener;
import com.haisheng.listener.HsErrorListener;
import com.haisheng.listener.HsLoadingListener;
import com.haisheng.listener.HsOnTimeInfoListener;
import com.haisheng.listener.HsPrepareListener;
import com.haisheng.video.R;
import com.haisheng.player.HsPlay;

import androidx.appcompat.app.AppCompatActivity;

public class MusicActivity extends AppCompatActivity {

    private static final String TAG = MusicActivity.class.getSimpleName();
    HsPlay mHsPlay;
    boolean isSeek = false;
    private TextView tvTime;
    SeekBar mSeekBar;
    private int mPlayPosition = 0;
    private SeekBar mVolumeSeekBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_music);
        tvTime = findViewById(R.id.tv_time);
        mSeekBar = findViewById(R.id.seekbar_seek);
        mVolumeSeekBar = findViewById(R.id.seekbar_volume);
        mHsPlay = new HsPlay();
        mHsPlay.setHsPrepareListener(new HsPrepareListener() {
            @Override
            public void prepare() {
                Log.d(HsPlay.TAG,"prepare");
                mHsPlay.start();
            }
        });
        mHsPlay.setHsLoadListener(new HsLoadingListener() {
            @Override
            public void loading(boolean load) {
                if (load){
                    Log.d(HsPlay.TAG,"加载中...");
                }else{
                    Log.d(HsPlay.TAG,"播放中...");
                }
            }
        });
        mHsPlay.setHsOnTimeInfoListener(new HsOnTimeInfoListener() {
            @Override
            public void onTimeInfo(HsTimeInfoBean timeInfoBean) {
                Message message = Message.obtain();
                message.what = 1;
                message.obj = timeInfoBean;
                handler.sendMessage(message);
                Log.d(HsPlay.TAG,"当前时间："+timeInfoBean.getCurrentTime()+" 总时间："+timeInfoBean.getTotalTime());
            }
        });
        mHsPlay.setHsErrorListener(new HsErrorListener() {
            @Override
            public void error(int code, String codemsg) {
                Log.e(HsPlay.TAG,"code:"+code+" codemsg:"+codemsg);
            }
        });
        mHsPlay.setHsCompleteListener(new HsCompleteListener() {
            @Override
            public void complete() {
                if (mHsPlay.getNextSource()){

                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                Thread.sleep(1000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            Log.e(HsPlay.TAG,"播放下一个");
                            mHsPlay.prepare();
                            mHsPlay.setNextSource("");
                        }
                    }).start();

                }
            }
        });
        //http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                Log.d(TAG,"onProgressChanged() progress:"+progress+" fromUser:"+fromUser+" isSeek:"+isSeek);
                if(mHsPlay.getDuration() >0 && fromUser &&isSeek){
                    mPlayPosition = mHsPlay.getDuration() * progress / 100;
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                Log.d(TAG,"onStartTrackingTouch()");
                isSeek = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Log.d(TAG,"onStopTrackingTouch() isSeek:"+isSeek);
                if(isSeek){
                    mHsPlay.seek(mPlayPosition);
                }
                isSeek = false;
            }
        });
        mVolumeSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (fromUser){
                    mHsPlay.seekVolume(progress);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public static void jumpMusicActivity(Context context){
        context.startActivity(new Intent(context,MusicActivity.class));
    }

    public void begin(View view) {
        mHsPlay.setSource("/sdcard/a.mp4");
        mHsPlay.prepare();
    }

    public void resume(View view){
        mHsPlay.resume();
    }

    public void pause(View view){
        mHsPlay.pause();
    }

    public void stop(View view){
        mHsPlay.stop();
    }

    public void next(View view){
        mHsPlay.setNextSource("http://ngcdn004.cnr.cn/live/dszs/index.m3u8");
        mHsPlay.stop();
    }

    Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if(msg.what == 1)
            {
                HsTimeInfoBean wlTimeInfoBean = (HsTimeInfoBean) msg.obj;
                tvTime.setText(HsTimeUtil.secdsToDateFormat(wlTimeInfoBean.getTotalTime(), wlTimeInfoBean.getTotalTime())
                        + "/" + HsTimeUtil.secdsToDateFormat(wlTimeInfoBean.getCurrentTime(), wlTimeInfoBean.getTotalTime()));

                if (!isSeek){
                    if(wlTimeInfoBean.getTotalTime()>0){
                        mSeekBar.setProgress(wlTimeInfoBean.getCurrentTime() * 100  / wlTimeInfoBean.getTotalTime() );
                    }else{
                        mSeekBar.setProgress(0);
                    }
                }


            }
        }
    };

    public void seek(View view) {
        mHsPlay.seek(215);
    }
}
