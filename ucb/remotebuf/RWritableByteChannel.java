package ucb.remotebuf;

import java.nio.channels.WritableByteChannel;
import java.nio.ByteBuffer;

public class RWritableByteChannel implements WritableByteChannel {
  RemoteBuf.Buffer Buffer;

  public RWritableByteChannel(RemoteBuf.Buffer B) {
    Buffer = B;
  }

  public int write(ByteBuffer b) {
    byte arr[] = b.array();
    Buffer.write(arr, arr.length);
    return arr.length;
  }

  public void close() {
    Buffer.flush();
  }

  public boolean isOpen() {
    return true;
  }
}
