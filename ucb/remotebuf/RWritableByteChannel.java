package ucb.remotebuf;

import java.nio.channels.WritableByteChannel;
import java.nio.ByteBuffer;

public class RWritableByteChannel implements WritableByteChannel {
  RemoteBuf.Buffer Buffer;
  boolean Open = true;

  public RWritableByteChannel(RemoteBuf.Buffer B) {
    Buffer = B;
  }

  public int write(ByteBuffer b) {
    int remaining = b.remaining();
    byte arr[] = new byte[remaining];
    b.get(arr);
    Buffer.write(arr, arr.length);
    return remaining;
  }

  public void close() {
    Buffer.flush();
    Open = false;
  }

  public boolean isOpen() {
    return Open;
  }
}
