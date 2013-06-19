package com.apdfviewer;

import java.io.FileNotFoundException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.opengl.GLUtils;
import android.util.Log;

/**
 * Render a pair of tumbling cubes.
 */

class PDFRenderer implements GLSurfaceView.Renderer {
	final static String TAG = "PDFRenderer";

	private Context m_context = null;

	/**
	 * The PDFDocument object
	 */
	private PDFDocument m_doc = null;

	/**
	 * file descriptor of the PDF document.
	 */
	private AssetFileDescriptor m_descriptor = null;
	

	/**
	 * current page number, default is 1.
	 */
	private int m_current_page = 1;
	
	private int m_texture = -1;

	protected static FloatBuffer makeFloatBuffer(float[] arr) {
		ByteBuffer bb = ByteBuffer.allocateDirect(arr.length * 4);
		bb.order(ByteOrder.nativeOrder());
		FloatBuffer fb = bb.asFloatBuffer();
		fb.put(arr);
		fb.position(0);
		return fb;
	}

	float vexcoords[] = new float[] { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
	float texcoords[] = new float[] { 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
	//float texcoords[] = new float[] { 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f};

	FloatBuffer vex_buf;
	FloatBuffer tex_buf;
	
    protected int loadTexture(GL10 gl) {
        gl.glEnable(GL10.GL_TEXTURE_2D);
        
        Bitmap bmp = BitmapFactory.decodeResource(m_context.getResources(), R.drawable.ic_launcher);

        int[] tex = new int[1];

        gl.glGenTextures(1, tex, 0);

        gl.glBindTexture(GL10.GL_TEXTURE_2D, tex[0]);
        GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bmp, 0);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);

        return tex[0];
    }

	public PDFRenderer(Context context) {
		m_context = context;
		vex_buf = makeFloatBuffer(vexcoords);
		tex_buf = makeFloatBuffer(texcoords);
	}

	public void onDrawFrame(GL10 gl) {       
        // texture
        if (m_texture == -1) {
        	m_doc.setXdpi(72.0F);
        	m_doc.setYdpi(72.0F);

        	m_doc.drawPage(null, 10);
        	//m_texture = loadTexture(gl);
        	m_texture = m_doc.loadTexture();
        	Log.v(TAG, "load texture: " + m_texture);
        }

        gl.glBindTexture(GL10.GL_TEXTURE_2D, m_texture);
        gl.glEnable(GL10.GL_TEXTURE_2D);

		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();

        gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
        gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);

        gl.glVertexPointer(2, GL10.GL_FLOAT, 0, vex_buf);
        gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, tex_buf);

        gl.glDrawArrays(GL10.GL_TRIANGLE_FAN, 0, 4);

        gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
        gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
        
        // clean up
        gl.glDisable(GL10.GL_TEXTURE_2D);
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
		gl.glViewport(0, 0, width, height);
		float ratio = (float) width / height;
        //gl.glMatrixMode(GL10.GL_PROJECTION);
        //gl.glLoadIdentity();
        //GLU.gluPerspective(gl, 0.0f, ratio, 0.0f, 0.0f);
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		gl.glDisable(GL10.GL_DITHER);

		gl.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		gl.glShadeModel(GL10.GL_SMOOTH);
		gl.glDisable(GL10.GL_DEPTH_TEST);
	}

	public void openUri(Uri uri) {
		// open uri
		try {
			m_descriptor = m_context.getContentResolver()
					.openAssetFileDescriptor(uri, "r");
			if (m_descriptor == null) {
				Log.e(TAG, "File desciptor is null.");
			}
		} catch (FileNotFoundException e) {
			Log.e(TAG, "Open file failed.");
			return;
		}

		// open document
		m_doc = new PDFDocument(m_descriptor.getFileDescriptor(), "", "");
		if (!m_doc.isOk()) {
			// TODO: report error.
			return;
		}
	}
}
