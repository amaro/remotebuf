package ucb.remotebuf;

import java.io.OutputStream;
import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;

public class ROutputStream extends OutputStream {
  private RemoteBuf.Buffer Buffer;

  /* Amount of data to buffer before flushing to the Rmem C++ library */
  private static final int LocalBufSize = 1024*1024;
  private ByteBuffer LocalBuf;

  private boolean Flushed;
  private int absPos; // Absolute position within the block that we are writing


  public ROutputStream(RemoteBuf.Buffer B) {
    Buffer = B;
    LocalBuf = ByteBuffer.allocate(LocalBufSize);
    Flushed = false;
  }

  @Override
  public void close() {
    flush();
  }

  @Override
  public void flush() {
    if (!Flushed) {
      Flushed = true;
      doActualWrite();
    }
  }

  @Override
  public void write(int b) throws IOException {
    byte[] bArr = {(byte)b};
    write(bArr, 0, 1);
  }

  @Override
  public void write(byte[] b) throws IOException {
    write(b, 0, b.length);
  }

  @Override
  public void write(byte[] b, int off, int len) throws IOException {

    int rem = LocalBuf.remaining();
    if(len <= rem) {
      /* Buffer will fit completely */
      LocalBuf.put(b, off, len);
    } else {
      /* We will run out of room in LocalBuf, trigger a transfer */
      LocalBuf.put(b, off, rem);

      doActualWrite();

      write(b, off + rem, len - rem);
    }
  }

  private void doActualWrite() {
    LocalBuf.flip();
    Buffer.write(LocalBuf, LocalBuf.limit());
    LocalBuf.clear();
  }
}
