// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.4.3 by WSRD Tencent.
// Generated from `Notify.jce'
// **********************************************************************

package com.qq.taf;

public final class NOTIFYLEVEL
{
    private static NOTIFYLEVEL[] __values = new NOTIFYLEVEL[3];
    private int __value;
    private String __T = new String();

    public static final int _NOTIFYNORMAL = 0;
    public static final NOTIFYLEVEL NOTIFYNORMAL = new NOTIFYLEVEL(0,_NOTIFYNORMAL,"NOTIFYNORMAL");
    public static final int _NOTIFYWARN = 1;
    public static final NOTIFYLEVEL NOTIFYWARN = new NOTIFYLEVEL(1,_NOTIFYWARN,"NOTIFYWARN");
    public static final int _NOTIFYERROR = 2;
    public static final NOTIFYLEVEL NOTIFYERROR = new NOTIFYLEVEL(2,_NOTIFYERROR,"NOTIFYERROR");

    public static NOTIFYLEVEL convert(int val)
    {
        for(int __i = 0; __i < __values.length; ++__i)
        {
            if(__values[__i].value() == val)
            {
                return __values[__i];
            }
        }
        assert false;
        return null;
    }

    public static NOTIFYLEVEL convert(String val)
    {
        for(int __i = 0; __i < __values.length; ++__i)
        {
            if(__values[__i].toString().equals(val))
            {
                return __values[__i];
            }
        }
        assert false;
        return null;
    }

    public int value()
    {
        return __value;
    }

    public String toString()
    {
        return __T;
    }

    private NOTIFYLEVEL(int index, int val, String s)
    {
        __T = s;
        __value = val;
        __values[index] = this;
    }

}
