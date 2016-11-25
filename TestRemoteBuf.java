import org.bytedeco.javacpp.*;
import java.io.*;

public class TestRemoteBuf {
  public static void main(String[] args) {
    RemoteBuf.BufferManager BM = new RemoteBuf.BufferManager();
    RemoteBuf.Buffer B = BM.CreateBuffer("buffid");
    if (!BM.BufferExists("buffid"))
      throw new RuntimeException("buffer wasn't created");

    String msg = "hihihi";
    B.Write(msg.getBytes(), msg.getBytes().length);
    B.WriteDone();

    byte result[] = new byte[B.GetSize()];
    B.Read(result);
    String strRes = new String(result);
    System.out.println("java here, result: " + strRes);
  }
}
