import org.bytedeco.javacpp.*;
import java.io.*;

public class TestRemoteBuf {
  public static void main(String[] args) {
    RemoteBuf.BufferManager BM = new RemoteBuf.BufferManager();
    RemoteBuf.Buffer B = BM.CreateBuffer("buf1");
    if (!BM.BufferExists("buf1"))
      throw new RuntimeException("buffer wasn't created");

    String msg = "hihihi";
    B.Write(msg.getBytes(), msg.getBytes().length);
    B.WriteDone();
    byte result[] = new byte[B.GetSize()];
    B.Read(result);
    String strRes = new String(result);
    if (!strRes.equals(msg))
      throw new RuntimeException("buffers didn't match");

    RemoteBuf.Buffer B2 = BM.CreateBuffer("buf2");
    msg = "hello";
    B2.Write(msg.getBytes(), msg.getBytes().length);
    msg = "there";
    B2.Write(msg.getBytes(), msg.getBytes().length);
    B2.WriteDone();
    byte result2[] = new byte[B2.GetSize()];
    B2.Read(result2);
    String strRes2 = new String(result2);
    if (!strRes2.equals("hellothere"))
      throw new RuntimeException("buffers didn't match: " + strRes2);

    System.out.println("OK");
  }
}
