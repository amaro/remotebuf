package ucb.remotebuf;

import java.io.OutputStream;
import java.io.IOException;
import java.io.ByteArrayOutputStream;

public class ROutputStream extends OutputStream {
  RemoteBuf.Buffer Buffer;
  ByteArrayOutputStream LocalBuf;
  boolean Flushed;

  public ROutputStream(RemoteBuf.Buffer B) {
    Buffer = B;
    LocalBuf = new ByteArrayOutputStream();
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
  public void write(byte[] b) throws IOException {
    write(b, 0, b.length);
  }

  @Override
  public void write(byte[] b, int off, int len) throws IOException {
    LocalBuf.write(b, off, len);
  }

  @Override
  public void write(int b) {
    LocalBuf.write(b);
  }

  private void doActualWrite() {
    Buffer.write(LocalBuf.toByteArray(), LocalBuf.size());
  }
}
