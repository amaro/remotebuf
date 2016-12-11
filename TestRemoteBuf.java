import org.bytedeco.javacpp.*;
import java.io.*;
import java.nio.ByteBuffer;
import java.util.Arrays;
import ucb.remotebuf.*;

public class TestRemoteBuf {
  public static void main(String[] args) {
    RemoteBuf.BufferManager BM = new RemoteBuf.BufferManager();

    String a = "lol";
    String dat = "asdf";
    byte buf[] = dat.getBytes();
    int s = 5;
      
    System.out.println("original input:"); 
    System.out.println(Arrays.toString(buf));
    BM.write(a, buf, s);

    byte buf_in[] = new byte[10];
    BM.read(a, buf_in, 5);
    System.out.println(Arrays.toString(buf_in));


    System.out.println("OK");
  }
}
