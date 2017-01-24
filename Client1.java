
//package ClientThread;

import java.io.*;
import java.net.*;
import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.image.BufferedImage;

class ClientClass extends Thread {

	public void run() {
		Socket socket = null;
		DataInputStream input = null;
		try {
			socket = new Socket("192.168.20.246", 5005);
			input = new DataInputStream(socket.getInputStream());

		} catch (UnknownHostException e1) {

			e1.printStackTrace();
		} catch (IOException e1) {

			e1.printStackTrace();
		}

		try {

			InputStreamReader isr = new InputStreamReader(System.in);
			BufferedReader br = new BufferedReader(isr);
			OutputStreamWriter osw = new OutputStreamWriter(socket.getOutputStream(), "UTF-8");

			// User inputs from available resolutions, and that is send to the
			// camera
			System.out.println("Choose resolution --> 320x180, 480x270, 640x360, 800x450 : /n");
			String res = br.readLine();
			osw.write(res, 0, res.length());
			osw.flush();

			// user inputs fps
			System.out.println("Give fps : /n");
			String fps = br.readLine();
			osw.write(fps, 0, fps.length());
			osw.flush();
			System.out.println("Resolution sent: " + res);

			// create panel to display image
			JFrame imageGUI = new JFrame();
			imageGUI.setVisible(true);
			imageGUI.setSize(900, 900);
			imageGUI.setTitle("Our beautiful image");
			imageGUI.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			JLabel label = new JLabel();
			imageGUI.add(label);

			// Read Image bytes
			while (true) {
				if (socket.isConnected()) {

					BufferedImage bImg = null;
					int picsize = input.readInt();

					System.out.println("Picture size: " + picsize);

					byte[] img = new byte[picsize];

					int x = 0;

					System.out.println("try to read image");
					while (true) {
						int read = input.read(img, x, picsize - x);
						x = x + read;
						if (x == picsize)
							break;

					}
					System.out.println("image read");
					InputStream in = new ByteArrayInputStream(img);
					bImg = ImageIO.read(in);

					ImageIcon image = new ImageIcon(bImg);
					label.setIcon(image);
					label.repaint();
					label.validate();

				}
			}

		} catch (IOException e) {
			System.out.println("Error 2... ");
		}

	}

}

public class Client1 {

	public static void main(String args[]) throws UnknownHostException, IOException {

		Thread myClass = new ClientClass();
		myClass.start();

	}

}
