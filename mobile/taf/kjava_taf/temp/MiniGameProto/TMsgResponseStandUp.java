// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 1.5.1 by WSRD Tencent.
// Generated from `/usr/local/resin_system.mqq.com/webapps/communication/taf/upload/MiniGameProto_new.jce'
// **********************************************************************

package MiniGameProto;

public final class TMsgResponseStandUp extends com.qq.taf.jce.JceStruct
{
    public String className()
    {
        return "MiniGameProto.TMsgResponseStandUp";
    }

    public short nResultID = (short)0;

    public short nRoomID = (short)0;

    public short getNResultID()
    {
        return nResultID;
    }

    public void  setNResultID(short nResultID)
    {
        this.nResultID = nResultID;
    }

    public short getNRoomID()
    {
        return nRoomID;
    }

    public void  setNRoomID(short nRoomID)
    {
        this.nRoomID = nRoomID;
    }

    public TMsgResponseStandUp()
    {
    }

    public TMsgResponseStandUp(short nResultID, short nRoomID)
    {
        this.nResultID = nResultID;
        this.nRoomID = nRoomID;
    }

    public boolean equals(Object o)
    {
        TMsgResponseStandUp t = (TMsgResponseStandUp) o;
        return (
            com.qq.taf.jce.JceUtil.equals(nResultID, t.nResultID) && 
            com.qq.taf.jce.JceUtil.equals(nRoomID, t.nRoomID) );
    }

    public void writeTo(com.qq.taf.jce.JceOutputStream _os)
    {
        try
        {
            _os.write(nResultID, 0);
            _os.write(nRoomID, 1);
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    public void readFrom(com.qq.taf.jce.JceInputStream _is)
    {
        try
        {
            nResultID = 0;
            nResultID = (short) _is.read(nResultID, 0, true);

            nRoomID = 0;
            nRoomID = (short) _is.read(nRoomID, 1, true);

        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    public void display(java.lang.StringBuffer _os, int _level)
    {
    }

}
