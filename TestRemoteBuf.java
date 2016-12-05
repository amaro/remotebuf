import org.bytedeco.javacpp.*;
import java.io.*;
import java.nio.ByteBuffer;
import ucb.remotebuf.*;

public class TestRemoteBuf {
  public static void main(String[] args) {
    RemoteBuf.BufferManager BM = new RemoteBuf.BufferManager();

    {
      RemoteBuf.Buffer B = BM.createBuffer("buf1");
      if (!BM.bufferExists("buf1"))
        throw new RuntimeException("buffer wasn't created");

      String msg = "hihihi";
      B.write(msg.getBytes(), msg.getBytes().length);
      B.flush();
      byte result[] = new byte[B.getSize()];
      B.read(result);
      String strRes = new String(result);
      if (!strRes.equals(msg))
        throw new RuntimeException("buffers didn't match");
    }

    {
      RemoteBuf.Buffer B2 = BM.createBuffer("buf2");
      String msg = "hello";
      B2.write(msg.getBytes(), msg.getBytes().length);
      msg = "there";
      B2.write(msg.getBytes(), msg.getBytes().length);
      B2.flush();
      byte result2[] = new byte[B2.getSize()];
      B2.read(result2);
      String strRes2 = new String(result2);
      if (!strRes2.equals("hellothere"))
        throw new RuntimeException("buffers didn't match: " + strRes2);
    }

    {
      RemoteBuf.Buffer B3 = BM.createBuffer("buf3");
      String msg = "buffeeeerrrrr3";
      ROutputStream OS = new ROutputStream(B3);
      OS.write(msg.getBytes());
      OS.close();
      byte result3[] = new byte[B3.getSize()];
      B3.read(result3);
      String strRes3 = new String(result3);
      if (!strRes3.equals(msg))
        throw new RuntimeException("buffers didn't match: " + strRes3);
    }

    {
      RemoteBuf.Buffer buf = BM.createBuffer("buf4");
      String msg = "buffeeeerrrrr4";
      RWritableByteChannel WBC = new RWritableByteChannel(buf);
      ByteBuffer BB = ByteBuffer.wrap(msg.getBytes());
      WBC.write(BB);
      byte res[] = new byte[buf.getSize()];
      buf.read(res);
      String resStr = new String(res);
      if (!resStr.equals(msg))
        throw new RuntimeException("buffers didn't match: " + resStr);
    }

    {
      RemoteBuf.Buffer buf = BM.createBuffer("buf5");
      String msg = "holahola";
      ROutputStream OS = new ROutputStream(buf);
      try {
        OS.write(msg.getBytes());
        OS.write(msg.getBytes(), 2, 4);
      } catch (IOException e) {
        e.printStackTrace();
      }
      OS.close();
      byte res[] = new byte[buf.getSize()];
      buf.read(res);
      msg = "hoholalahola";
      String resStr = new String(res);
      if (!resStr.equals(msg))
        throw new RuntimeException("buffers didn't match: " + resStr);
    }

    System.out.println("OK");
  }
}
