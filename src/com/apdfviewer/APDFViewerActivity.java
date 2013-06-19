package com.apdfviewer;

import com.apdfviewer.PDFRenderer;

import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;

public class APDFViewerActivity extends Activity {
    private GLSurfaceView mGLSurfaceView;
    private PDFRenderer mRender;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = getIntent();
		/*
		if (intent.getData() == null)
			return;

		Uri uri = intent.getData();
		*/
		Uri uri = Uri.parse("file:///sdcard/test.pdf");
		mRender = new PDFRenderer(this);
		mRender.openUri(uri);

        mGLSurfaceView = new GLSurfaceView(this);
        mGLSurfaceView.setRenderer(mRender);
        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        
        setContentView(mGLSurfaceView);
	}
	
    @Override
    protected void onResume() {
        super.onResume();
        mGLSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLSurfaceView.onPause();
    }

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_apdfviewer, menu);
		return true;
	}

}
