// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 1.5.1 by WSRD Tencent.
// Generated from `/usr/local/resin_system.mqq.com/webapps/communication/taf/upload/MiniGameProto_new.jce'
// **********************************************************************

package MiniGameProto;

public final class TMsgResponseListZone extends com.qq.taf.jce.JceStruct
{
    public String className()
    {
        return "MiniGameProto.TMsgResponseListZone";
    }

    public short nResultID = (short)0;

    public short nGameID = (short)0;

    public short nTotalNum = (short)0;

    public java.util.Vector vecZoneList = new java.util.Vector();

    public short getNResultID()
    {
        return nResultID;
    }

    public void  setNResultID(short nResultID)
    {
        this.nResultID = nResultID;
    }

    public short getNGameID()
    {
        return nGameID;
    }

    public void  setNGameID(short nGameID)
    {
        this.nGameID = nGameID;
    }

    public short getNTotalNum()
    {
        return nTotalNum;
    }

    public void  setNTotalNum(short nTotalNum)
    {
        this.nTotalNum = nTotalNum;
    }

    public java.util.Vector getVecZoneList()
    {
        return vecZoneList;
    }

    public void  setVecZoneList(java.util.Vector vecZoneList)
    {
        this.vecZoneList = vecZoneList;
    }

    public TMsgResponseListZone()
    {
    }

    public TMsgResponseListZone(short nResultID, short nGameID, short nTotalNum, java.util.Vector vecZoneList)
    {
        this.nResultID = nResultID;
        this.nGameID = nGameID;
        this.nTotalNum = nTotalNum;
        this.vecZoneList = vecZoneList;
    }

    public boolean equals(Object o)
    {
        TMsgResponseListZone t = (TMsgResponseListZone) o;
        return (
            com.qq.taf.jce.JceUtil.equals(nResultID, t.nResultID) && 
            com.qq.taf.jce.JceUtil.equals(nGameID, t.nGameID) && 
            com.qq.taf.jce.JceUtil.equals(nTotalNum, t.nTotalNum) && 
            com.qq.taf.jce.JceUtil.equals(vecZoneList, t.vecZoneList) );
    }

    public void writeTo(com.qq.taf.jce.JceOutputStream _os)
    {
        try
        {
            _os.write(nResultID, 0);
            _os.write(nGameID, 1);
            _os.write(nTotalNum, 2);
            _os.write(vecZoneList, 3);
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

            nGameID = 0;
            nGameID = (short) _is.read(nGameID, 1, true);

            nTotalNum = 0;
            nTotalNum = (short) _is.read(nTotalNum, 2, true);

            vecZoneList = new java.util.Vector();
            MiniGameProto.TZoneInfo __var_12 = new MiniGameProto.TZoneInfo();
            vecZoneList.addElement(__var_12);
            vecZoneList = (java.util.Vector) _is.read(vecZoneList, 3, true);

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
