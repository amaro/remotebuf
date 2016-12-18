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
/*
    int readLen = BM.get_read_alloc(a);
    byte buf_in[] = new byte[readLen];
//    BM.read(buf_in);
    System.out.println(Arrays.toString(buf_in));
*/
    System.out.println("OK");
  }
}
