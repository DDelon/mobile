// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 1.3.1 by WSRD Tencent.
// Generated from `./Log.jce'
// **********************************************************************

package com.qq.taf.server.proxy;

public final class LogPrxHelper extends com.qq.taf.proxy.ServantProxy implements LogPrx
{

    public LogPrxHelper taf_hash(int hashCode)
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

    public void logger(String app, String server, String file, String format, String[] buffer)
    {
        logger(app, server, file, format, buffer, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public void logger(String app, String server, String file, String format, String[] buffer, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(app, 1);
        _os.write(server, 2);
        _os.write(file, 3);
        _os.write(format, 4);
        _os.write(buffer, 5);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        java.util.HashMap<String, String> status = new java.util.HashMap<String, String>();

        byte[] _returnSBuffer = taf_invoke("logger", _sBuffer, __ctx, status);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        
    }

    public void async_logger(com.qq.taf.server.proxy.LogPrxCallback callback, String app, String server, String file, String format, String[] buffer)
    {
        async_logger(callback, app, server, file, format, buffer, __defaultContext());
    }

    public void async_logger(com.qq.taf.server.proxy.LogPrxCallback callback, String app, String server, String file, String format, String[] buffer, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(app, 1);
        _os.write(server, 2);
        _os.write(file, 3);
        _os.write(format, 4);
        _os.write(buffer, 5);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        java.util.HashMap<String, String> status = new java.util.HashMap<String, String>();

        taf_invokeAsync(callback, "logger", _sBuffer, __ctx, status);

    }

}
