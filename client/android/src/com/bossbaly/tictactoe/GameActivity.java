package com.bossbaly.tictactoe;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;
import base.handler.EndCommandHandler;
import base.handler.ErrorCommandHandler;
import base.handler.MoveCommandHandler;
import base.handler.StartCommandHandler;
import base.serverinterface.TicTacToeRecieveInterface;
import base.serverinterface.TicTacToeSendInterface;
import base.thread.TicTacToeRecieveThread;

public class GameActivity extends Activity implements OnClickListener,
		MoveCommandHandler, ErrorCommandHandler, EndCommandHandler, StartCommandHandler {

	private Button button[] = new Button[9];

	private TicTacToeSendInterface sendInterface;
	private TicTacToeRecieveInterface recieveInterface;
	private TicTacToeRecieveThread recieveThread;

	private Socket socket;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_game);
		
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
		final String host = prefs.getString("host", "localhost");
		final int port = Integer.parseInt(prefs.getString("port", "32600"));

		Thread thread = new Thread() {
			@Override
			public void run() {
				try {
					
					socket = new Socket(host, port);
					sendInterface = new TicTacToeSendInterface(socket);
					recieveThread = new TicTacToeRecieveThread(socket,
							GameActivity.this, GameActivity.this,
							GameActivity.this, GameActivity.this);
					recieveThread.start();
				} catch (UnknownHostException e) {
					e.printStackTrace();
					GameActivity.this.runOnUiThread(new Runnable() {
						public void run() {
							Toast.makeText(GameActivity.this,
									"Can not find host!", Toast.LENGTH_SHORT)
									.show();
							finish();
						}
					});
				} catch (IOException e) {
					e.printStackTrace();
					GameActivity.this.runOnUiThread(new Runnable() {
						public void run() {
							Toast.makeText(GameActivity.this,
									"Bad connection!", Toast.LENGTH_SHORT)
									.show();
							finish();
						}
					});
				}
			}
		};

		thread.start();

		button[0] = (Button) findViewById(R.id.button_1);
		button[1] = (Button) findViewById(R.id.button_2);
		button[2] = (Button) findViewById(R.id.button_3);
		button[3] = (Button) findViewById(R.id.button_4);
		button[4] = (Button) findViewById(R.id.button_5);
		button[5] = (Button) findViewById(R.id.button_6);
		button[6] = (Button) findViewById(R.id.button_7);
		button[7] = (Button) findViewById(R.id.button_8);
		button[8] = (Button) findViewById(R.id.button_9);

		for (int i = 0; i < button.length; i++) {
			button[i].setOnClickListener(this);
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.game, menu);
		return true;
	}

	public int coordToPos(int x, int y) {
		return (x * 3) + y;
	}

	public int xPosToCoord(int pos) {
		return (pos / 3);
	}

	public int yPosToCoord(int pos) {
		return (pos % 3);
	}

	@Override
	public void onClick(View view) {
		int position = Integer.parseInt(view.getTag().toString());

		final int x = xPosToCoord(position);
		final int y = yPosToCoord(position);

		Thread send = new Thread() {
			@Override
			public void run() {
				sendInterface.sendMoveCommand(x, y);
			}
		};

		send.start();
	}

	@Override
	public void handleEndCommand(final int status, final int player) {

		this.runOnUiThread(new Runnable() {
			public void run() {
				for (int i = 0; i < button.length; i++) {
					button[i].setEnabled(false);
				}

				if (status == 0) {
					if (player != 0) {
						Toast.makeText(
								GameActivity.this,
								"The game has ended! Player " + player
										+ " has won!", Toast.LENGTH_LONG)
								.show();
					} else {
						Toast.makeText(GameActivity.this,
								"The game has ended! It was a draw!",
								Toast.LENGTH_LONG).show();
					}
				} else {
					Toast.makeText(
							GameActivity.this,
							"The game has ended because a player disconnected. Player "
									+ player + " has won!", Toast.LENGTH_LONG)
							.show();
				}
			}
		});
	}

	@Override
	public void handleErrorCommand(final int error) {
		this.runOnUiThread(new Runnable() {
			public void run() {
				if (error == 1) {
					Toast.makeText(GameActivity.this, "Not your turn!",
							Toast.LENGTH_LONG).show();
				} else if (error == 2) {
					Toast.makeText(GameActivity.this,
							"Location is out of bounds!", Toast.LENGTH_LONG)
							.show();
				} else if (error == 3) {
					Toast.makeText(GameActivity.this,
							"Location is currently occupied!",
							Toast.LENGTH_LONG).show();
				} else {
					Toast.makeText(GameActivity.this, "Unspecified error!",
							Toast.LENGTH_LONG).show();
				}
			}
		});
	}

	@Override
	public void handleMoveCommand(final int x, final int y, final int player) {
		this.runOnUiThread(new Runnable() {
			public void run() {
				String tempLetter;
				if (player == 1) {
					tempLetter = "X";
				} else {
					tempLetter = "O";
				}

				int position = coordToPos(x, y);

				button[position].setEnabled(false);
				button[position].setText(tempLetter);
			}
		});

	}

	@Override
	public void handleStartCommand() {
		// TODO Auto-generated method stub
		
	}
}
