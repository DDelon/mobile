// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 1.5.1 by WSRD Tencent.
// Generated from `/usr/local/resin_system.mqq.com/webapps/communication/taf/upload/MiniGameProto_new.jce'
// **********************************************************************

package MiniGameProto;

public final class TUserSpeed extends com.qq.taf.jce.JceStruct
{
    public String className()
    {
        return "MiniGameProto.TUserSpeed";
    }

    public int iUin = (int)0;

    public int iSpeed = (int)0;

    public int getIUin()
    {
        return iUin;
    }

    public void  setIUin(int iUin)
    {
        this.iUin = iUin;
    }

    public int getISpeed()
    {
        return iSpeed;
    }

    public void  setISpeed(int iSpeed)
    {
        this.iSpeed = iSpeed;
    }

    public TUserSpeed()
    {
    }

    public TUserSpeed(int iUin, int iSpeed)
    {
        this.iUin = iUin;
        this.iSpeed = iSpeed;
    }

    public boolean equals(Object o)
    {
        TUserSpeed t = (TUserSpeed) o;
        return (
            com.qq.taf.jce.JceUtil.equals(iUin, t.iUin) && 
            com.qq.taf.jce.JceUtil.equals(iSpeed, t.iSpeed) );
    }

    public void writeTo(com.qq.taf.jce.JceOutputStream _os)
    {
        try
        {
            _os.write(iUin, 0);
            _os.write(iSpeed, 1);
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
            iUin = 0;
            iUin = (int) _is.read(iUin, 0, true);

            iSpeed = 0;
            iSpeed = (int) _is.read(iSpeed, 1, true);

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
