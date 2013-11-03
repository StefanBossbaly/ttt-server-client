import java.io.IOException;
import java.io.*;
import java.net.*;
import java.nio.charset.Charset;

public class Driver {

   public static void main(String[] args) throws UnknownHostException, IOException{
      int portNumber = 9876;
      
      Socket socket = new Socket("localhost", portNumber);
      
      //need to fix this
      OutputStreamWriter writer = new OutputStreamWriter(socket.getOutputStream(), ("US-ASCII"));    
      
      
      writer.write("Hello Server 1\0", 0, "Hello Server 1\0".length());
   
      
      
      writer.close();
      socket.close();
        
   }
   
   public void makeSocket(int portNumber, int address){
   
   }
   
   public void playerTurn(char player, int row, int col){
      
   } 

   public void declareDraw(){
   
   }
   
   public void playerWin(char player){
   
   }
   
   public void playerLoss(char player){
   
   }
}