package ucb.remotebuf.config;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;
import org.bytedeco.javacpp.tools.*;

@Properties(value=@Platform(include="RemoteBuf.h", link="RemoteBuf", linkpath="/home/amaro/repos/spark/external/remotebuf", compiler="cpp11"),
            target="ucb.remotebuf.RemoteBuf")
public class RemoteBuf implements InfoMapper {
  public void map(InfoMap infoMap) {}
}
