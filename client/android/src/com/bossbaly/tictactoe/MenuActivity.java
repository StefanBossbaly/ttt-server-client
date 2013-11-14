package com.bossbaly.tictactoe;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MenuActivity extends Activity implements OnClickListener {

	private Button buttonAbout, buttonPlay;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		this.setContentView(R.layout.activity_menu);

		buttonPlay = (Button) findViewById(R.id.button_play);
		buttonAbout = (Button) findViewById(R.id.button_about);

		buttonPlay.setOnClickListener(this);
		buttonAbout.setOnClickListener(this);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.menu, menu);
		return true;
	}

	@Override
	public void onClick(View view) {
		Intent i = null;
		switch (view.getId()) {
		case R.id.button_about:
			i = new Intent(this, AboutActivity.class);
			startActivity(i);
			break;
		case R.id.button_play:
			i = new Intent(this, GameActivity.class);
			startActivity(i);
			break;
		}
	}
}
