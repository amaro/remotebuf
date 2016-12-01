import org.bytedeco.javacpp.*;
import java.io.*;

public class TestRemoteBuf {
  public static void main(String[] args) {
    RemoteBuf.BufferManager BM = new RemoteBuf.BufferManager();

    {
      RemoteBuf.Buffer B = BM.CreateBuffer("buf1");
      if (!BM.BufferExists("buf1"))
        throw new RuntimeException("buffer wasn't created");

      String msg = "hihihi";
      B.Write(msg.getBytes(), msg.getBytes().length);
      B.Flush();
      byte result[] = new byte[B.GetSize()];
      B.Read(result);
      String strRes = new String(result);
      if (!strRes.equals(msg))
        throw new RuntimeException("buffers didn't match");
    }

    {
      RemoteBuf.Buffer B2 = BM.CreateBuffer("buf2");
      String msg = "hello";
      B2.Write(msg.getBytes(), msg.getBytes().length);
      msg = "there";
      B2.Write(msg.getBytes(), msg.getBytes().length);
      B2.Flush();
      byte result2[] = new byte[B2.GetSize()];
      B2.Read(result2);
      String strRes2 = new String(result2);
      if (!strRes2.equals("hellothere"))
        throw new RuntimeException("buffers didn't match: " + strRes2);
    }

    {
      RemoteBuf.Buffer B3 = BM.CreateBuffer("buf3");
      String msg = "buffeeeerrrrr3";
      ROutputStream OS = new ROutputStream(B3);
      OS.write(msg.getBytes());
      OS.close();
      byte result3[] = new byte[B3.GetSize()];
      B3.Read(result3);
      String strRes3 = new String(result3);
      if (!strRes3.equals(msg))
        throw new RuntimeException("buffers didn't match: " + strRes3);
    }

    System.out.println("OK");
  }
}
