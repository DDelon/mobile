// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 1.1.7 by WSRD Tencent.
// Generated from `NodeF.jce'
// **********************************************************************

package com.qq.taf.server.proxy;

public final class ServerFPrxHelper extends com.qq.taf.proxy.ServantProxy implements ServerFPrx
{

    public ServerFPrxHelper taf_hash(int hashCode)
    {
        super.taf_hash(hashCode);
        return this;
    }

    public java.util.Map __defaultContext()
    {
        java.util.HashMap _ctx = new java.util.HashMap();
        return _ctx;
    }

    protected String sServerEncoding = "GBK";
    public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }

    public int keepAlive(com.qq.taf.server.proxy.ServerInfo serverInfo)
    {
        return keepAlive(serverInfo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int keepAlive(com.qq.taf.server.proxy.ServerInfo serverInfo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(serverInfo, 1);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        java.util.HashMap<String, String> status = new java.util.HashMap<String, String>();

        byte[] _returnSBuffer = taf_invoke("keepAlive", _sBuffer, __ctx, status);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        return _ret;
    }

    public void async_keepAlive(com.qq.taf.server.proxy.ServerFPrxCallback callback, com.qq.taf.server.proxy.ServerInfo serverInfo)
    {
        async_keepAlive(callback, serverInfo, __defaultContext());
    }

    public void async_keepAlive(com.qq.taf.server.proxy.ServerFPrxCallback callback, com.qq.taf.server.proxy.ServerInfo serverInfo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(serverInfo, 1);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        java.util.HashMap<String, String> status = new java.util.HashMap<String, String>();

        taf_invokeAsync(callback, "keepAlive", _sBuffer, __ctx, status);

    }

    public int reportVersion(String app, String serverName, String version)
    {
        return reportVersion(app, serverName, version, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int reportVersion(String app, String serverName, String version, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(app, 1);
        _os.write(serverName, 2);
        _os.write(version, 3);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        java.util.HashMap<String, String> status = new java.util.HashMap<String, String>();

        byte[] _returnSBuffer = taf_invoke("reportVersion", _sBuffer, __ctx, status);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        return _ret;
    }

    public void async_reportVersion(com.qq.taf.server.proxy.ServerFPrxCallback callback, String app, String serverName, String version)
    {
        async_reportVersion(callback, app, serverName, version, __defaultContext());
    }

    public void async_reportVersion(com.qq.taf.server.proxy.ServerFPrxCallback callback, String app, String serverName, String version, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(app, 1);
        _os.write(serverName, 2);
        _os.write(version, 3);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        java.util.HashMap<String, String> status = new java.util.HashMap<String, String>();

        taf_invokeAsync(callback, "reportVersion", _sBuffer, __ctx, status);

    }

}
