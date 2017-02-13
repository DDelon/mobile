// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.5.7
// Generated from `Registry.jce'
// **********************************************************************

#ifndef __REGISTRY_H_
#define __REGISTRY_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;
#include "NodeDescriptor.h"
#include "RegistryDescriptor.h"
#include "servant/ServantProxy.h"
#include "servant/Servant.h"


namespace taf
{
    struct PatchResult : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "taf.PatchResult";
        }
        static string MD5()
        {
            return "c73c13678f156f762d622e8672930c1d";
        }
        PatchResult(const std::string &sApplication,const std::string &sServerName,const std::string &sNodeName,const std::string &sVersion,const std::string &sUserName)
            :sApplication(sApplication),sServerName(sServerName),sNodeName(sNodeName),sVersion(sVersion),sUserName(sUserName)
        {
        }
        PatchResult()
        :sApplication(""),sServerName(""),sNodeName(""),sVersion(""),sUserName("")
        {
        }
        void resetDefautlt()
        {
            sApplication = "";
            sServerName = "";
            sNodeName = "";
            sVersion = "";
            sUserName = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(sApplication, 0);
            _os.write(sServerName, 1);
            _os.write(sNodeName, 2);
            _os.write(sVersion, 3);
            _os.write(sUserName, 4);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(sApplication, 0, true);
            _is.read(sServerName, 1, true);
            _is.read(sNodeName, 2, true);
            _is.read(sVersion, 3, true);
            _is.read(sUserName, 4, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(sApplication,"sApplication");
            _ds.display(sServerName,"sServerName");
            _ds.display(sNodeName,"sNodeName");
            _ds.display(sVersion,"sVersion");
            _ds.display(sUserName,"sUserName");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(sApplication, true);
            _ds.displaySimple(sServerName, true);
            _ds.displaySimple(sNodeName, true);
            _ds.displaySimple(sVersion, true);
            _ds.displaySimple(sUserName, false);
            return _os;
        }
    public:
        std::string sApplication;
        std::string sServerName;
        std::string sNodeName;
        std::string sVersion;
        std::string sUserName;
    };
    inline bool operator==(const PatchResult&l, const PatchResult&r)
    {
        return l.sApplication == r.sApplication && l.sServerName == r.sServerName && l.sNodeName == r.sNodeName && l.sVersion == r.sVersion && l.sUserName == r.sUserName;
    }
    inline bool operator!=(const PatchResult&l, const PatchResult&r)
    {
        return !(l == r);
    }


    /* callback of async proxy for client */
    class RegistryPrxCallback: public taf::ServantProxyCallback
    {
    public:
        virtual ~RegistryPrxCallback(){}
        virtual void callback_registerNode(taf::Int32 ret)
        { throw std::runtime_error("callback_registerNode() overloading incorrect."); }
        virtual void callback_registerNode_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_registerNode_exception() overloading incorrect."); }

        virtual void callback_keepAlive(taf::Int32 ret)
        { throw std::runtime_error("callback_keepAlive() overloading incorrect."); }
        virtual void callback_keepAlive_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_keepAlive_exception() overloading incorrect."); }

        virtual void callback_getServers(const vector<taf::ServerDescriptor>& ret)
        { throw std::runtime_error("callback_getServers() overloading incorrect."); }
        virtual void callback_getServers_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_getServers_exception() overloading incorrect."); }

        virtual void callback_updateServer(taf::Int32 ret)
        { throw std::runtime_error("callback_updateServer() overloading incorrect."); }
        virtual void callback_updateServer_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_updateServer_exception() overloading incorrect."); }

        virtual void callback_updateServerBatch(taf::Int32 ret)
        { throw std::runtime_error("callback_updateServerBatch() overloading incorrect."); }
        virtual void callback_updateServerBatch_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_updateServerBatch_exception() overloading incorrect."); }

        virtual void callback_destroy(taf::Int32 ret)
        { throw std::runtime_error("callback_destroy() overloading incorrect."); }
        virtual void callback_destroy_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_destroy_exception() overloading incorrect."); }

        virtual void callback_reportVersion(taf::Int32 ret)
        { throw std::runtime_error("callback_reportVersion() overloading incorrect."); }
        virtual void callback_reportVersion_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_reportVersion_exception() overloading incorrect."); }

        virtual void callback_getNodeTemplate(taf::Int32 ret, const std::string& profileTemplate)
        { throw std::runtime_error("callback_getNodeTemplate() overloading incorrect."); }
        virtual void callback_getNodeTemplate_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_getNodeTemplate_exception() overloading incorrect."); }

        virtual void callback_getClientIp(taf::Int32 ret, const std::string& sClientIp)
        { throw std::runtime_error("callback_getClientIp() overloading incorrect."); }
        virtual void callback_getClientIp_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_getClientIp_exception() overloading incorrect."); }

        virtual void callback_updatePatchResult(taf::Int32 ret)
        { throw std::runtime_error("callback_updatePatchResult() overloading incorrect."); }
        virtual void callback_updatePatchResult_exception(taf::Int32 ret)
        { throw std::runtime_error("callback_updatePatchResult_exception() overloading incorrect."); }

    public:
        int onDispatch(taf::ReqMessagePtr msg);
    };
    typedef taf::TC_AutoPtr<RegistryPrxCallback> RegistryPrxCallbackPtr;

    /* proxy for client */
    class RegistryProxy : public taf::ServantProxy
    {
    public:
        typedef map<string, string> TAF_CONTEXT;

        taf::Int32 registerNode(const std::string & nodeName,const taf::NodeInfo & ni,const taf::LoadInfo & li,const map<string, string> &context = TAF_CONTEXT());
        void async_registerNode(RegistryPrxCallbackPtr callback,const std::string & nodeName,const taf::NodeInfo & ni,const taf::LoadInfo & li,const map<string, string> &context = TAF_CONTEXT());

        taf::Int32 keepAlive(const std::string & nodeName,const taf::LoadInfo & load,const map<string, string> &context = TAF_CONTEXT());
        void async_keepAlive(RegistryPrxCallbackPtr callback,const std::string & nodeName,const taf::LoadInfo & load,const map<string, string> &context = TAF_CONTEXT());

        vector<taf::ServerDescriptor> getServers(const std::string & app,const std::string & serverName,const std::string & nodeName,const map<string, string> &context = TAF_CONTEXT());
        void async_getServers(RegistryPrxCallbackPtr callback,const std::string & app,const std::string & serverName,const std::string & nodeName,const map<string, string> &context = TAF_CONTEXT());

        taf::Int32 updateServer(const std::string & app,const std::string & serverName,const std::string & nodeName,const taf::ServerStateInfo & state,const map<string, string> &context = TAF_CONTEXT());
        void async_updateServer(RegistryPrxCallbackPtr callback,const std::string & app,const std::string & serverName,const std::string & nodeName,const taf::ServerStateInfo & state,const map<string, string> &context = TAF_CONTEXT());

        taf::Int32 updateServerBatch(const vector<taf::ServerStateInfo> & vState,const map<string, string> &context = TAF_CONTEXT());
        void async_updateServerBatch(RegistryPrxCallbackPtr callback,const vector<taf::ServerStateInfo> & vState,const map<string, string> &context = TAF_CONTEXT());

        taf::Int32 destroy(const std::string & nodeName,const map<string, string> &context = TAF_CONTEXT());
        void async_destroy(RegistryPrxCallbackPtr callback,const std::string & nodeName,const map<string, string> &context = TAF_CONTEXT());

        taf::Int32 reportVersion(const std::string & app,const std::string & serverName,const std::string & nodeName,const std::string & version,const map<string, string> &context = TAF_CONTEXT());
        void async_reportVersion(RegistryPrxCallbackPtr callback,const std::string & app,const std::string & serverName,const std::string & nodeName,const std::string & version,const map<string, string> &context = TAF_CONTEXT());

        taf::Int32 getNodeTemplate(const std::string & nodeName,std::string &profileTemplate,const map<string, string> &context = TAF_CONTEXT());
        void async_getNodeTemplate(RegistryPrxCallbackPtr callback,const std::string & nodeName,const map<string, string> &context = TAF_CONTEXT());

        taf::Int32 getClientIp(std::string &sClientIp,const map<string, string> &context = TAF_CONTEXT());
        void async_getClientIp(RegistryPrxCallbackPtr callback,const map<string, string> &context = TAF_CONTEXT());

        taf::Int32 updatePatchResult(const taf::PatchResult & result,const map<string, string> &context = TAF_CONTEXT());
        void async_updatePatchResult(RegistryPrxCallbackPtr callback,const taf::PatchResult & result,const map<string, string> &context = TAF_CONTEXT());

        RegistryProxy* taf_hash(int64_t key);
    };
    typedef taf::TC_AutoPtr<RegistryProxy> RegistryPrx;

    /* servant for server */
    class Registry : public taf::Servant
    {
    public:
        virtual ~Registry(){}
        virtual taf::Int32 registerNode(const std::string & nodeName,const taf::NodeInfo & ni,const taf::LoadInfo & li,taf::JceCurrentPtr current) = 0;
        static void async_response_registerNode(taf::JceCurrentPtr current, taf::Int32 _ret);

        virtual taf::Int32 keepAlive(const std::string & nodeName,const taf::LoadInfo & load,taf::JceCurrentPtr current) = 0;
        static void async_response_keepAlive(taf::JceCurrentPtr current, taf::Int32 _ret);

        virtual vector<taf::ServerDescriptor> getServers(const std::string & app,const std::string & serverName,const std::string & nodeName,taf::JceCurrentPtr current) = 0;
        static void async_response_getServers(taf::JceCurrentPtr current, const vector<taf::ServerDescriptor> &_ret);

        virtual taf::Int32 updateServer(const std::string & app,const std::string & serverName,const std::string & nodeName,const taf::ServerStateInfo & state,taf::JceCurrentPtr current) = 0;
        static void async_response_updateServer(taf::JceCurrentPtr current, taf::Int32 _ret);

        virtual taf::Int32 updateServerBatch(const vector<taf::ServerStateInfo> & vState,taf::JceCurrentPtr current) = 0;
        static void async_response_updateServerBatch(taf::JceCurrentPtr current, taf::Int32 _ret);

        virtual taf::Int32 destroy(const std::string & nodeName,taf::JceCurrentPtr current) = 0;
        static void async_response_destroy(taf::JceCurrentPtr current, taf::Int32 _ret);

        virtual taf::Int32 reportVersion(const std::string & app,const std::string & serverName,const std::string & nodeName,const std::string & version,taf::JceCurrentPtr current) = 0;
        static void async_response_reportVersion(taf::JceCurrentPtr current, taf::Int32 _ret);

        virtual taf::Int32 getNodeTemplate(const std::string & nodeName,std::string &profileTemplate,taf::JceCurrentPtr current) = 0;
        static void async_response_getNodeTemplate(taf::JceCurrentPtr current, taf::Int32 _ret, const std::string &profileTemplate);

        virtual taf::Int32 getClientIp(std::string &sClientIp,taf::JceCurrentPtr current) = 0;
        static void async_response_getClientIp(taf::JceCurrentPtr current, taf::Int32 _ret, const std::string &sClientIp);

        virtual taf::Int32 updatePatchResult(const taf::PatchResult & result,taf::JceCurrentPtr current) = 0;
        static void async_response_updatePatchResult(taf::JceCurrentPtr current, taf::Int32 _ret);

    public:
        int onDispatch(taf::JceCurrentPtr _current, vector<char> &_sResponseBuffer);
    };


}

#define taf_PatchResult_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.sApplication,b.sApplication);jce_copy_struct(a.sServerName,b.sServerName);jce_copy_struct(a.sNodeName,b.sNodeName);jce_copy_struct(a.sVersion,b.sVersion);jce_copy_struct(a.sUserName,b.sUserName);



#endif