package ucb.remotebuf;

import java.io.OutputStream;
import java.io.IOException;

public class ROutputStream extends OutputStream {
  RemoteBuf.Buffer Buffer;

  public ROutputStream(RemoteBuf.Buffer B) {
    Buffer = B;
  }

  @Override
  public void close() {
    flush();
  }

  @Override
  public void flush() {
    Buffer.flush();
  }

  @Override
  public void write(byte[] b) {
    Buffer.write(b, b.length);
  }

  @Override
  public void write(byte[] b, int off, int len) throws IOException {
    throw new IOException("(amaro) not implemented");
  }

  /* not 100% sure this works */
  @Override
  public void write(int b) {
    byte buf[] = new byte[1];
    buf[0] = (byte) b;
    Buffer.write(buf, 1);
  }
}
