import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;
import org.bytedeco.javacpp.tools.*;

@Properties(value=@Platform(include="RemoteBuf.h", link="RemoteBuf", linkpath="/home/amaro/repos/remotebuf"),
            target="RemoteBuf")
public class RemoteBufConfig implements InfoMapper {
  public void map(InfoMap infoMap) {}
}
