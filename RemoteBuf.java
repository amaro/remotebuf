// Targeted by JavaCPP version 1.2.5: DO NOT EDIT THIS FILE

import java.nio.*;
import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

public class RemoteBuf extends RemoteBufConfig {
    static { Loader.load(); }

// Parsed from RemoteBuf.h

// #include <vector>
// #include <string>
// #include <future>
// #include <unordered_map>
// #include <cassert>
// #include <stdexcept>
// #include <mutex>
/* Buffer is not reentrant */
@Namespace("RemoteBuf") @NoOffset public static class Buffer extends Pointer {
    static { Loader.load(); }
    /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
    public Buffer(Pointer p) { super(p); }
    /** Native array allocator. Access with {@link Pointer#position(long)}. */
    public Buffer(long size) { super((Pointer)null); allocateArray(size); }
    private native void allocateArray(long size);
    @Override public Buffer position(long position) {
        return (Buffer)super.position(position);
    }

    public Buffer() { super((Pointer)null); allocate(); }
    private native void allocate();

    /* Writes buf with size s to LocalBuf */
    public native void write(@Cast("char*") BytePointer buf, @Cast("unsigned int") int s);
    public native void write(@Cast("char*") ByteBuffer buf, @Cast("unsigned int") int s);
    public native void write(@Cast("char*") byte[] buf, @Cast("unsigned int") int s);

    /* Flush pending requests (write) */
    public native void flush();

    /* Copy LocalBuf to buf */
    public native void read(@Cast("char*") BytePointer buf);
    public native void read(@Cast("char*") ByteBuffer buf);
    public native void read(@Cast("char*") byte[] buf);

    /* Get the size of the buffer, regardless of where it is located. */
    public native @Cast("unsigned int") int getSize();
}

/* BufferManager not reentrant */
@Namespace("RemoteBuf") @NoOffset public static class BufferManager extends Pointer {
    static { Loader.load(); }
    /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
    public BufferManager(Pointer p) { super(p); }
    /** Native array allocator. Access with {@link Pointer#position(long)}. */
    public BufferManager(long size) { super((Pointer)null); allocateArray(size); }
    private native void allocateArray(long size);
    @Override public BufferManager position(long position) {
        return (BufferManager)super.position(position);
    }

    public BufferManager() { super((Pointer)null); allocate(); }
    private native void allocate();

    /* Creates buffer with specified id.
     * Throws an exception if the buffer already exists
     */
    public native Buffer createBuffer(@StdString BytePointer id);
    public native Buffer createBuffer(@StdString String id);

    /* Gets the buffer with specified id.
     * Throws an exception if the buffer doesn't exist
     */
    public native Buffer getBuffer(@StdString BytePointer id);
    public native Buffer getBuffer(@StdString String id);

    /* If the buffer exists, it is deleted.
     * If it doesn't, an exception is thrown.
     */
    public native void deleteBuffer(@StdString BytePointer id);
    public native void deleteBuffer(@StdString String id);

    /* Returns true if the buffer with specified id exists. */
    public native @Cast("bool") boolean bufferExists(@StdString BytePointer id);
    public native @Cast("bool") boolean bufferExists(@StdString String id);
}



}
