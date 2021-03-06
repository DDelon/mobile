// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.0.1 by WSRD Tencent.
// Generated from `StatF.jce'
// **********************************************************************

package com.qq.taf;

public final class StatSampleMsg extends com.qq.taf.jce.JceStruct
{
    public String className()
    {
        return "com.qq.taf.StatSampleMsg";
    }

    public String unid = "";

    public String masterName = "";

    public String slaveName = "";

    public String interfaceName = "";

    public String masterIp = "";

    public String slaveIp = "";

    public int depth = 0;

    public int width = 0;

    public int parentWidth = 0;

    public String getUnid()
    {
        return unid;
    }

    public void  setUnid(String unid)
    {
        this.unid = unid;
    }

    public String getMasterName()
    {
        return masterName;
    }

    public void  setMasterName(String masterName)
    {
        this.masterName = masterName;
    }

    public String getSlaveName()
    {
        return slaveName;
    }

    public void  setSlaveName(String slaveName)
    {
        this.slaveName = slaveName;
    }

    public String getInterfaceName()
    {
        return interfaceName;
    }

    public void  setInterfaceName(String interfaceName)
    {
        this.interfaceName = interfaceName;
    }

    public String getMasterIp()
    {
        return masterIp;
    }

    public void  setMasterIp(String masterIp)
    {
        this.masterIp = masterIp;
    }

    public String getSlaveIp()
    {
        return slaveIp;
    }

    public void  setSlaveIp(String slaveIp)
    {
        this.slaveIp = slaveIp;
    }

    public int getDepth()
    {
        return depth;
    }

    public void  setDepth(int depth)
    {
        this.depth = depth;
    }

    public int getWidth()
    {
        return width;
    }

    public void  setWidth(int width)
    {
        this.width = width;
    }

    public int getParentWidth()
    {
        return parentWidth;
    }

    public void  setParentWidth(int parentWidth)
    {
        this.parentWidth = parentWidth;
    }

    public StatSampleMsg()
    {
        setUnid(unid);
        setMasterName(masterName);
        setSlaveName(slaveName);
        setInterfaceName(interfaceName);
        setMasterIp(masterIp);
        setSlaveIp(slaveIp);
        setDepth(depth);
        setWidth(width);
        setParentWidth(parentWidth);
    }

    public StatSampleMsg(String unid, String masterName, String slaveName, String interfaceName, String masterIp, String slaveIp, int depth, int width, int parentWidth)
    {
        setUnid(unid);
        setMasterName(masterName);
        setSlaveName(slaveName);
        setInterfaceName(interfaceName);
        setMasterIp(masterIp);
        setSlaveIp(slaveIp);
        setDepth(depth);
        setWidth(width);
        setParentWidth(parentWidth);
    }

    public boolean equals(Object o)
    {
        StatSampleMsg t = (StatSampleMsg) o;
        return (
            com.qq.taf.jce.JceUtil.equals(unid, t.unid) && 
            com.qq.taf.jce.JceUtil.equals(masterName, t.masterName) && 
            com.qq.taf.jce.JceUtil.equals(slaveName, t.slaveName) && 
            com.qq.taf.jce.JceUtil.equals(interfaceName, t.interfaceName) && 
            com.qq.taf.jce.JceUtil.equals(masterIp, t.masterIp) && 
            com.qq.taf.jce.JceUtil.equals(slaveIp, t.slaveIp) && 
            com.qq.taf.jce.JceUtil.equals(depth, t.depth) && 
            com.qq.taf.jce.JceUtil.equals(width, t.width) && 
            com.qq.taf.jce.JceUtil.equals(parentWidth, t.parentWidth) );
    }

    public java.lang.Object clone()
    {
        java.lang.Object o = null;
        try
        {
            o = super.clone();
        }
        catch(CloneNotSupportedException ex)
        {
            assert false; // impossible
        }
        return o;
    }

    public void writeTo(com.qq.taf.jce.JceOutputStream _os)
    {
        _os.write(unid, 0);
        _os.write(masterName, 1);
        _os.write(slaveName, 2);
        _os.write(interfaceName, 3);
        _os.write(masterIp, 4);
        _os.write(slaveIp, 5);
        _os.write(depth, 6);
        _os.write(width, 7);
        _os.write(parentWidth, 8);
    }


    public void readFrom(com.qq.taf.jce.JceInputStream _is)
    {
        setUnid( _is.readString(0, true));

        setMasterName( _is.readString(1, true));

        setSlaveName( _is.readString(2, true));

        setInterfaceName( _is.readString(3, true));

        setMasterIp( _is.readString(4, true));

        setSlaveIp( _is.readString(5, true));

        setDepth((int) _is.read(depth, 6, true));

        setWidth((int) _is.read(width, 7, true));

        setParentWidth((int) _is.read(parentWidth, 8, true));

    }

    public void display(java.lang.StringBuilder _os, int _level)
    {
        com.qq.taf.jce.JceDisplayer _ds = new com.qq.taf.jce.JceDisplayer(_os, _level);
        _ds.display(unid, "unid");
        _ds.display(masterName, "masterName");
        _ds.display(slaveName, "slaveName");
        _ds.display(interfaceName, "interfaceName");
        _ds.display(masterIp, "masterIp");
        _ds.display(slaveIp, "slaveIp");
        _ds.display(depth, "depth");
        _ds.display(width, "width");
        _ds.display(parentWidth, "parentWidth");
    }

}

